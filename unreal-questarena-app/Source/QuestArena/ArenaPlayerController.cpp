// Fill out your copyright notice in the Description page of Project Settings.

//Quest Arena
#include "ArenaPlayerController.h"
#include "ServerGameMode.h"
#include "PlayerStartPosition.h"
#include "ArenaHeadsetConfig.h"
#include "StartManager.h"
#include "IPickable.h"
#include "ArenaPlayerState.h"
#include "DevelopmentConfig.h"

//Engine
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


#if UE_EDITOR
const int32 AArenaPlayerController::MAX_NUMBER_OF_PLAYERS = 4;
#endif

//------------------------------------C-style methods
FLinearColor PlayerColorTable(uint8 ArenaPlayerId)
{
	switch (ArenaPlayerId)
	{
	case 0: return FLinearColor::Red;
	case 1: return FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); //::Cyan
	case 2: return FLinearColor::Green;
	case 3: return FLinearColor::Yellow;
	default: return FLinearColor::Black;
	}
}

//------------------------------------AArenaPlayerController

void AArenaPlayerController::Setter_UserDetails(UUserSetupDetails* NewDetails)
{
	UserDetails = NewDetails;
	auto startPositions = AStartManager::GetSingleton(this)->GetAvailableStartPositions();
	if (startPositions.Num() > UserDetails->GetPos())
	{
		auto startPos = startPositions[UserDetails->GetPos()];
		AcknowledgedPawn->SetActorLocation(startPos->GetActorLocation());
		AcknowledgedPawn->SetActorRotation(startPos->GetActorRotation());
	}
}
 
void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	#if WITH_EDITOR
	// This code will assign ArenaPlayerId with PIE and multiple instances in mind. To avoid HID collision while using editor/OculusLink.
	if (GetLocalRole() == ROLE_AutonomousProxy || (GetLocalRole() == ROLE_Authority && !UKismetSystemLibrary::IsDedicatedServer(this)))
	{
		static uint8 tmpPlayerId = -1; //0 after first incrementation
		tmpPlayerId = (tmpPlayerId + 1) % MAX_NUMBER_OF_PLAYERS;

		ArenaPlayerId = tmpPlayerId;
		ClientToServer_Handshake(ArenaPlayerId);
	}
	#else
	if (GetLocalRole() == ROLE_AutonomousProxy) //call from owner client to server
	{
		ArenaPlayerId = UArenaHeadsetConfig::GetSaveObject()->GetHeadsetId();
		ClientToServer_Handshake(ArenaPlayerId);
	}
	#endif
}

bool AArenaPlayerController::Server_SpawnPlayer()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Error, TEXT("Non-server attempted to spawn player"));
		return false;
	}

	AArenaPlayerState* APS = Cast<AArenaPlayerState>(PlayerState);
	check(APS);

	switch(APS->AliveState)
	{
	case EPlayerAliveState::NotSpawnedYet:
	case EPlayerAliveState::Ghost:

		UE_LOG(LogTemp, Error, TEXT("Spawning player"));
		APS->AliveState = EPlayerAliveState::Alive;
		APS->OnPlayerAliveStateChanged.Broadcast(); //is also broadcasted on clients inside OnRep
		return true;

	default:

		UE_LOG(LogTemp, Error, TEXT("Attempted to spawn a player that is either alive or irreversibly dead"));
		return false;
	}
}

bool AArenaPlayerController::Server_KillPlayer(bool Permanent)
{
	if(GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Error, TEXT("Non-server attempted to kill player"));
		return false;
	}

	AArenaPlayerState* APS = Cast<AArenaPlayerState>(PlayerState);
	check(APS);

	if (APS->AliveState != EPlayerAliveState::Alive)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempted to kill a player that is not alive"));
		return false;
	}

	APS->AliveState = Permanent ? EPlayerAliveState::GameOver : EPlayerAliveState::Ghost;
	APS->OnPlayerAliveStateChanged.Broadcast(); //is also broadcasted on clients inside OnRep
	
	//this runs only on server so it's safe to get gamemode
	AServerGameMode* SGM = Cast<AServerGameMode>(UGameplayStatics::GetGameMode(this));
	check(SGM->IsValidLowLevel());
	SGM->MVP_OnPlayerDied();

	return true;
}

void AArenaPlayerController::ClientToServer_Handshake_Implementation(uint8 TargetPlayerId)
{
	AArenaPlayerState* APS = Cast<AArenaPlayerState>(PlayerState);
	check(APS);

	APS->ArenaPlayerId = TargetPlayerId;
	APS->ArenaPlayerColor = PlayerColorTable(TargetPlayerId); /* @TODO: read client color from arcade controller app */
	APS->ArenaPlayerName = FString::Printf(TEXT("jacek %d"), (int32)TargetPlayerId); /* @TODO: read client name from arcade controller app */

	//Handshake is confirmed when IngameState changes; otherwise client is disconnected immediately through _Validate()
	APS->IngameState = EPlayerStateInGame::PostHandshake;

	auto StartPositions = AStartManager::GetSingleton(this)->GetAvailableStartPositions();
	if(StartPositions.Num() > TargetPlayerId && GetPawn())
	{
		ServerToClient_Handshake((StartPositions[TargetPlayerId])->GetActorLocation(), (StartPositions[TargetPlayerId])->GetActorRotation());
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Something went wrong. Either there are no spawn points in this level or GetPawn() method return nullptr."));
	}
}

bool AArenaPlayerController::ClientToServer_Handshake_Validate(uint8 TargetPlayerId)
{
	//runs only on server so it's safe to get gamemode
	AServerGameMode* SGM = Cast<AServerGameMode>(UGameplayStatics::GetGameMode(this));
	check(SGM->IsValidLowLevel());

	uint8 UnrealPlayerId;
	if (SGM->RegisterPlayer(this, TargetPlayerId, UnrealPlayerId))
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully registered player %d as HeadsetID %d"),
			(int32)UnrealPlayerId,
			(int32)TargetPlayerId);
		return true;
	}

	UE_LOG(LogTemp, Log, TEXT("Registration fo player %d as HeadsetID %d failed"),
		(int32)UnrealPlayerId,
		(int32)TargetPlayerId);

	return false;
}

void AArenaPlayerController::ServerToClient_Handshake_Implementation(FVector StartPos, FRotator StartRot)
{
	if(GetPawn())
	{
		GetPawn()->SetActorLocation(StartPos);
		GetPawn()->SetActorRotation(StartRot);
	}

	// @todo: actually make players mark themselves as ready instead of calling it directly after handshake
	//ClientToServer_MarkReadyToPlay();
}

bool AArenaPlayerController::ClientToServer_MarkReadyToPlay_Validate()
{
	return true;
}

void AArenaPlayerController::ClientToServer_MarkReadyToPlay_Implementation()
{
	AArenaPlayerState* APS = Cast<AArenaPlayerState>(PlayerState);
	check(APS);
	if (APS->IngameState != EPlayerStateInGame::PostHandshake)
	{
		UE_LOG(LogTemp, Log, TEXT("Can't mark player ReadyToPlay because they are either not registered or already ingame"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Successfully marked player as ReadyToPlay!"));
	APS->IngameState = EPlayerStateInGame::ReadyToPlay;

	//@todo: on MVP game launches immedately when everybody is ready to play through this code,
	//       but in the final version this should not be the case.

	//this runs only on server so it's safe to get gamemode
	AServerGameMode* SGM = Cast<AServerGameMode>(UGameplayStatics::GetGameMode(this));
	check(SGM->IsValidLowLevel());

	const int32 TargetNumPlayers = UDevelopmentConfig::GetDebugTargetNumPlayers();
	if (SGM->RegisteredPlayers.Num() == TargetNumPlayers && SGM->CanStartGame())
	{
		UE_LOG(LogTemp, Warning, TEXT("AArenaPlayerController::ClientToServer_MarkReadyToPlay_Implementation(): Development setting activated: auto start game because target num players criteria was met (%d registered players)"), TargetNumPlayers);
		SGM->StartGame();
	}
}

//------------------------------------MVP RESTART

void AArenaPlayerController::ClientToServer_RestartRequested_Implementation()
{
	//received client request to restart on server. Pass it to GameMode
	AServerGameMode* SGM = Cast<AServerGameMode>(UGameplayStatics::GetGameMode(this));
	SGM->MVP_OnRestartRequested();
}

//----------------------------------STAT MANAGEMENT

void AArenaPlayerController::AskServerForStats(const FOnStatsReceived& Delegate)
{
	OnStatsReceived = Delegate;
	ClientToServer_AskForStats(this);
}

void AArenaPlayerController::ClientToServer_AskForStats_Implementation(AArenaPlayerController* AskingController)
{
	AServerGameMode* SGM = Cast<AServerGameMode>(UGameplayStatics::GetGameMode(this));
	check(SGM->IsValidLowLevel());

	FGameStats Stats;
	UStatManager::FetchGameStatsForDisplay(SGM, AskingController, Stats);
	ServerToClient_SendStats(Stats);
}

void AArenaPlayerController::ServerToClient_SendStats_Implementation(FGameStats Stats)
{
	OnStatsReceived.ExecuteIfBound(Stats);
}

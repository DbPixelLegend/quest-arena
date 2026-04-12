// Fill out your copyright notice in the Description page of Project Settings.

//Quest Arena
#include "ServerGameMode.h"
#include "ArenaGameSession.h"
#include "ArenaGameState.h"
#include "ArenaPlayerController.h"
#include "ArenaPlayerState.h"
#include "DevelopmentConfig.h"
#include "VRPawn.h"
#include "Resetable.h"
#include "StatFields.h"

//Engine
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

AServerGameMode::AServerGameMode(const FObjectInitializer& OI)
	: MinNumPlayers(1)
	, MaxNumPlayers(8)
	, GlobalStatFields({ GAME_TIME })
	, PerUserStatFields({ USER_NAME, USER_HID })
{
	bPauseable = false;
	bStartPlayersAsSpectators = false; //i mean we kind of do want that, but players should control motion controllers from the start
	bUseSeamlessTravel = false;

	DefaultPawnClass = AVRPawn::StaticClass();
	GameSessionClass = AArenaGameSession::StaticClass();
	GameStateClass = AArenaGameState::StaticClass();
	PlayerControllerClass = AArenaPlayerController::StaticClass();
	PlayerStateClass = AArenaPlayerState::StaticClass();
	ReplaySpectatorPlayerControllerClass = AArenaPlayerController::StaticClass();
}

AServerGameMode::~AServerGameMode()
{

}

void AServerGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AServerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UnregisteredPlayers.Add(NewPlayer);
}

void AServerGameMode::Logout(AController* Exiting)
{
	APlayerController* PC = Cast<APlayerController>(Exiting);
	check(PC);

	int32 UnregisteredIdx = UnregisteredPlayers.Find(PC);
	if (UnregisteredIdx != INDEX_NONE)
	{
		UnregisteredPlayers.RemoveAt(UnregisteredIdx);
		UE_LOG(LogTemp, Log, TEXT("GameMode Logout(): removed unregistered player"));
		Super::Logout(Exiting);
		return;
	}
	
	const uint8* Key = RegisteredPlayers.FindKey(PC);
	if (Key != nullptr)
	{
		RegisteredPlayers.Remove(*Key);
		UE_LOG(LogTemp, Log, TEXT("GameMode Logout(): removed registered player with arena id %d"), (int32)(*Key));
		Super::Logout(Exiting);
		return;
	}

	ensure(false);
	UE_LOG(LogTemp, Warning, TEXT("GameMode Logout(): removed player with unknown registration status!"));
	Super::Logout(Exiting);
}

bool AServerGameMode::RegisterPlayer(APlayerController* PC, uint8 ArenaPlayerId, uint8& UnrealPlayerId)
{
	AArenaGameState* AGS = Cast<AArenaGameState>(GameState);
	check(AGS->IsValidLowLevel());
	if (AGS->GetCurrentGameState() != ECurrentGameState::GatheringPlayers && !UDevelopmentConfig::GetDebugAllowLateJoiners())
	{
		UE_LOG(LogTemp, Error, TEXT("AServerGameMode::RegisterPlayer(): Can't register player - the game session has already started!"));
		return false;
	}

	check(PC->IsValidLowLevel());

	int32 UnregisteredIdx = UnregisteredPlayers.Find(PC);
	if (UnregisteredIdx == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("AServerGameMode::RegisterPlayer(): Can't register player - not found in unregistered player library"));
		return false;
	}

	APlayerController** RegisteredPC = RegisteredPlayers.Find(ArenaPlayerId);
	if (RegisteredPC != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AServerGameMode::RegisterPlayer(): Can't register player - another player with this ArenaID exists"));
		return false;
	}

	UnregisteredPlayers.RemoveAt(UnregisteredIdx);
	RegisteredPlayers.Add(ArenaPlayerId, PC);

	UnrealPlayerId = PC->NetPlayerIndex; // @todo: this is not used anywhere but always returns 0, this is an insignificant error
	UE_LOG(LogTemp, Error, TEXT("AServerGameMode::RegisterPlayer(): Registered successfully"));

	return true;
}

bool AServerGameMode::CanStartGame()
{
	for (auto& Pair : RegisteredPlayers)
	{
		AArenaPlayerState* APS = Cast<AArenaPlayerState>(Pair.Value->PlayerState);
		if (APS->IngameState != EPlayerStateInGame::ReadyToPlay)
		{
			UE_LOG(LogTemp, Error, TEXT("Can't start game - not all players are ReadyToPlay"));
			return false;
		}
	}

	AArenaGameState* AGS = Cast<AArenaGameState>(GameState);
	check(AGS->IsValidLowLevel());
	if (AGS->GetCurrentGameState() != ECurrentGameState::GatheringPlayers)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't start game - server is no longer gathering players"));
		return false;
	}

	if (UnregisteredPlayers.Num() != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't start game - there are unregistered players connected"));
		return false;
	}
	if (RegisteredPlayers.Num() > MaxNumPlayers)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't start game - there are more players than max allowed"));
		return false;
	}
	if (RegisteredPlayers.Num() < MinNumPlayers)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't start game - there are less players than min allowed"));
		return false;
	}

	return true;
}

bool AServerGameMode::CanEndGame(bool& bIsGameConsideredWon)
{
	AArenaGameState* AGS = Cast<AArenaGameState>(GameState);
	check(AGS->IsValidLowLevel());

	bool bCanGameBeEnded;
	AGS->IsGameEndConditionSatisfied(bCanGameBeEnded, bIsGameConsideredWon);
	return AGS->GetCurrentGameState() == ECurrentGameState::InGame && bCanGameBeEnded;
}

bool AServerGameMode::StartGame()
{
	if (!CanStartGame())
	{
		UE_LOG(LogTemp, Error, TEXT("AServerGameMode::StartGame(): can't start game (not all start conditions were met)!"));
		return false;
	}

	RestartGame();

	AArenaGameState* AGS = Cast<AArenaGameState>(GameState);
	AGS->Multicast_SetGameState(ECurrentGameState::InGame);
	for (auto& Pair : RegisteredPlayers)
	{
		AArenaPlayerState* APS = Cast<AArenaPlayerState>(Pair.Value->PlayerState);
		APS->IngameState = EPlayerStateInGame::Ingame;
	}
	OnGameStarted.Broadcast();
	UE_LOG(LogTemp, Error, TEXT("AServerGameMode::StartGame(): game STARTED!"));
	return true;
}

bool AServerGameMode::EndGame()
{
	//can we end the game?
	bool bWasGameWon;
	if (!CanEndGame(bWasGameWon))
	{
		UE_LOG(LogTemp, Error, TEXT("AServerGameMode::EndGame(): can't end game (not all start conditions were met)!"));
		return false;
	}

	//mark game as game over
	AArenaGameState* AGS = Cast<AArenaGameState>(GameState);
	AGS->Multicast_SetGameState(ECurrentGameState::GameOver);
	OnGameEnded.Broadcast(bWasGameWon);
	UE_LOG(LogTemp, Error, TEXT("AServerGameMode::EndGame(): game ENDED!"));

	//users are presented with an option to restart through ArenaGameState::OnGameStateChanged().
	//that means that the button to reset game is presented and clickable. Allow restart when requested:
	bRestartButtonEnabled = true;
	
	return true;
}

void AServerGameMode::RestartGame()
{
	TArray<AActor*> RestartableActors;
	UGameplayStatics::GetAllActorsWithInterface(this, UResetable::StaticClass(), RestartableActors);

	bool bRestarted = true;
	for (AActor* Actor : RestartableActors)
	{
		bool bThisActorRestarted = IResetable::Execute_ServerRestart(Actor);
		bRestarted = bRestarted && bThisActorRestarted;
		if (!bThisActorRestarted)
		{
			UE_LOG(LogTemp, Log, TEXT("Actor %s failed to restart successfully."), *(Actor->GetName()));
		}
	}

	if (bRestarted)
	{
		UE_LOG(LogTemp, Log, TEXT("RestartGame executed!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RestartGame executed but not all actors restarted successfully. See above for details."));
	}
}

void AServerGameMode::MVP_OnPlayerDied()
{
	//try to end game on each player death
	EndGame();

	//if the game ended (EndGame() returned true), then at this point all players
	//are presented with a restart button. When one of the players presses the 
	//button, MVP_OnPlayerDied_Part2() will be executed.
}

void AServerGameMode::MVP_OnRestartRequested()
{
	if (!bRestartButtonEnabled) return;

	//game state will now change to GatheringPlayers, which means the Reset button
	//is no longer presented to users. Disable ability to restart:
	bRestartButtonEnabled = false;

	for (auto& Pair : RegisteredPlayers)
	{
		AArenaPlayerController* APC = Cast<AArenaPlayerController>(Pair.Value);
		AArenaPlayerState* APS = Cast<AArenaPlayerState>(APC->PlayerState);
		APS->RestartForNextSession();
	}

	AArenaGameState* AGS = Cast<AArenaGameState>(GameState);
	AGS->Multicast_SetGameState(ECurrentGameState::GatheringPlayers);

	//at this point players marked themselves as PostHandshake & NotSpawnedYet.
	//The game will start again when all players mark themselves ready.
}

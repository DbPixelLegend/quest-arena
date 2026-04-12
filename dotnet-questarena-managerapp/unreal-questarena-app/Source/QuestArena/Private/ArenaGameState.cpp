// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameState.h"
#include "ServerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "NetworkSerializable.h"
#include "ArenaPlayerState.h"
#include "UnrealNetwork.h"

AArenaGameState::AArenaGameState(const FObjectInitializer& OI)
	: CurrentGameState(ECurrentGameState::GatheringPlayers)
	, GameStartedTime(FDateTime::UtcNow()) //@note: this is not correct, but a nice failsafe
{ }

AArenaGameState::~AArenaGameState()
{ }

void AArenaGameState::BeginPlay()
{
	OnGameStateChanged(); //Because initial ECurrentGameState::GatheringPlayers is not multicasted and we need the broadcast to init game
}

//void AArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	DOREPLIFETIME(AArenaGameState, CurrentGameState);
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//}

void AArenaGameState::Multicast_SetGameState_Implementation(ECurrentGameState NewState)
{
	if (CurrentGameState == NewState) return;

	CurrentGameState = NewState;

	if (CurrentGameState == ECurrentGameState::InGame && Role == ROLE_Authority)
	{
		GameStartedTime = FDateTime::UtcNow();
	}

	OnGameStateChanged();
}

void AArenaGameState::OnGameStateChanged_Implementation() { }

void AArenaGameState::IsGameEndConditionSatisfied_Implementation(bool& CanEndGame, bool& GameWon)
{
	//this runs only on server so it's safe to get gamemode
	AServerGameMode* SGM = Cast<AServerGameMode>(UGameplayStatics::GetGameMode(this));
	check(SGM->IsValidLowLevel());

	bool bAlivePlayersExist = false;
	for (auto& Pair : SGM->RegisteredPlayers)
	{
		AArenaPlayerState* APS = Cast<AArenaPlayerState>(Pair.Value->PlayerState);
		if (APS->AliveState == EPlayerAliveState::GameOver) continue;
		bAlivePlayersExist = true;
		break;
	}

	CanEndGame = !bAlivePlayersExist;
	GameWon = false;
}

bool AArenaGameState::FetchStat_Implementation(FString& Stat, FString& Value)
{
	if (Stat.Equals(GAME_TIME))
	{
		const TCHAR* WithHours = TEXT("%h:%m:%s");
		const TCHAR* NoHours   = TEXT(   "%m:%s");

		FTimespan GameTime = FDateTime::UtcNow() - GameStartedTime;
		Value = GameTime.ToString(GameTime.GetTotalHours() >= 1.0f ? WithHours : NoHours);
		return true;
	}

	return false;
}

bool AArenaGameState::ServerRestart_Implementation()
{
	if (!UKismetSystemLibrary::IsServer(this)) //clients can't reset anything
	{
		ensureMsgf(false, TEXT("You can't call IResetable::Reset() on clients."));
		return false;
	}

	//note: this is what you should NOT do here, GameState changes as soon as previous game ended:
	//Multicast_SetGameState(ECurrentGameState::GatheringPlayers);

	return true;
}

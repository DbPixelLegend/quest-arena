// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaPlayerState.h"
#include "ArenaHeadsetConfig.h"
#include "ArenaPlayerController.h"
#include "StatFields.h"

#include "NetworkSerializable.h"
#include "UnrealNetwork.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"


//------------------------------------Actor lifecycle

AArenaPlayerState::AArenaPlayerState(const FObjectInitializer& ObjectInitializer)
	: ArenaPlayerId(255)
	, ArenaPlayerColor(FLinearColor::Black)
	, ArenaPlayerName(FString())
	, IngameState(EPlayerStateInGame::PreHandshake)
	, OptionalPC(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	PlayerHealth = CreateDefaultSubobject<UHealthComponent>("PlayerHealth"); //replicates by default
	PlayerHealth->MaxHealth = 1.0f;
	PlayerHealth->HealthLeft = 1.0f;
	PlayerHealth->SetIsReplicated(true);
}

void AArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AArenaPlayerState, ArenaPlayerId);
	DOREPLIFETIME(AArenaPlayerState, ArenaPlayerColor);
	DOREPLIFETIME(AArenaPlayerState, ArenaPlayerName);
	DOREPLIFETIME(AArenaPlayerState, IngameState);
	DOREPLIFETIME(AArenaPlayerState, AliveState);
	DOREPLIFETIME(AArenaPlayerState, PlayerHealth);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AArenaPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AArenaPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AArenaPlayerState::~AArenaPlayerState()
{

}

bool AArenaPlayerState::FetchStat_Implementation(FString& Stat, FString& Value)
{
	if (Stat.Equals(USER_NAME))
	{
		Value = ArenaPlayerName;
		return true;
	}
	if (Stat.Equals(USER_HID))
	{
		Value = FString::FromInt((int32)ArenaPlayerId);
		return true;
	}

	return false;
}

//------------------------------------Virtual OnReps

void AArenaPlayerState::OnRep_ArenaPlayerId_Implementation()
{
	if (ArenaPlayerId == 255) return;
	
	for (TActorIterator<AArenaPlayerController> It(GetWorld()); It; ++It)
	{
		if (It->ArenaPlayerId != ArenaPlayerId) continue;
		OptionalPC = *It;
		return;
	}
}
void AArenaPlayerState::OnRep_ArenaPlayerColor_Implementation() { }
void AArenaPlayerState::OnRep_ArenaPlayerName_Implementation() { }
void AArenaPlayerState::OnRep_IngameState_Implementation() { OnPlayerIngameStateChanged.Broadcast(); }
void AArenaPlayerState::OnRep_AliveState_Implementation() { OnPlayerAliveStateChanged.Broadcast(); }

bool AArenaPlayerState::IsLocalPlayerState()
{
	//nullptr is kinda pointless but generates less spam in output log
	return (OptionalPC != nullptr && OptionalPC->IsValidLowLevel());
}

void AArenaPlayerState::RestartForNextSession_Implementation()
{
	if (!UKismetSystemLibrary::IsServer(this)) //clients can't reset anything
	{
		ensureMsgf(false, TEXT("You can't call AArenaPlayerState::RestartForNextSession() on clients."));
		return;
	}

	//don't reset HID, name and color since we return to the POST-handshake phase
	IngameState = EPlayerStateInGame::PostHandshake;
	AliveState = EPlayerAliveState::NotSpawnedYet;
	PlayerHealth->ResetHealth(); //restore initial health values
	return;
}

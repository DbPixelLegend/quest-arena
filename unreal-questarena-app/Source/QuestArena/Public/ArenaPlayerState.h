// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "GameFramework/PlayerState.h"
#include "Resetable.h"
#include "ArenaPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStateChanged);

UENUM(Blueprintable)
enum class EPlayerStateInGame : uint8
{
	PreHandshake  = 0, // Player has just joined the server and didn't tell server its HID
	PostHandshake = 1, // Server accepted this Player's HID - they are now registered
	ReadyToPlay   = 2, // Player told server that they are ready to start the match
	Ingame        = 3, // Server has started the game, Player is in-game
	PostGame      = 4  // Server has ended the game, Players should take off the HMD and disconnect
};

UENUM(Blueprintable)
enum class EPlayerAliveState : uint8
{
	NotSpawnedYet = 0, // Player exists on server but not yet in-game (can spawn when State==ReadyToPlay)
	Alive         = 1, // Player is actively participating in game at this moment
	Ghost         = 2, // Player is dead but can still spawn into the game
	GameOver      = 3  // Player will not play the game anymore, is waiting for others
};

UCLASS(Blueprintable)
class QUESTARENA_API AArenaPlayerState : public APlayerState/*, public IResetable*/
{
	GENERATED_BODY()

public:
	//------------------------------------Actor lifecycle
	AArenaPlayerState(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	~AArenaPlayerState();

	//------------------------------------Properties

	UPROPERTY(BlueprintReadOnly, Category = "Player Info", ReplicatedUsing = OnRep_ArenaPlayerId)
	uint8 ArenaPlayerId;

	UPROPERTY(BlueprintReadOnly, Category = "Player Info", ReplicatedUsing = OnRep_ArenaPlayerColor)
	FLinearColor ArenaPlayerColor;

	UPROPERTY(BlueprintReadOnly, Category = "Player Info", ReplicatedUsing = OnRep_ArenaPlayerName)
	FString ArenaPlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Player State", ReplicatedUsing = OnRep_IngameState)
	EPlayerStateInGame IngameState;

	UPROPERTY(BlueprintReadOnly, Category = "Player State", ReplicatedUsing = OnRep_AliveState)
	EPlayerAliveState AliveState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Player State", Replicated)
	UHealthComponent* PlayerHealth;

	////------------------------------------Event dispatchers

	//Called on server directly after making changes to this variable.
	//Called on client after OnRep
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerIngameStateChanged;

	//Called on server directly after making changes to this variable.
	//Called on client after OnRep
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerAliveStateChanged;

	//------------------------------------Virtual OnReps

	UFUNCTION(BlueprintNativeEvent)
	void OnRep_ArenaPlayerId();
	virtual void OnRep_ArenaPlayerId_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnRep_ArenaPlayerColor();
	virtual void OnRep_ArenaPlayerColor_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnRep_ArenaPlayerName();
	virtual void OnRep_ArenaPlayerName_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnRep_IngameState();
	virtual void OnRep_IngameState_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void OnRep_AliveState();
	virtual void OnRep_AliveState_Implementation();

	//-----------------------------------Helpers
	UPROPERTY()
	class APlayerController* OptionalPC;

	UFUNCTION(BlueprintPure)
	bool IsLocalPlayerState();

	//-----------------------------------Stat management
	UFUNCTION(BlueprintNativeEvent)
	bool FetchStat(UPARAM(ref) FString& Stat, FString& Value);
	virtual bool FetchStat_Implementation(FString& Stat, FString& Value);

	////-----------------------------------IResetable interface
	//virtual bool ServerRestart_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	void RestartForNextSession();
	virtual void RestartForNextSession_Implementation();
};

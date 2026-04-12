// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ServerGameMode.generated.h"

class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnded, bool, bGameWon);

UCLASS()
class QUESTARENA_API AServerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	//-----------------------------Actor lifecycle
	AServerGameMode(const FObjectInitializer& OI);
	~AServerGameMode();

	virtual void BeginPlay() override;

	//-----------------------------Arena GameMode properties ("EditAnywheres")
	//-----------player count

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 MinNumPlayers;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 MaxNumPlayers;

	//-----------game stats screen

	//These fields are fetched from GameState and are equal for all players, displayed on player stats screen
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> GlobalStatFields;
	
	//These fields are fetched from all PlayerStates, displayed on high score table
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> PerUserStatFields;

	//These fields are fetched from requesting PlayerState, displayed on player stats screen
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> PrivateUserStatFields;

	//-----------------------------Arena player registration

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	bool RegisterPlayer(APlayerController* PC, uint8 ArenaPlayerId, uint8& UnrealPlayerId);

	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> UnregisteredPlayers;

	UPROPERTY(BlueprintReadOnly)
	TMap<uint8, APlayerController*> RegisteredPlayers;

	//-----------------------------GameState change

	//game can be started if GameState==GatheringPlayers && NumUnregisteredPlayers==0 && NumRegisteredPlayers in range(MinNumPlayers && MaxNumPlayers)
	UFUNCTION(BlueprintPure)
	bool CanStartGame();

	UFUNCTION(BlueprintCallable)
	bool StartGame();

	//game can be ended if GameState==InGame. @todo: we might want to add more conditions later
	UFUNCTION(BlueprintPure)
	bool CanEndGame(bool& bIsGameConsideredWon);

	UFUNCTION(BlueprintCallable)
	bool EndGame();

	UFUNCTION(BlueprintCallable)
	void RestartGame();

	UPROPERTY(BlueprintAssignable)
	FOnStateStarted OnGameStarted;

	UPROPERTY(BlueprintAssignable)
	FOnStateEnded OnGameEnded;

	//-----------------------------MVP - automatic game restart

	//failsafe flag in case two players press the restart button in the same time
	UPROPERTY()
	bool bRestartButtonEnabled = false;

	//Every time player state changes during GameState=Ingame, check if we can end the game
	UFUNCTION()
	void MVP_OnPlayerDied();

	UFUNCTION()
	void MVP_OnRestartRequested();
};

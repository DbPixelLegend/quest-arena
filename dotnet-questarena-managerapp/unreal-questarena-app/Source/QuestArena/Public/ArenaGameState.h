// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Resetable.h"
#include "StatFields.h"
#include "ArenaGameState.generated.h"

class UDataTable;

UENUM(Blueprintable)
enum class ECurrentGameState : uint8
{
	GatheringPlayers = 0,
	InGame = 1,
	GameOver = 2
};

UCLASS()
class QUESTARENA_API AArenaGameState : public AGameStateBase, public IResetable
{
	GENERATED_BODY()
	
private:
	
	//Current state of this Arena session
	//@note: no longer replicated, we set this value through multicast RPC
	//@note: no longer BlueprintReadOnly to hide it from being set directly in C++, get it through pure func instead
	UPROPERTY(/*BlueprintReadOnly, replicated*/)
	ECurrentGameState CurrentGameState;

	//-----------------------------------Stat management
	FDateTime GameStartedTime;

public:

	//table for converting stat fields into FTexts. Theoretically should be contained in GameMode
	//with the rest of stat field support, but we need clients to access this and GM is server only.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDataTable* StatFieldTranslations;

	//-----------------------------------Actor lifecycle

	AArenaGameState(const FObjectInitializer& OI);
	~AArenaGameState();
	virtual void BeginPlay() override;
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Current game state is set by this function from GameMode which
	// registers players and thus must ensure that everyone is ready to go ingame.
	UFUNCTION(NetMulticast, reliable)
	void Multicast_SetGameState(ECurrentGameState NewState);
	void Multicast_SetGameState_Implementation(ECurrentGameState NewState);

	// Blueprint implementations of this gamestate should react to
	// the change and start the actual game if InGame (ie. spawn all players,
	// reset object locations) or cleanup if GameOver.
	UFUNCTION(BlueprintNativeEvent)
	void OnGameStateChanged();
	virtual void OnGameStateChanged_Implementation();
	
	UFUNCTION(BlueprintPure)
	inline ECurrentGameState GetCurrentGameState() { return CurrentGameState; }

	// This overridable method determines whether the game can be ended.
	// It's called by ServerGameMode when checking for CanEndGame().
	// The default implementation returns:
	//  GameWon = always false.
	//  CanEndGame = true if there are no more living players left.
	//               NOTE: feel free to not use the parent implementation
	//                     if you don't need the alive-players check.
	UFUNCTION(BlueprintNativeEvent)
	void IsGameEndConditionSatisfied(bool& CanEndGame, bool& GameWon);
	void IsGameEndConditionSatisfied_Implementation(bool& CanEndGame, bool& GameWon);

	//-----------------------------------Stat management
	UFUNCTION(BlueprintNativeEvent)
	bool FetchStat(UPARAM(ref) FString& Stat, FString& Value);
	virtual bool FetchStat_Implementation(FString& Stat, FString& Value);

	//-----------------------------------IResetable interface
	virtual bool ServerRestart_Implementation() override;
};

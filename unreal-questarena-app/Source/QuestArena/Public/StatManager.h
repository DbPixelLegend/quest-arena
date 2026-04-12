// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatManager.generated.h"

USTRUCT(BlueprintType)
struct FStatRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FString>  RowValues;
};

USTRUCT(BlueprintType)
struct FGameStats
{
	GENERATED_BODY()

	//GLOBALS - game stats, equal for all players, visible in stats screen
	UPROPERTY(BlueprintReadOnly)
	TArray<FString>  GameStatNames;
	
	UPROPERTY(BlueprintReadOnly)
	FStatRow         GameStatValues;

	//PRIVATES - per player stats, visible only for current player in stats screen
	UPROPERTY(BlueprintReadOnly)
	TArray<FString>  PrivateStatNames;

	UPROPERTY(BlueprintReadOnly)
	FStatRow         PrivateStatValues;

	//STATS - per player stats, visible for all players in score table screen
	UPROPERTY(BlueprintReadOnly)
	TArray<FString>  PlayerStatNames;

	UPROPERTY(BlueprintReadOnly)
	TArray<FStatRow> PlayerStatValues;
};

class AServerGameMode;
class AArenaPlayerController;

UCLASS(BlueprintType)
class QUESTARENA_API UStatManager : public UObject
{
	GENERATED_BODY()
	
public:
	//Collects all stats.
	//NOTE: This method is server only! Requires GameMode
	//NOTE: when called from C++, ensure that Stats is an empty object

	UFUNCTION(BlueprintCallable)
	static void FetchGameStatsForDisplay(AServerGameMode* GameMode, AArenaPlayerController* Controller, FGameStats& Stats);

	//@todo: implement this if it becomes necessary
	//static void GameStatsToJson();
};

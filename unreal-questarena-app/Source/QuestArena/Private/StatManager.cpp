// Fill out your copyright notice in the Description page of Project Settings.

#include "StatManager.h"
#include "ServerGameMode.h"
#include "ArenaGameState.h"
#include "ArenaPlayerState.h"
#include "ArenaPlayerController.h"

void UStatManager::FetchGameStatsForDisplay(AServerGameMode* GameMode, AArenaPlayerController* Controller, FGameStats& Stats)
{
	AArenaGameState* AGS;
	AArenaPlayerState* PrivateAPS = nullptr;
	TArray<AArenaPlayerState*> APSs;
	if (!GameMode->IsValidLowLevel())
	{
		ensureAlwaysMsgf(false, TEXT("UStartManager::FetchGameStats(): No GameMode provided!"));
		return;
	}

	AGS = Cast<AArenaGameState>(GameMode->GameState);
	check(AGS->IsValidLowLevel());

	for (auto& Pair : GameMode->RegisteredPlayers)
	{
		AArenaPlayerState* APS = Cast<AArenaPlayerState>(Pair.Value->PlayerState);
		check(APS->IsValidLowLevel());
		APSs.Add(APS);
		if (Pair.Value == Controller) PrivateAPS = APS;
	}

	Stats.GameStatNames = GameMode->GlobalStatFields;
	Stats.PlayerStatNames = GameMode->PerUserStatFields;
	Stats.PrivateStatNames = GameMode->PrivateUserStatFields;

	FString Value;
	bool bFoundValue;

	const int32 GameStatNum = Stats.GameStatNames.Num();
	for (FString& Field : Stats.GameStatNames)
	{
		bFoundValue = AGS->FetchStat(Field, Value);
		ensureAlwaysMsgf(bFoundValue, TEXT("Field not found in GameState: %s. Using empty string"), *Field);
		Stats.GameStatValues.RowValues.Add(bFoundValue ? Value : "");
	}

	const int32 PrivateStatNum = Stats.PrivateStatNames.Num();
	for (FString& Field : Stats.PrivateStatNames)
	{
		bFoundValue = PrivateAPS->FetchStat(Field, Value);
		ensureAlwaysMsgf(bFoundValue, TEXT("Field not found in PlayerState: %s. Using empty string"), *Field);
		Stats.PrivateStatValues.RowValues.Add(bFoundValue ? Value : "");
	}

	const int32 PlayerNum = APSs.Num();
	const int32 PlayerStatNum = Stats.PlayerStatNames.Num();
	for (AArenaPlayerState* APS : APSs)
	{
		FStatRow& CurrentRow = Stats.PlayerStatValues[Stats.PlayerStatValues.Add(FStatRow())];

		for (FString& Field : Stats.PlayerStatNames)
		{
			bFoundValue = APS->FetchStat(Field, Value);
			ensureAlwaysMsgf(bFoundValue, TEXT("Field not found in PlayerState: %s. Using empty string"), *Field);
			CurrentRow.RowValues.Add(bFoundValue ? Value : "");
		}
	}
}

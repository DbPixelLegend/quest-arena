// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

/* ---------------------------------------------------------------------------------------------------- */

void UArenaFunctionLibrary::GetCurrentNetworkType(const UObject* WorldContextObject, ENetworkUser& Network)
{
	if (!WorldContextObject)
	{
		Network = ENetworkUser::NU_Unknown;
		return;
	}
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		if (World->GetNetMode() == NM_Standalone)
		{
			Network = ENetworkUser::NU_OfflineSession;
			return;
		}
		Network = (World->GetNetMode() == NM_DedicatedServer) ? ENetworkUser::NU_DedicatedServer : ENetworkUser::NU_OnlineSession;
		return;
	}
}

/* ---------------------------------------------------------------------------------------------------- */

float UArenaFunctionLibrary::GetRandomFloatInRange(FVector2D Range)
{
	return UKismetMathLibrary::RandomFloatInRange(Range.X, Range.Y);
}

int32 UArenaFunctionLibrary::GetRandomIntegerInRange(FVector2D Range)
{
	return UKismetMathLibrary::RandomIntegerInRange((int32)Range.X, (int32)Range.Y);
}

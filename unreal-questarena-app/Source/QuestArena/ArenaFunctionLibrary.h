// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArenaFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class ENetworkUser : uint8
{
	NU_Unknown UMETA(DisplayName = "Unknown"),
	NU_OfflineSession UMETA(DisplayName = "Offline"),
	NU_OnlineSession UMETA(DisplayName = "Online"),
	NU_DedicatedServer UMETA(DisplayName = "Server")
};

/**
 * 
 */
UCLASS()
class QUESTARENA_API UArenaFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Checks if currently running app is connected to online service. If yes determines if it's dedicated server or online-aware client.
	UFUNCTION(BlueprintCallable, Category = "Networking", meta = (WorldContext = "WorldContextObject", ExpandEnumAsExecs = "Network"))
	static void GetCurrentNetworkType(const UObject* WorldContextObject, ENetworkUser& Network);

	UFUNCTION(BlueprintPure, Category = "Range Vectors")
	static float GetRandomFloatInRange(FVector2D Range);

	UFUNCTION(BlueprintPure, Category = "Range Vectors")
	static int32 GetRandomIntegerInRange(FVector2D Range);
};

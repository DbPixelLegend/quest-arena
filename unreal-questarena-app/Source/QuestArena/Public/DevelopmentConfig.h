// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeveloperSettings.h"
#include "UObject/NoExportTypes.h"
#include "DevelopmentConfig.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Development Settings"))
class QUESTARENA_API UDevelopmentConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UDevelopmentConfig(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, config, Category = VR)
	bool bStartInVREnabled = true;

	//during development, AServerGameMode will automatically go ingame
	//when this amount of players had joined and registered.
	UPROPERTY(EditAnywhere, config, Category = Multiplayer)
	int32 DebugTargetNumPlayers = 1;

	//if this flag is checked during development, AServerGameMode
	//will accept players even if session has already gone ingame.
	UPROPERTY(EditAnywhere, config, Category = Multiplayer)
	bool bDebugAllowLateJoiners = false;

	UFUNCTION(BlueprintCallable)
	static bool IsStartInVREnabled();

	UFUNCTION(BlueprintCallable)
	static int32 GetDebugTargetNumPlayers();

	UFUNCTION(BlueprintCallable)
	static bool GetDebugAllowLateJoiners();
};

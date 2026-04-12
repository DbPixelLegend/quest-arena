// Fill out your copyright notice in the Description page of Project Settings.

#include "DevelopmentConfig.h"
#include "CoreGlobals.h"

UDevelopmentConfig::UDevelopmentConfig(const FObjectInitializer& ObjectInitializer)
{

}

bool UDevelopmentConfig::IsStartInVREnabled()
{
	// We return true by default, because in most cases we want VR to run as soon as possible. Target device is Oculus Quest.
	UDevelopmentConfig* Settings = GetMutableDefault<UDevelopmentConfig>();
	return Settings ? Settings->bStartInVREnabled : true;
}

int32 UDevelopmentConfig::GetDebugTargetNumPlayers()
{
	// Return -1 by default, because then the comparison in GameMode (is NumRegisteredPlayers = -1 ?) will
	// always fail as it should in production environment (game should start automatically, but target num
	// players will differ on session-by-session basis and should be received from somewhere else in the code)

	#if UE_BUILD_SHIPPING
	return -1;
	
	#else
	UDevelopmentConfig* Settings = GetMutableDefault<UDevelopmentConfig>();
	return Settings ? Settings->DebugTargetNumPlayers : -1;
	
	#endif
}

bool UDevelopmentConfig::GetDebugAllowLateJoiners()
{
	// Return false by default, because all players must be present
	// to go ingame in production environment.

#if UE_BUILD_SHIPPING
	return false;

#else
	UDevelopmentConfig* Settings = GetMutableDefault<UDevelopmentConfig>();
	return Settings ? Settings->bDebugAllowLateJoiners : false;

#endif
}

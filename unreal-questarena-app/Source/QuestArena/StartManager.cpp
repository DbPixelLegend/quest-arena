// Fill out your copyright notice in the Description page of Project Settings.

#include "StartManager.h"
#include "QuestArena.h"
#include "PlayerStartPosition.h"
#include "Kismet/GameplayStatics.h"

AStartManager* AStartManager::StartManagerPtr = nullptr;

// Sets default values
AStartManager::AStartManager()
{
#if WITH_EDITOR
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
#else
	PrimaryActorTick.bCanEverTick = false;
#endif
}

void AStartManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
#if WITH_EDITOR
	for (int i = 0; i < PlayerStartPositions.Num(); i++)
	{
		if (PlayerStartPositions[i])
		{
			PlayerStartPositions[i]->SetIndex(i);
		}
	}
#endif
}

void AStartManager::BeginPlay()
{
	Super::BeginPlay();
	AStartManager::StartManagerPtr = this;
}

const TArray<APlayerStartPosition*> AStartManager::GetAvailableStartPositions()
{
	/*
	Before QuestArena MVP was specified there was an idea to make StartManager more complicated than it should be now.
	In the final "revision" of this manager, the occupation of the start position and other "spawn" related things will be checked here.
	For now whole idea of StartManager was dropped, but we will keep this thing, because it works and will help level designers in the future.
*/
// 	TArray<APlayerStartPosition*> OutPositions;
// 	for (auto pos : PlayerStartPositions)
// 	{
// 		if (pos->IsOccupied() == false)
// 		{
// 			OutPositions.Add(pos);
// 		}
// 	}
// 	return OutPositions;
	return PlayerStartPositions;
}

AStartManager* AStartManager::GetSingleton(UObject* WorldRef)
{
	if (WorldRef != nullptr && AStartManager::StartManagerPtr == nullptr)
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(WorldRef, AStartManager::StaticClass(), foundActors);
		if (foundActors.Num() > 0)
		{
			return Cast<AStartManager>(foundActors[0]);
		}
		else if (foundActors.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("No instance of ALoginManager actor was found on current level. Please add one if you want to use its' functionality."))
		}
	}
	return AStartManager::StartManagerPtr;
}

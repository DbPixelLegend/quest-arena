// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartManager.generated.h"

class APlayerStartPosition;

UCLASS(BlueprintType)
class QUESTARENA_API AStartManager : public AActor
{
	GENERATED_BODY()
	
public:	

	static AStartManager* GetStaticPointer() { return AStartManager::StartManagerPtr; }

	// Gets static pointer to ALoginManager class. If it points to null spawns new actor (in most cases it will spawn one actor per world).
	UFUNCTION(BlueprintCallable, Category = "Start manager", meta = (WorldContext = "WorldRef", DisplayName = "GetStartManager"))
	static AStartManager* GetSingleton(UObject* WorldRef);

	static AStartManager* StartManagerPtr;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<APlayerStartPosition*> GetAvailableStartPositions();

protected:

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
	TArray<APlayerStartPosition*> PlayerStartPositions;

	// Sets default values for this actor's properties
	AStartManager();

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const { return true; }
};

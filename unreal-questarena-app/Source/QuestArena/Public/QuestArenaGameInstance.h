// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "QuestArenaGameInstance.generated.h"

class AActor;
class UWorld;

UCLASS()
class QUESTARENA_API UQuestArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

private:
	void HandlePostWorldInitialization(UWorld* World, const UWorld::InitializationValues InitializationValues);
	void RepairTutorialHandsForWorld(TWeakObjectPtr<UWorld> WorldPtr);
	bool EnsureWidgetInteractionReference(AActor* Actor) const;

	FDelegateHandle PostWorldInitDelegateHandle;
	TMap<TWeakObjectPtr<UWorld>, FTimerHandle> RepairTimerHandles;
};

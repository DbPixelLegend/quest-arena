// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "IPickable.generated.h"


UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class QUESTARENA_API UPickable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class QUESTARENA_API IPickable
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void Pickup(AActor* PickedUpBy, EControllerHand HandType = EControllerHand::AnyHand);

	UFUNCTION(BlueprintCallable)
	virtual void Drop(AActor* DroppedBy, EControllerHand Hand);

	virtual void PickupImplementation(AActor* PickedUpBy, EControllerHand Hand) = 0;
	virtual void DropImplementation(AActor* DropBy, EControllerHand Hand) = 0;

	UFUNCTION(BlueprintCallable)
	virtual FString GetPickupSocket();
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "IPickable.h"

/* ---------------------------------------------------------------------------------------------------- */

UPickable::UPickable(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

/* ---------------------------------------------------------------------------------------------------- */

void IPickable::Pickup(AActor* PickedUpBy, EControllerHand Hand)
{
	PickupImplementation(PickedUpBy, Hand);
}

/* ---------------------------------------------------------------------------------------------------- */

void IPickable::Drop(AActor* DroppedBy, EControllerHand Hand)
{
	DropImplementation(DroppedBy, Hand);
}

FString IPickable::GetPickupSocket()
{
	return TEXT("");
}

/* ---------------------------------------------------------------------------------------------------- */
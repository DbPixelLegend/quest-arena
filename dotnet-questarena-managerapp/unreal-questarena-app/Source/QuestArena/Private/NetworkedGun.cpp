// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkedGun.h"
#include "UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "VRPawn.h"
#include "MotionControllerBase.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

ANetworkedGun::ANetworkedGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ANetworkedGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION(ANetworkedGun, R_Owner, COND_None);
	DOREPLIFETIME_CONDITION(ANetworkedGun, R_HandAttachedTo, COND_None);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ANetworkedGun::PickupImplementation(AActor* PickedBy, EControllerHand Hand)
{
	SetOwner(PickedBy);
	R_Owner = PickedBy;
	R_HandAttachedTo = Hand;
	UpdateGunAttachment();
}

void ANetworkedGun::DropImplementation(AActor* DropBy, EControllerHand Hand)
{
	auto pawn = Cast<AVRPawn>(R_Owner);
	pawn->GetHandController(R_HandAttachedTo)->DropGunFromHand(this);
	SetOwner(nullptr);
	R_Owner = nullptr;

	// No hand
	R_HandAttachedTo = EControllerHand::Special_9;
}

void ANetworkedGun::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANetworkedGun::BeginPlay()
{
	Super::BeginPlay();
}

FString ANetworkedGun::GetPickupSocket()
{
	return TEXT("");
}

void ANetworkedGun::MULTICAST_Restart_Implementation(FTransform NewTransform)
{
	GetPhysicsObject()->SetWorldTransform(NewTransform);
}

bool ANetworkedGun::ServerRestart_Implementation()
{
	return true;
}

void ANetworkedGun::OnRep_OnNetworkedOwnerChanged()
{
	UpdateGunAttachment();
}

void ANetworkedGun::OnRep_OnNetworkedHandChanged()
{
	UpdateGunAttachment();
}

void ANetworkedGun::UpdateGunAttachment()
{
	if (R_Owner && R_HandAttachedTo == EControllerHand::Left || R_HandAttachedTo == EControllerHand::Right)
	{
		auto pawn = Cast<AVRPawn>(R_Owner);
		pawn->GetHandController(R_HandAttachedTo)->AttachGunToHand(this);
	}
}

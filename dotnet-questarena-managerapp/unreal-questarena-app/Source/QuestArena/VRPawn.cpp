// Pixel Legend 2018, All Rights Reserved

#include "VRPawn.h"
#include "QuestArena.h"
#include "NetworkedPawn.h"
#include "MotionControllerBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "IPickable.h"
#include "NetworkedGun.h"
#include "Kismet/KismetSystemLibrary.h"

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION(AVRPawn, R_LeftMCNetworkedLocation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AVRPawn, R_LeftMCNetworkedRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AVRPawn, R_RightMCNetworkedLocation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AVRPawn, R_RightMCNetworkedRotation, COND_SkipOwner);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

/* ---------------------------------------------------------------------------------------------------- */

AMotionControllerBase* AVRPawn::StaticGetHandController(const UObject* WorldRef, EControllerHand HandEnum)
{
	if (WorldRef)
	{
		auto pawn = Cast<AVRPawn>(WorldRef->GetWorld()->GetFirstPlayerController()->GetPawn());
		return pawn ? pawn->GetHandController(HandEnum) : nullptr;

	}
	return nullptr;
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::ShootGun(AActor* GunToShoot, EControllerHand Hand)
{
	if (GunToShoot && IsLocallyControlled())
	{
		SERVER_ShootGun(GunToShoot, Hand);
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::SERVER_ShootGun_Implementation(AActor* GunToShoot, EControllerHand Hand)
{
	if (GunToShoot)
	{
		auto networkedGun = Cast<ANetworkedGun>(GunToShoot);
		if (networkedGun)
		{
			networkedGun->SERVER_Fire();
			Serveronly_OnGunFired.Broadcast();
		}
		MULTICAST_ShootGun(networkedGun, Hand);
	}
}

/* ---------------------------------------------------------------------------------------------------- */

bool AVRPawn::SERVER_ShootGun_Validate(AActor* GunToShoot, EControllerHand Hand)
{
	return true;
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::MULTICAST_ShootGun_Implementation(AActor* GunToShoot, EControllerHand Hand)
{
	if (GunToShoot)
	{
		ShootGunEvent(Hand);
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::DropGun(AActor* GunToDrop, EControllerHand Hand)
{
	if (GunToDrop)
	{
		SERVER_DropGun(GunToDrop, this, Hand);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GunToGrab pointer is null."));
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::SERVER_DropGun_Implementation(AActor* GunToDrop, AVRPawn* GrabbingPawn, EControllerHand Hand)
{
//#if UE_SERVER
	if (GunToDrop)
	{
		IPickable* pickupInterface = Cast<IPickable>(GunToDrop);
		if (pickupInterface)
		{
			pickupInterface->Drop(this, Hand);
			GunToDrop->SetActorLocation((Hand == EControllerHand::Left) ? R_LeftMCNetworkedLocation : R_RightMCNetworkedLocation);
			GunToDrop->SetActorRotation((Hand == EControllerHand::Left) ? R_LeftMCNetworkedRotation : R_RightMCNetworkedRotation);
		}
		MULTICAST_DropGun(GunToDrop, GrabbingPawn, Hand);
	}
//#endif
}

/* ---------------------------------------------------------------------------------------------------- */

bool AVRPawn::SERVER_DropGun_Validate(AActor* GunToDrop, AVRPawn* GrabbingPawn, EControllerHand Hand)
{
	return true;
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::MULTICAST_DropGun_Implementation(AActor* GunToDrop, AVRPawn* GrabbingPawn, EControllerHand Hand)
{
	if (GunToDrop && !UKismetSystemLibrary::IsDedicatedServer(this))
	{
		OnActorDropDispatcher.Broadcast(GunToDrop, Hand);
		if(GrabbingPawn && GrabbingPawn->GetHandController(Hand) && GunToDrop->GetAttachParentActor() != nullptr && !GrabbingPawn->IsLocallyControlled())
		{
			GrabbingPawn->GetHandController(Hand)->DropGunFromHand(GunToDrop);
		}
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::GrabGun(AActor* GunToGrab, EControllerHand Hand)
{
	if (GunToGrab && GunToGrab->GetOwner() == nullptr)
	{
		SERVER_GrabGun(GunToGrab, Hand);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GunToGrab pointer is null."));
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::SERVER_GrabGun_Implementation(AActor* GunToGrab, EControllerHand Hand)
{
	if (GunToGrab)
	{
		IPickable* pickupInterface = Cast<IPickable>(GunToGrab);
		if (pickupInterface)
		{
			pickupInterface->Pickup(this, Hand);
		}
		MULTICAST_GrabGun(GunToGrab, this, Hand);
	}
}

/* ---------------------------------------------------------------------------------------------------- */

bool AVRPawn::SERVER_GrabGun_Validate(AActor* GunToGrab, EControllerHand Hand)
{
	return true;
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::MULTICAST_GrabGun_Implementation(AActor* GunToGrab, AVRPawn* GrabbingPawn, EControllerHand Hand)
{
	if (GunToGrab && !UKismetSystemLibrary::IsDedicatedServer(this))
	{
		OnActorPickupDispatcher.Broadcast(GunToGrab, Hand);
		if (GrabbingPawn && GrabbingPawn->GetHandController(Hand) != nullptr && GunToGrab->GetAttachParentActor() == nullptr && !GrabbingPawn->IsLocallyControlled())
		{
			GrabbingPawn->GetHandController(Hand)->AttachGunToHand(GunToGrab);
		}
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::BitSerialize(TArray<uint8>& Array, FMemoryWriter& Writer)
{
	Super::BitSerialize(Array, Writer);

	AMotionControllerBase* leftController = GetHandController(EControllerHand::Left);
	AMotionControllerBase* rightController = GetHandController(EControllerHand::Right);

	if (leftController && rightController)
	{
		uint8 bitmask = Array[0];

		FVector leftControllerLoc = leftController->GetMotionControllerLocation();
		FRotator leftControllerRot = leftController->GetMotionControllerRotation();

		FVector rightControllerLoc = rightController->GetMotionControllerLocation();
		FRotator rightControllerRot = rightController->GetMotionControllerRotation();

		// Check if should send LeftMotionController location
		if (!leftControllerLoc.Equals(R_LeftMCNetworkedLocation, ReplicationLocationThreshold))
		{
			bitmask |= 0x08;
			R_LeftMCNetworkedLocation = leftControllerLoc;
			Writer << R_LeftMCNetworkedLocation;
		}

		// Check if should send LeftMotionController rotation
		if (!leftControllerRot.Equals(R_LeftMCNetworkedRotation, ReplicationRotationThreshold))
		{
			bitmask |= 0x10;
			R_LeftMCNetworkedRotation = leftControllerRot;
			Writer << R_LeftMCNetworkedRotation;
		}

		// Check if should sent RightMotionController location
		if (!rightControllerLoc.Equals(R_RightMCNetworkedLocation, ReplicationLocationThreshold))
		{
			bitmask |= 0x20;
			R_RightMCNetworkedLocation = rightControllerLoc;
			Writer << R_RightMCNetworkedLocation;
		}

		// Check if should sent RightMotionController location
		if (!rightControllerRot.Equals(R_RightMCNetworkedRotation, ReplicationRotationThreshold))
		{
			bitmask |= 0x40;
			R_RightMCNetworkedRotation = rightControllerRot;
			Writer << R_RightMCNetworkedRotation;
		}
		Array[0] = bitmask;
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::BitDeserialize(const TArray<uint8>& Array, FMemoryReader& Reader)
{
	Super::BitDeserialize(Array, Reader);

	uint8 bitmask = Array[0];
	// Check if bit array contains new left controller location
	if (bitmask & 0x08)
	{
		Reader << R_LeftMCNetworkedLocation;
	}

	// Check if bit array contains new left controller rotation
	if (bitmask & 0x10)
	{
		Reader << R_LeftMCNetworkedRotation;
	}

	// Check if bit array contains new right controller location
	if (bitmask & 0x20)
	{
		Reader << R_RightMCNetworkedLocation;
	}

	// Check if bit array contains new right controller rotation
	if (bitmask & 0x40)
	{
		Reader << R_RightMCNetworkedRotation;
	}

#if UE_SERVER
	// We need to set position of controles on the server
	AMotionControllerBase* leftController = GetHandController(EControllerHand::Left);
	AMotionControllerBase* rightController = GetHandController(EControllerHand::Right);

	rightController->SetMotionControllerRotation(R_RightMCNetworkedRotation);
	rightController->SetMotionControllerLocation(R_RightMCNetworkedLocation);

	leftController->SetMotionControllerRotation(R_LeftMCNetworkedRotation);
	leftController->SetMotionControllerLocation(R_LeftMCNetworkedLocation);
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void AVRPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if !UE_SERVER
	if (!IsControlled())
	{
		float actualInterpolationSpeed = FMath::Clamp(DeltaSeconds * ReplicationInterpolationSpeed, 0.0f, 1.0f);
		AMotionControllerBase* leftController = GetHandController(EControllerHand::Left);
		AMotionControllerBase* rightController = GetHandController(EControllerHand::Right);

		if (leftController && rightController)
		{
			// Lerp left MC location
			const FVector& leftControllerLoc = leftController->GetMotionControllerLocation();
			if (leftControllerLoc != R_LeftMCNetworkedLocation)
			{
				leftController->SetMotionControllerLocation(UKismetMathLibrary::VEase(leftControllerLoc, R_LeftMCNetworkedLocation, actualInterpolationSpeed, EEasingFunc::Linear));
			}
			// Lerp left MC rotation
			const FRotator& leftControllerRot = leftController->GetMotionControllerRotation();
			if (leftControllerRot != R_LeftMCNetworkedRotation)
			{
				leftController->SetMotionControllerRotation(UKismetMathLibrary::REase(leftControllerRot, R_LeftMCNetworkedRotation, actualInterpolationSpeed, true, EEasingFunc::Linear));
			}

			// Lerp right MC location
			const FVector& rightControllerLoc = rightController->GetMotionControllerLocation();
			if (rightControllerLoc != R_RightMCNetworkedLocation)
			{
				rightController->SetMotionControllerLocation(UKismetMathLibrary::VEase(rightControllerLoc, R_RightMCNetworkedLocation, actualInterpolationSpeed, EEasingFunc::Linear));
			}
			// Lerp right MC rotation
			const FRotator& rightControllerRot = rightController->GetMotionControllerRotation();
			if (rightControllerRot != R_RightMCNetworkedRotation)
			{
				rightController->SetMotionControllerRotation(UKismetMathLibrary::REase(rightControllerRot, R_RightMCNetworkedRotation, actualInterpolationSpeed, true, EEasingFunc::Linear));
			}
		}
	}
#endif
}

/* ---------------------------------------------------------------------------------------------------- */
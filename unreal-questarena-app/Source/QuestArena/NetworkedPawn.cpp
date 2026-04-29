// Pixel Legend 2018, All Rights Reserved

#include "NetworkedPawn.h"
#include "QuestArena.h"
#include "TimerManager.h"
#include "StaticBitArray.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "NetworkSerializable.h"

/* ---------------------------------------------------------------------------------------------------- */

// Sets default values
ANetworkedPawn::ANetworkedPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION(ANetworkedPawn, R_ActorNetworkedLocation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANetworkedPawn, R_ActorNetworkedRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANetworkedPawn, R_CameraNetworkedRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANetworkedPawn, R_CameraNetworkedLocation, COND_SkipOwner);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::BeginPlay()
{
	Super::BeginPlay();
#if !UE_SERVER
	if (ReplicationTimeInterval > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ReplicationTimerHandle, this, &ANetworkedPawn::Replicate, ReplicationTimeInterval, true, ReplicationTimeInterval);
	}
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::Destroyed()
{
#if !UE_SERVER
	GetWorld()->GetTimerManager().ClearTimer(ReplicationTimerHandle);
#endif
	Super::Destroyed();
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// We need nice interpolation only on client side
#if !UE_SERVER
	if (!IsPawnControlled())
	{
		DeltaSeconds = FMath::Clamp(DeltaSeconds, 0.0f, 1.0f);

		// Lerp actor location
		const FVector& actorLocation = GetActorLocation();
		if (actorLocation != R_ActorNetworkedLocation)
		{
			SetActorLocation(UKismetMathLibrary::VEase(actorLocation, R_ActorNetworkedLocation, DeltaSeconds * ReplicationInterpolationSpeed, EEasingFunc::Linear));
		}

		// Lerp actor rotation
		const FRotator& actorRotation = GetActorRotation();
		if (actorRotation != R_ActorNetworkedRotation)
		{
			SetActorRotation(UKismetMathLibrary::REase(actorRotation, R_ActorNetworkedRotation, DeltaSeconds * ReplicationInterpolationSpeed, true, EEasingFunc::Linear));
		}

		// Lerp actor rotation
		if (CameraRef)
		{
			const FRotator& cameraRotation = CameraRef->GetComponentRotation();
			if (cameraRotation != R_CameraNetworkedRotation)
			{
				CameraRef->SetWorldRotation(UKismetMathLibrary::REase(cameraRotation, R_CameraNetworkedRotation, DeltaSeconds * ReplicationInterpolationSpeed, true, EEasingFunc::Linear));
			}
		}

		// Lerp actor location
		if (CameraRef)
		{
			const FVector& cameraLocation = CameraRef->GetComponentLocation();
			if (cameraLocation != R_CameraNetworkedLocation)
			{
				CameraRef->SetWorldLocation(UKismetMathLibrary::VEase(cameraLocation, R_CameraNetworkedLocation, DeltaSeconds * ReplicationInterpolationSpeed, EEasingFunc::Linear));
			}
		}
	}
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::Replicate()
{
	// Check if this pawn is controlled locally (Client).
	if (GetController() && GetWorld()->GetFirstPlayerController() == GetController())
	{
		Server_Replicate(NetworkSerialize());
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::Server_Replicate_Implementation(const TArray<uint8>& Data)
{
#if UE_SERVER
	NetworkDeserialize(Data);
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

bool ANetworkedPawn::Server_Replicate_Validate(const TArray<uint8>& Data)
{
	return true;
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::SetReplicationTimeInterval(float NewInterval)
{
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	GetWorld()->GetTimerManager().ClearTimer(ReplicationTimerHandle);
	ReplicationTimeInterval = NewInterval;
	if (ReplicationTimeInterval > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ReplicationTimerHandle, this, &ANetworkedPawn::Replicate, ReplicationTimeInterval, true, ReplicationTimeInterval);
	}
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::SetReplicationInterpolationSpeed(float NewSpeed)
{
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	ReplicationInterpolationSpeed = NewSpeed;
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::BitSerialize(TArray<uint8>& Array, FMemoryWriter& Writer)
{
	/*
	String representing which values will be sent to the server over RPC (bit positions):
	0 - Actor Location
	1 - Actor Rotation
	2 - Actor VisibleMesh/Camera location
*/
	FVector actorLocation = GetActorLocation();
	FRotator actorRotation = GetActorRotation();
	FRotator actorCameraRotation = CameraRef ? CameraRef->GetComponentRotation() : FRotator::ZeroRotator;
	FVector actorCameraLocation = CameraRef ? CameraRef->GetComponentLocation() : FVector::ZeroVector;
	uint8 bitmask = 0;

	Writer << bitmask;

	// Check if actor location has changed since last RPC to server
	if (!actorLocation.Equals(R_ActorNetworkedLocation, ReplicationLocationThreshold))
	{
		R_ActorNetworkedLocation = actorLocation;
		bitmask |= 0x01;
		Writer << R_ActorNetworkedLocation;
	}

	// Check if actor rotation has changed since last RPC to server
	if (!actorRotation.Equals(R_ActorNetworkedRotation, ReplicationRotationThreshold))
	{
		R_ActorNetworkedRotation = actorRotation;
		bitmask |= 0x02;
		Writer << R_ActorNetworkedRotation;
	}

	// Check if actor's camera rotation has changed since last RPC to server
	if (!actorCameraRotation.Equals(R_CameraNetworkedRotation, ReplicationRotationThreshold))
	{
		R_CameraNetworkedRotation = actorCameraRotation;
		bitmask |= 0x04;
		Writer << R_CameraNetworkedRotation;
	}

	// Check if actor's camera location has changed since last RPC to server
	if (!actorCameraLocation.Equals(R_CameraNetworkedLocation, ReplicationRotationThreshold))
	{
		R_CameraNetworkedLocation = actorCameraLocation;
		bitmask |= 0x80;
		Writer << R_CameraNetworkedLocation;
	}
	Array[0] = bitmask;
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedPawn::BitDeserialize(const TArray<uint8>& Array, FMemoryReader& Reader)
{
	/*
		Getting string representing which values will be sent to the server over RPC (bit positions):
		0 - Actor Location
		1 - Actor Rotation
		2 - Actor VisibleMesh/Camera location

		IMPORTANT NOTICE:
		Bits from 0-2 are already set int Super::NetworkSerialize_Implementation()
	*/
	uint8 bitMask;
	Reader << bitMask;

	// Check if bit array contains new Actor location
	if (bitMask & 0x01)
	{
		Reader << R_ActorNetworkedLocation;
#if UE_SERVER
		SetActorLocation(R_ActorNetworkedLocation);
#endif
	}

	// Check if bit array contains new Actor rotation
	if (bitMask & 0x02)
	{
		Reader << R_ActorNetworkedRotation;
#if UE_SERVER
		SetActorRotation(R_ActorNetworkedRotation);
#endif
	}

	// Check if bit array contains new Actor's visible mesh rotation
	if (bitMask & 0x04)
	{
		Reader << R_CameraNetworkedRotation;
#if UE_SERVER
		if (CameraRef)
		{
			CameraRef->SetWorldRotation(R_CameraNetworkedRotation);
		}
#endif
	}

	// Check if bit array contains new Actor's visible mesh rotation
	if (bitMask & 0x80)
	{
		Reader << R_CameraNetworkedLocation;
#if UE_SERVER
		if (CameraRef)
		{
			CameraRef->SetWorldLocation(R_CameraNetworkedLocation);
		}
#endif
	}
}

/* ---------------------------------------------------------------------------------------------------- */
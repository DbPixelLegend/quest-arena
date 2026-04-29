// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkedActor.h"
#include "TimerManager.h"
#include "StaticBitArray.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "GameFramework/Actor.h"

/* ---------------------------------------------------------------------------------------------------- */

// Sets default values
ANetworkedActor::ANetworkedActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION(ANetworkedActor, R_ActorNetworkedLocation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANetworkedActor, R_ActorNetworkedRotation, COND_SkipOwner);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

/* ---------------------------------------------------------------------------------------------------- */


// Called when the game starts or when spawned
void ANetworkedActor::BeginPlay()
{
	Super::BeginPlay();
	R_ActorNetworkedLocation = GetActorLocation();
	R_ActorNetworkedRotation = GetActorRotation();
#if !UE_SERVER
	if (ReplicationTimeInterval > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ReplicationTimerHandle, this, &ANetworkedActor::Replicate, ReplicationTimeInterval, true, ReplicationTimeInterval);
	}
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::Destroyed()
{
#if !UE_SERVER
	GetWorld()->GetTimerManager().ClearTimer(ReplicationTimerHandle);
#endif
	Super::Destroyed();
}

/* ---------------------------------------------------------------------------------------------------- */

// Called every frame
void ANetworkedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if !UE_SERVER
	if (!IsOwnedBy(GetWorld()->GetFirstPlayerController()->AcknowledgedPawn))
	{
		DeltaTime = FMath::Clamp(DeltaTime, 0.0f, 1.0f);

		// Lerp actor location
		const FVector& actorLocation = GetActorLocation();
		if (actorLocation != R_ActorNetworkedLocation)
		{
			SetActorLocation(UKismetMathLibrary::VEase(actorLocation, R_ActorNetworkedLocation, DeltaTime * ReplicationInterpolationSpeed, EEasingFunc::Linear));
		}

		// Lerp actor rotation
		const FRotator& actorRotation = GetActorRotation();
		if (actorRotation != R_ActorNetworkedRotation)
		{
			SetActorRotation(UKismetMathLibrary::REase(actorRotation, R_ActorNetworkedRotation, DeltaTime * ReplicationInterpolationSpeed, true, EEasingFunc::Linear));
		}
	}
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::Replicate()
{
	// Check if this pawn is controlled locally (Client).
	if (IsOwnedBy(GetWorld()->GetFirstPlayerController()))
	{
		Server_Replicate(NetworkSerialize());
	}
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::Server_Replicate_Implementation(const TArray<uint8>& Data)
{
#if UE_SERVER
	NetworkDeserialize(Data);
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

bool ANetworkedActor::Server_Replicate_Validate(const TArray<uint8>& Data)
{
	return true;
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::SetReplicationTimeInterval(float NewInterval)
{
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	GetWorld()->GetTimerManager().ClearTimer(ReplicationTimerHandle);
	ReplicationTimeInterval = NewInterval;
	if (ReplicationTimeInterval > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ReplicationTimerHandle, this, &ANetworkedActor::Replicate, ReplicationTimeInterval, true, ReplicationTimeInterval);
	}
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::SetReplicationInterpolationSpeed(float NewSpeed)
{
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	ReplicationInterpolationSpeed = NewSpeed;
#endif
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::BitSerialize(TArray<uint8>& Array, FMemoryWriter& Writer)
{
	/*
	String representing which values will be sent to the server over RPC (bit positions):
	0 - Actor Location
	1 - Actor Rotation
	2 - Actor VisibleMesh/Camera location
*/
	FVector actorLocation = GetActorLocation();
	FRotator actorRotation = GetActorRotation();
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
	Array[0] = bitmask;
}

/* ---------------------------------------------------------------------------------------------------- */

void ANetworkedActor::BitDeserialize(const TArray<uint8>& Array, FMemoryReader& Reader)
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
}

/* ---------------------------------------------------------------------------------------------------- */
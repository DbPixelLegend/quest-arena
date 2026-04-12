// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetworkSerializable.h"
#include "NetworkedActor.generated.h"

UCLASS()
class QUESTARENA_API ANetworkedActor : public AActor, public INetworkSerializable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetworkedActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:

	// Minimum threshold for location replication
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Serialization")
	float ReplicationLocationThreshold = 1.f;

	// Minimum threshold for rotation replication
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Serialization")
	float ReplicationRotationThreshold = 1.f;

	// Minimum threshold for scale replication
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Serialization")
	float ReplicationScaleThreshold = 1.f;

	virtual void BitSerialize(TArray<uint8>& Array, FMemoryWriter& Writer) override;
	virtual void BitDeserialize(const TArray<uint8>& Array, FMemoryReader& Reader) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	// How often NetworkedPawn class should be replicated over the network.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Serialization")
	float ReplicationTimeInterval = 0.1f;

	// How often NetworkedPawn class should be replicated over the network.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Serialization")
	float ReplicationInterpolationSpeed = 20.f;

	UFUNCTION(BlueprintInternalUseOnly, Category = "Network")
	void Replicate();

	UFUNCTION(Server, Reliable, WithValidation, Category = "Network")
	void Server_Replicate(const TArray<uint8>& Data);

	/*
		These variables represent what was sent last time to the Master Server.

		IMPORTANT NOTICE:
		If this actor is PROXY then this variables store last received data from the Master Server.
	*/

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	FVector R_ActorNetworkedLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	FRotator R_ActorNetworkedRotation;

public:

	UFUNCTION(Exec, Category = ExecFunctions)
	void SetReplicationTimeInterval(float NewInterval);

	UFUNCTION(Exec, Category = ExecFunctions)
	void SetReplicationInterpolationSpeed(float NewSpeed);

private:

	FTimerHandle ReplicationTimerHandle;
};

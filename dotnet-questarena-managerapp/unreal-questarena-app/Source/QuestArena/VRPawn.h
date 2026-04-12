// Pixel Legend 2018, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "NetworkedPawn.h"
#include "InputCore/Public/InputCoreModule.h"
#include "VRPawn.generated.h"

class AMotionControllerBase;
class ANetworkedGun;
class AVRPawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorPickup, AActor*, PickedUp, EControllerHand, Hand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorDrop, AActor*, PickedUp, EControllerHand, Hand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGunFired);

UCLASS(Blueprintable)
class QUESTARENA_API AVRPawn : public ANetworkedPawn
{

	GENERATED_BODY()
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Network|Serialization")
	AMotionControllerBase* GetHandController(EControllerHand HandEnum);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VRPawn", meta = (WorldContext="WorldRef"))
	static AMotionControllerBase* StaticGetHandController(const UObject* WorldRef, EControllerHand HandEnum);

	UFUNCTION(BlueprintCallable)
	void GrabGun(AActor* GunToGrab, EControllerHand Hand);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Network")
	void SERVER_GrabGun(AActor* GunToGrab, EControllerHand Hand);

	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MULTICAST_GrabGun(AActor* GunToGrab, AVRPawn* GrabbingPawn, EControllerHand Hand);

	UFUNCTION(BlueprintCallable)
	void DropGun(AActor* GunToDrop, EControllerHand Hand);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Network")
	void SERVER_DropGun(AActor* GunToDrop, AVRPawn* GrabbingPawn, EControllerHand Hand);

	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MULTICAST_DropGun(AActor* GunToDrop, AVRPawn* GrabbingPawn, EControllerHand Hand);

	UFUNCTION(BlueprintCallable)
	void ShootGun(AActor* GunToShoot, EControllerHand Hand);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Network")
	void SERVER_ShootGun(AActor* GunToShoot, EControllerHand Hand);

	UFUNCTION(NetMulticast, Unreliable, Category = "Network")
	void MULTICAST_ShootGun(AActor* GunToShoot, EControllerHand Hand);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShootGunEvent(EControllerHand Hand);

	UPROPERTY(BlueprintAssignable)
	FOnActorPickup OnActorPickupDispatcher;

	UPROPERTY(BlueprintAssignable)
	FOnActorDrop OnActorDropDispatcher;

	UPROPERTY(BlueprintAssignable)
	FOnGunFired Serveronly_OnGunFired;

protected:

	virtual void BitSerialize(TArray<uint8>& Array, FMemoryWriter& Writer) override;
	virtual void BitDeserialize(const TArray<uint8>& Array, FMemoryReader& Reader) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	FVector R_LeftMCNetworkedLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	FRotator R_LeftMCNetworkedRotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	FVector R_RightMCNetworkedLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	FRotator R_RightMCNetworkedRotation;
};

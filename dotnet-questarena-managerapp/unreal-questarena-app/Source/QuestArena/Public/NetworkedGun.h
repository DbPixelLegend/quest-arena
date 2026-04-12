// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkedActor.h"
#include "IPickable.h"
#include "Resetable.h"
#include "NetworkedGun.generated.h"

/**
 * 
 */
UCLASS()
class QUESTARENA_API ANetworkedGun : public AActor, public IPickable, public IResetable
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pickable")
	FString SocketName;

	ANetworkedGun();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* ----------------------------------------- Pickup --------------------------------------------------- */

	virtual void PickupImplementation(AActor* PickedBy, EControllerHand Hand) override;
	virtual void DropImplementation(AActor* DropBy, EControllerHand Hand) override;

	/* ---------------------------------------------------------------------------------------------------- */

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_OnNetworkedOwnerChanged, BlueprintReadOnly, VisibleAnywhere)
	AActor* R_Owner; //VRPawn

	UPROPERTY(ReplicatedUsing = OnRep_OnNetworkedHandChanged, BlueprintReadOnly, VisibleAnywhere)
	EControllerHand R_HandAttachedTo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform StartTransform;

	virtual FString GetPickupSocket() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SERVER_Fire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CLIENTONLY_Fire();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MULTICAST_Restart(FTransform NewTransform);

	virtual bool ServerRestart_Implementation() override;

protected:

	UFUNCTION()
	void OnRep_OnNetworkedOwnerChanged();

	UFUNCTION()
	void OnRep_OnNetworkedHandChanged();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	UStaticMeshComponent* GetPhysicsObject();

	void UpdateGunAttachment();
};

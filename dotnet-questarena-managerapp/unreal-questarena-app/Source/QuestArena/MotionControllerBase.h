// Pixel Legend 2018, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerBase.generated.h"

class ANetworkedGun;

UENUM(BlueprintType)
enum class EHandGripState : uint8
{
	HGS_Open,
	HGS_Grab
};

UCLASS()
class QUESTARENA_API AMotionControllerBase : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter = Setter_OnGripStateChanged)
	EHandGripState GripState;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Network|Motion Control")
	FVector GetMotionControllerLocation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Network|Motion Control")
	void SetMotionControllerLocation(UPARAM(ref) const FVector& NewLocation);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Network|Motion Control")
	FRotator GetMotionControllerRotation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Network|Motion Control")
	void SetMotionControllerRotation(UPARAM(ref) const FRotator& NewRotation);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Network|Motion Control")
	void AttachGunToHand(AActor* Gun);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Network|Motion Control")
	void DropGunFromHand(AActor* Gun);

protected:

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Network|Motion Control")
	void Setter_OnGripStateChanged(EHandGripState NewState);

};

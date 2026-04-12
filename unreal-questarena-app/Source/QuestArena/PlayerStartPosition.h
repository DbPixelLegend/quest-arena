// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerStartPosition.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UTextRenderComponent;

UCLASS()
class QUESTARENA_API APlayerStartPosition : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	APlayerStartPosition();

	APlayerStartPosition(const FObjectInitializer& Initializer);

#if WITH_EDITORONLY_DATA
	void SetIndex(int32 index);
#endif

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const bool IsOccupied();

private:

	// START EDITOR ONLY
	UPROPERTY()
	UStaticMeshComponent* FlagPostComponent;
	UPROPERTY()
	UStaticMeshComponent* FlagComponent;
	UPROPERTY()
	UTextRenderComponent* IndexTextRenderer;
	// END EDITOR ONLY
};

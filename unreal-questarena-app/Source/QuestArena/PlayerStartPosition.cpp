// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStartPosition.h"
#include "Components/SceneComponent.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextRenderComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APlayerStartPosition::APlayerStartPosition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

APlayerStartPosition::APlayerStartPosition(const FObjectInitializer& Initializer) : Super(Initializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

#if !UE_SERVER
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> cubeMat(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'"));
	FlagPostComponent = CreateDefaultSubobject<UStaticMeshComponent>(MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), TEXT("FlagPostComponent")));
	FlagComponent = CreateDefaultSubobject<UStaticMeshComponent>(MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), TEXT("FlagComponent")));
	FlagPostComponent->SetupAttachment(RootComponent);
	FlagPostComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	FlagPostComponent->SetRelativeScale3D(FVector(0.01f, 0.01f, 1.f));
	FlagPostComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagComponent->SetupAttachment(RootComponent);
	FlagComponent->SetRelativeLocation(FVector(20.f, 0.f, 80.f));
	FlagComponent->SetRelativeScale3D(FVector(0.4f, 0.01f, 0.4f));
	FlagComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (cubeMesh.Object)
	{
		FlagPostComponent->SetStaticMesh(cubeMesh.Object);
		FlagComponent->SetStaticMesh(cubeMesh.Object);
	}

	IndexTextRenderer = CreateDefaultSubobject<UTextRenderComponent>(MakeUniqueObjectName(this, UTextRenderComponent::StaticClass(), TEXT("FlagIndex")));
	IndexTextRenderer->SetupAttachment(RootComponent);
	IndexTextRenderer->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	IndexTextRenderer->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	IndexTextRenderer->SetRelativeLocation(FVector(20.f, 0.f, 110.f));
	IndexTextRenderer->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0.f, 0.f, 90.f)));
#endif

#if !WITH_EDITOR
	SetActorHiddenInGame(true);
#endif
}

#if WITH_EDITOR
void APlayerStartPosition::SetIndex(int32 index)
{
	if (IndexTextRenderer)
	{
		IndexTextRenderer->SetText(FText::FromString(*FString::FromInt(index)));
	}
}
#endif

const bool APlayerStartPosition::IsOccupied()
{
	TArray<AActor*> occupants;
	const TArray< AActor*> ignore;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };
	UKismetSystemLibrary::CapsuleOverlapActors(this, GetActorLocation(), 50, 50, objectTypes, AActor::StaticClass(), ignore, occupants);
	return occupants.Num() > 0 ? true : false;
}
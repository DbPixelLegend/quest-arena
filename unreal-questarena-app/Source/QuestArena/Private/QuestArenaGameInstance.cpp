// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestArenaGameInstance.h"

#include "Components/WidgetInteractionComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "UObject/UnrealType.h"

void UQuestArenaGameInstance::Init()
{
	Super::Init();

	if (!PostWorldInitDelegateHandle.IsValid())
	{
		PostWorldInitDelegateHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
			this, &UQuestArenaGameInstance::HandlePostWorldInitialization);
	}
}

void UQuestArenaGameInstance::Shutdown()
{
	if (PostWorldInitDelegateHandle.IsValid())
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(PostWorldInitDelegateHandle);
		PostWorldInitDelegateHandle.Reset();
	}

	for (TPair<TWeakObjectPtr<UWorld>, FTimerHandle>& Pair : RepairTimerHandles)
	{
		if (Pair.Key.IsValid())
		{
			Pair.Key->GetTimerManager().ClearTimer(Pair.Value);
		}
	}
	RepairTimerHandles.Empty();

	Super::Shutdown();
}

void UQuestArenaGameInstance::HandlePostWorldInitialization(UWorld* World, const UWorld::InitializationValues InitializationValues)
{
	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	FTimerHandle Handle;
	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(
		this, &UQuestArenaGameInstance::RepairTutorialHandsForWorld, TWeakObjectPtr<UWorld>(World));
	World->GetTimerManager().SetTimer(Handle, Delegate, 0.25f, true, 0.5f);
	RepairTimerHandles.Add(World, Handle);
}

void UQuestArenaGameInstance::RepairTutorialHandsForWorld(TWeakObjectPtr<UWorld> WorldPtr)
{
	if (!WorldPtr.IsValid())
	{
		return;
	}

	UWorld* World = WorldPtr.Get();
	const FString MapName = World->GetMapName();
	if (!MapName.Contains(TEXT("TutorialMap")))
	{
		return;
	}

	for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
	{
		AActor* Actor = *ActorIt;
		if (!IsValid(Actor))
		{
			continue;
		}

		const FString ClassName = Actor->GetClass()->GetName();
		if (!ClassName.Contains(TEXT("BP_VRrGunHands")))
		{
			continue;
		}

		EnsureWidgetInteractionReference(Actor);
	}
}

bool UQuestArenaGameInstance::EnsureWidgetInteractionReference(AActor* Actor) const
{
	static const FName WidgetInteractionPropertyName(TEXT("WidgetInteraction"));
	FObjectProperty* WidgetInteractionProperty = FindFProperty<FObjectProperty>(
		Actor->GetClass(), WidgetInteractionPropertyName);
	if (WidgetInteractionProperty == nullptr)
	{
		return false;
	}

	UObject* CurrentObject = WidgetInteractionProperty->GetObjectPropertyValue_InContainer(Actor);
	if (IsValid(CurrentObject))
	{
		return false;
	}

	UWidgetInteractionComponent* ReplacementComponent = Actor->FindComponentByClass<UWidgetInteractionComponent>();
	if (!IsValid(ReplacementComponent))
	{
		ReplacementComponent = NewObject<UWidgetInteractionComponent>(
			Actor, UWidgetInteractionComponent::StaticClass(), TEXT("WidgetInteraction_RuntimeFix"));
		if (!IsValid(ReplacementComponent))
		{
			return false;
		}

		if (IsValid(Actor->GetRootComponent()))
		{
			ReplacementComponent->SetupAttachment(Actor->GetRootComponent());
		}

		ReplacementComponent->RegisterComponent();
	}

	WidgetInteractionProperty->SetObjectPropertyValue_InContainer(Actor, ReplacementComponent);
	return true;
}


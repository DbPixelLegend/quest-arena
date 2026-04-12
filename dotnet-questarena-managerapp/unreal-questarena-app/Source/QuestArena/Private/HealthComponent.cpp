#include "HealthComponent.h"
#include "UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UHealthComponent::UHealthComponent() { }

UHealthComponent::~UHealthComponent() { }

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHealthComponent, MaxHealth, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UHealthComponent, HealthLeft, COND_InitialOnly);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	bReplicates = true;

	if (HasAnyFlags(EObjectFlags::RF_ArchetypeObject)) return; //don't do spawn health checks for the archetype object

	//validate initial values on server. no need to do this on client because they are read-only and might not even be replicated yet
	if (UKismetSystemLibrary::IsServer(this))
	{
		if (MaxHealth <= 0.0f)
		{
			#if WITH_EDITOR
			UE_LOG(LogTemp, Error, TEXT("HealthComponent (%s) has MaxHealth smaller than or equal 0. This WILL crash in production"), *(GetOwner()->GetName()));
			#else
			UE_LOG(LogTemp, Fatal, TEXT("HealthComponent (%s) has MaxHealth smaller than or equal 0."), *(GetOwner()->GetName()));
			#endif
		}

		if (HealthLeft <= 0.0f || HealthLeft > MaxHealth)
		{
			UE_LOG(LogTemp, Error, TEXT("HealthComponent (%s) has invalid initial HealthLeft value (should be greater than 0 but smaller (or equal) to Max. Setting Max)"), *(GetOwner()->GetName()));
			HealthLeft = MaxHealth;
			ensure(false);

			#if WITH_EDITOR || UE_SERVER
			InitialMaxHealth = MaxHealth;
			InitialHealthLeft = HealthLeft;
			#endif
		}
	}
}

void UHealthComponent::Multicast_OnHealthChanged_Implementation(float HealthChange, float CurrentHealth, float _MaxHealth)
{
	//accept data from server on clients
	if (!UKismetSystemLibrary::IsServer(this))
	{
		HealthLeft = CurrentHealth;
		MaxHealth = _MaxHealth;
	}

	OnHealthChanged.Broadcast(HealthChange, HealthLeft, MaxHealth);
	if (CurrentHealth <= 0.0f) OnHealthDepleted.Broadcast();
}

void UHealthComponent::ApplyHealthBoost(FVector2D BoostRatio)
{
	if (!UKismetSystemLibrary::IsServer(this)) //clients can't heal anything directly
	{
		ensureMsgf(false, TEXT("You can't apply health boosts on clients."));
		return;
	}

	if (BoostRatio.X < 0.0f || BoostRatio.X > BoostRatio.Y)
	{
		ensureMsgf(false, TEXT("Invalid BoostRatio value"));
		return;
	}

	if (HealthLeft <= 0.0f)
	{
		ensureMsgf(false, TEXT("You can't apply health boosts to dead objects!"));
		return;
	}

	float HealthToMaxRatio = HealthLeft / MaxHealth;
	MaxHealth *= UKismetMathLibrary::RandomFloatInRange(BoostRatio.X, BoostRatio.Y);
	HealthLeft = MaxHealth * HealthToMaxRatio;
	Multicast_OnHealthChanged(0.0f, HealthLeft, MaxHealth); //0 change means change through health boost instead of damage/heal
}

void UHealthComponent::SubtractHealth(float Amount)
{
	if (!UKismetSystemLibrary::IsServer(this)) //clients can't hurt anything directly
	{
		ensureMsgf(false, TEXT("You can't subtract health on clients."));
		return;
	}

	if(Amount < 0.0f)
	{
		ensureMsgf(false, TEXT("You can't subtract negative amount of health. Use AddHealth instead"));
		return;
	}

	if (HealthLeft <= 0.0f)
	{
		ensureMsgf(false, TEXT("You can't subtract health on dead objects!"));
		return;
	}
	
	HealthLeft -= Amount;
	
	if (HealthLeft < 0.0f) HealthLeft = 0.0f;
	
	Multicast_OnHealthChanged(-Amount, HealthLeft, MaxHealth);
	//NOTE: don't dispatch OnHealthDepleted here - the multicast method handles this
}

void UHealthComponent::AddHealth(float Amount)
{
	if (!UKismetSystemLibrary::IsServer(this)) //clients can't heal anything directly
	{
		ensureMsgf(false, TEXT("You can't add health on clients."));
		return;
	}

	if(Amount > 0.0f)
	{
		ensureMsgf(false, TEXT("You can't add negative amount of health. Use SubtractHealth instead"));
		return;
	}
	
	if (HealthLeft <= 0.0f)
	{
		ensureMsgf(false, TEXT("You can't add health to dead objects!"));
		return;
	}

	HealthLeft += Amount;
	
	if (HealthLeft > MaxHealth)
	{
		Amount -= HealthLeft - MaxHealth;
		HealthLeft = MaxHealth;
	}

	Multicast_OnHealthChanged(Amount, HealthLeft, MaxHealth);
}

void UHealthComponent::ResetHealth()
{
	if (!UKismetSystemLibrary::IsServer(this)) //clients can't reset anything
	{
		ensureMsgf(false, TEXT("You can't reset health on clients."));
		return;
	}

	#if WITH_EDITOR || UE_SERVER
	//@todo: check why initials are equal 0
	//MaxHealth  = InitialMaxHealth;
	//HealthLeft = InitialHealthLeft;

	//only for testing. this code is incorrect in situations where initial HealthLeft doesn't equal MaxHealth
	HealthLeft = MaxHealth;
	#endif

	Multicast_OnHealthChanged(0, HealthLeft, MaxHealth);
}

bool UHealthComponent::IsDead()
{
	return (HealthLeft <= 0.0f);
}

#pragma once
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, HealthChange, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepleted);

UCLASS(Blueprintable)
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	
	//---------------------- construction ----------------------
	UHealthComponent();
	~UHealthComponent();
	virtual void BeginPlay() override;

	//----------------------- replication ----------------------

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//------------------------ delegates -----------------------
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHealthDepleted OnHealthDepleted;

	//------------------ dispatch over the net -----------------

	UFUNCTION(NetMulticast, reliable)
	void Multicast_OnHealthChanged(float HealthChange, float CurrentHealth, float _MaxHealth);
	void Multicast_OnHealthChanged_Implementation(float HealthChange, float CurrentHealth, float MaxHealth);

	//------------------------ fields --------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated) //COND_InitialOnly, later replicated by RPC (Multicast_OnHealthChanged)
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated) //COND_InitialOnly, later replicated by RPC (Multicast_OnHealthChanged)
	float HealthLeft;

	#if WITH_EDITOR || UE_SERVER

	//for reset purposes
	float InitialMaxHealth;
	float InitialHealthLeft;

	#endif

	//------------------------ methods -------------------------
	
	//Applies randomly defined health boost between FVector's X and Y values.
	//Will Multicast_OnHealthChanged with HealthChange = 0.
	UFUNCTION(BlueprintCallable)
	void ApplyHealthBoost(FVector2D BoostRatio);

	//Subtracts health from the object and calls OnDestructibleHealthChanged with negative HealthChange value
	//If final health is <=0, additionally calls OnDestructibleDestroyed.
	//
	// NOTE: broadcasted HealthChange will include the actual amount of damage, ie. if the object
	// has 10HP left and receives 15HP damage, the broadcasted change will still be -15HP (not -10).
	UFUNCTION(BlueprintCallable)
	void SubtractHealth(float Amount);
	
	//Adds health from the object and calls OnDestructibleHealthChanged with positive HealthChange value
	//
	// NOTE: broadcasted HealthChange will NOT include the actual amount of health restoration, ie. if
	// the object has 10/15 HP and receives 10HP boost, the broadcasted change will be +5HP (not +10).
	UFUNCTION(BlueprintCallable)
	void AddHealth(float Amount);

	//Resets health to default value
	UFUNCTION(BlueprintCallable)
	void ResetHealth();

	UFUNCTION(BlueprintPure)
	bool IsDead();

};

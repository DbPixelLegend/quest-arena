#include "ZombieSpawnerConfig.h"
#include "Kismet/KismetMathLibrary.h"

TSubclassOf<APawn> FWeightedClasses::GetRandomClassWithWeight() const
{
	float TotalWeight = 0.0f;
	for (const FWeightedClass& Class : Classes) TotalWeight += Class.RandomWeight;

	float TargetWeight = UKismetMathLibrary::RandomFloatInRange(0, TotalWeight);
	
	TotalWeight = 0.0f;
	for (const FWeightedClass& Class : Classes)
	{
		TotalWeight += Class.RandomWeight;
		if (TargetWeight <= TotalWeight) return Class.ActorClass;
	}

	return APawn::StaticClass();
}

TSubclassOf<APawn> USpawnerConfigHelper::GetRandomClassWithWeight(const FZombieSpawnerConfig& Config)
{
	return Config.SpawnedClasses.GetRandomClassWithWeight();
}

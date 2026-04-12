// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "ZombieSpawnerConfig.generated.h"

UENUM(Blueprintable)
enum class WaveStatus : uint8
{
	SpawningZombies,
	WaitingToKillRemaining,
	WaveEnded
};

USTRUCT(BlueprintType)
struct FWeightedClass
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RandomWeight;
};

USTRUCT(BlueprintType)
struct FWeightedClasses
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWeightedClass> Classes;

	TSubclassOf<APawn> GetRandomClassWithWeight() const; //UFUNCTIONed by USpawnerConfigHelper below
};

USTRUCT(BlueprintType)
struct FZombieSpawnerConfig
{
	GENERATED_BODY()
	
public:

	//is the first spawn after activation immediate or do we wait the interval
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bImmediateFirstSpawn;

	//max alive zombies from this spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxZombiesFromThisSpawn;

	//max alive zombies on arena
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxZombiesOnLevel;

	//float range in seconds between zombie spawns
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpawnInterval;

	//array of zombie classes spawned from this spawner with random weights
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeightedClasses SpawnedClasses;

	//float range of per-zombie health boost (ie. 1.5f boost on zombie class with 20HP will give it 30HP)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D HealthBoostRange;

	//float range of per-zombie speed boost (ie. 0.8f boost on zombie class with 100 speed will give it 80)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedBoostRange;

	FZombieSpawnerConfig()
		: bImmediateFirstSpawn(false)
		, MaxZombiesFromThisSpawn(3)
		, MaxZombiesOnLevel(10)
		, SpawnInterval(FVector2D(10.0f, 15.0f))
	  //, SpawnedClasses() - empty by default
		, HealthBoostRange(FVector2D(0.9f, 1.1f))
		, SpeedBoostRange(FVector2D(0.9f, 1.1f))
	{ }
};

UCLASS()
class USpawnerConfigHelper : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static TSubclassOf<APawn> GetRandomClassWithWeight(UPARAM(ref) const FZombieSpawnerConfig& Config);
};

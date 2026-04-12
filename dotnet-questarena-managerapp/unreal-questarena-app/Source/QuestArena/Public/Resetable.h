// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Resetable.generated.h"

UINTERFACE(BlueprintType)
class QUESTARENA_API UResetable : public UInterface
{
	GENERATED_BODY()
};

class QUESTARENA_API IResetable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ServerRestart();
};
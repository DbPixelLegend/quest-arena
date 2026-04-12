#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Resetable.h"
#include "AiCharacter.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None = 0,
	Melee = 1
};

UCLASS(config=Game)
class AAiCharacter : public ACharacter, public IResetable
{
	GENERATED_BODY()

public:
	AAiCharacter();
	
	//------------------IResetable interface
	virtual bool ServerRestart_Implementation() override;
};

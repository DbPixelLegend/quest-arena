#pragma once
#include "TutorialStepBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialStepEnded);

UCLASS(Blueprintable)
class ATutorialStepBase : public AActor
{
	GENERATED_BODY()
	
public:
	
	//---------------------- construction ----------------------
	ATutorialStepBase();
	~ATutorialStepBase();
	
	//------------------------ delegates ------------------------
	
	UPROPERTY(BlueprintAssignable)
	FOnTutorialStepEnded OnTutorialStepEnded;

	//------------------------ methods ------------------------
	
	UFUNCTION(BlueprintNativeEvent)
	void BeginStep();
	virtual void BeginStep_Implementation() const;
	
	UFUNCTION(BlueprintCallable)
	void EndStep();

};

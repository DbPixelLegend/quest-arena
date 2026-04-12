#pragma once
#include "TutorialStepBase.h"
#include "TutorialManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTutorialProgress, int32, StepsDone, int32, StepsTotal);

UENUM()
enum class ETutorialState : uint8
{
	NotReady = 0,
	Ready    = 1,
	Running  = 2,
	Complete = 3
};

UCLASS(Blueprintable)
class ATutorialManager : public AActor
{
	GENERATED_BODY()
	
public:
	
	//---------------------- construction ----------------------
	ATutorialManager();
	~ATutorialManager();
	
	//------------------------ delegates ------------------------
	UPROPERTY(BlueprintAssignable)
	FOnTutorialStateChanged OnTutorialStarted;
	
	UPROPERTY(BlueprintAssignable)
	FOnTutorialStateChanged OnTutorialEnded;

	UPROPERTY(BlueprintAssignable)
	FOnTutorialProgress OnTutorialStepFinished;
	
	//------------------------- fields -------------------------
	UPROPERTY(BlueprintReadOnly)
	FTimespan TutorialCompletedIn;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<ATutorialStepBase>> Steps;
	
	//------------------------ methods ------------------------
	
	UFUNCTION(BlueprintCallable)
	bool AddStep(TSubclassOf<ATutorialStepBase> StepClass);

	UFUNCTION(BlueprintCallable)
	bool SetSteps(UPARAM(ref) const TArray<TSubclassOf<ATutorialStepBase>>& StepClassArray);
	
	UFUNCTION(BlueprintCallable)
	void BeginTutorial();

	UFUNCTION(BlueprintPure)
	ETutorialState GetTutorialState() const;

private:
	int CurrentTutorialStepId;
	ATutorialStepBase* CurrentTutorialStep;
	FDateTime TutorialStartedTimestamp;
	
	UFUNCTION()
	void BeginNextStep();
};

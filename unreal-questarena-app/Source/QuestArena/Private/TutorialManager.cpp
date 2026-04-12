#include "TutorialManager.h"

ATutorialManager::ATutorialManager()
	: CurrentTutorialStepId(-1) { }
	
ATutorialManager::~ATutorialManager() { }

bool ATutorialManager::AddStep(TSubclassOf<ATutorialStepBase> StepClass)
{
	if(GetTutorialState() > ETutorialState::Ready) return false;
	ensureMsgf(Steps.Find(StepClass) == INDEX_NONE, TEXT("Requested PushNewStep() to add the same step more than once. Is this by design?"));
	Steps.Add(StepClass);
	return true;
}

bool ATutorialManager::SetSteps(const TArray<TSubclassOf<ATutorialStepBase>>& StepClassArray)
{
	if(GetTutorialState() > ETutorialState::Ready) return false;
	Steps = StepClassArray;
	return true;
}

void ATutorialManager::BeginTutorial()
{
	if(GetTutorialState() != ETutorialState::Ready)
	{
		ensureMsgf(false, TEXT("Requested BeginTutorial() twice or there are no steps available"));
		return;
	}
	
	TutorialStartedTimestamp = FDateTime::UtcNow();
	OnTutorialStarted.Broadcast();
	BeginNextStep();
}

void ATutorialManager::BeginNextStep()
{
	++CurrentTutorialStepId;
	OnTutorialStepFinished.Broadcast(CurrentTutorialStepId, Steps.Num());
	if(CurrentTutorialStep->IsValidLowLevel()) CurrentTutorialStep->Destroy();

	if(CurrentTutorialStepId == Steps.Num())
	{
		TutorialCompletedIn = FDateTime::UtcNow() - TutorialStartedTimestamp;
		OnTutorialEnded.Broadcast();
	}
	else
	{
		CurrentTutorialStep = (ATutorialStepBase*)GetWorld()->SpawnActor(Steps[CurrentTutorialStepId]);
		CurrentTutorialStep->OnTutorialStepEnded.AddDynamic(this, &ATutorialManager::BeginNextStep);
		CurrentTutorialStep->BeginStep();
	}
}

ETutorialState ATutorialManager::GetTutorialState() const
{
	if(Steps.Num() == 0) return ETutorialState::NotReady;
	if(CurrentTutorialStepId == -1) return ETutorialState::Ready;
	return (CurrentTutorialStepId == Steps.Num()) ? ETutorialState::Complete : ETutorialState::Running;
}

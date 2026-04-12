#include "TutorialStepBase.h"

ATutorialStepBase::ATutorialStepBase() { }
ATutorialStepBase::~ATutorialStepBase() { }

void ATutorialStepBase::BeginStep_Implementation() const
{
	check(false); // Base (pure virtual) implementation of ATutorialStepBase called"
}

void ATutorialStepBase::EndStep()
{
	OnTutorialStepEnded.Broadcast();
}

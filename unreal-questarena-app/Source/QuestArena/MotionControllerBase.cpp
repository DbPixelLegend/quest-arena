// Pixel Legend 2018, All Rights Reserved

#include "MotionControllerBase.h"
#include "QuestArena.h"

void AMotionControllerBase::Setter_OnGripStateChanged(EHandGripState NewState)
{
	GripState = NewState;
}

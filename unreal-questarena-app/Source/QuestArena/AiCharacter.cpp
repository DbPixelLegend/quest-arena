#include "AiCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AAiCharacter::AAiCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
}

bool AAiCharacter::ServerRestart_Implementation()
{
	if (!UKismetSystemLibrary::IsServer(this)) //clients can't reset anything
	{
		ensureMsgf(false, TEXT("You can't call IResetable::Reset() on clients."));
		return false;
	}

	Destroy();
	return true;
}

// Pixel Legend 2018, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "MemoryWriter.h"
#include "MemoryReader.h"
#include "NetworkSerializable.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class QUESTARENA_API UNetworkSerializable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class QUESTARENA_API INetworkSerializable
{
	GENERATED_IINTERFACE_BODY()

public:

	// --- DON'T OVERRIDE THESE FUCKS, THEY ARE VIRTUAL ONLY BECAUSE UHT IS A BITCH ---

	UFUNCTION(BlueprintCallable)
	virtual TArray<uint8> NetworkSerialize();

	UFUNCTION(BlueprintCallable)
	virtual void NetworkDeserialize(const TArray<uint8>& Data);

	// -------------------------- OVERRIDE THESE INSTEAD :3  --------------------------

	virtual void BitSerialize(TArray<uint8>& Array, FMemoryWriter& Writer) = 0;
	virtual void BitDeserialize(const TArray<uint8>& Array, FMemoryReader& Reader) = 0;
};

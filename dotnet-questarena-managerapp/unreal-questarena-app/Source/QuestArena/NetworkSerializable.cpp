// Pixel Legend 2018, All Rights Reserved

#include "NetworkSerializable.h"
#include "QuestArena.h"
#include <bitset>
#include <string>
#include "MemoryWriter.h"
#include "MemoryReader.h"

/* ---------------------------------------------------------------------------------------------------- */

UNetworkSerializable::UNetworkSerializable(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

/* ---------------------------------------------------------------------------------------------------- */

TArray<uint8> INetworkSerializable::NetworkSerialize()
{
	TArray<uint8> serializedObject = TArray<uint8>();
	FMemoryWriter ar = FMemoryWriter(serializedObject);
	BitSerialize(serializedObject, ar);
	return serializedObject;
}

/* ---------------------------------------------------------------------------------------------------- */

void INetworkSerializable::NetworkDeserialize(const TArray<uint8>& Data)
{
	FMemoryReader ar = FMemoryReader(Data);
	BitDeserialize(Data, ar);
}

/* ---------------------------------------------------------------------------------------------------- */

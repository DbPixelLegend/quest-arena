// Copyright 2019 Pixel Legend Sp. z o.o.. All Rights Reserved.
// Access to the source code outside of Pixel Legend is prohibited.

#include "ArenaHeadsetConfig.h"
#include "Kismet/GameplayStatics.h"

#define SAV_NAME "ArenaHeadsetConfig"
UArenaHeadsetConfig* UArenaHeadsetConfig::SaveObject = nullptr;

//------------------ getters

UArenaHeadsetConfig* UArenaHeadsetConfig::GetSaveObject()
{
	if (!SaveObject->IsValidLowLevel())
	{
		SaveObject = Cast<UArenaHeadsetConfig>(UGameplayStatics::LoadGameFromSlot(SAV_NAME, 0)); //sets SaveObject in constructor
		if (SaveObject == nullptr)
		{
			//default object
			SaveObject = NewObject<UArenaHeadsetConfig>(UArenaHeadsetConfig::StaticClass());
			SaveObject->ServerIpAddressByte1 = 10;
			SaveObject->ServerIpAddressByte2 = 70;
			SaveObject->ServerIpAddressByte3 = 32;
			SaveObject->ServerIpAddressByte4 = 71;
			SaveObject->ServerPort = 6969;
			SaveObject->ClientHeadsetId = 0;
			UGameplayStatics::SaveGameToSlot(SaveObject, SAV_NAME, 0);
		}
	}
	return SaveObject;
}

UArenaHeadsetConfig::UArenaHeadsetConfig() { }
UArenaHeadsetConfig::~UArenaHeadsetConfig() { SaveObject = nullptr; }

void UArenaHeadsetConfig::GetServerIpAddressAsBytes(uint8& B1, uint8& B2, uint8& B3, uint8& B4)
{
	B1 = ServerIpAddressByte1;
	B2 = ServerIpAddressByte2;
	B3 = ServerIpAddressByte3;
	B4 = ServerIpAddressByte4;
}

void UArenaHeadsetConfig::GetServerIpAddressAsString(bool bWithPort, FString& Ip)
{
	if (bWithPort)
	{
		Ip = FString::Printf(TEXT("%d.%d.%d.%d:%d"), ServerIpAddressByte1, ServerIpAddressByte2, ServerIpAddressByte3, ServerIpAddressByte4, ServerPort);
	}
	else
	{
		Ip = FString::Printf(TEXT("%d.%d.%d.%d"), ServerIpAddressByte1, ServerIpAddressByte2, ServerIpAddressByte3, ServerIpAddressByte4);
	}
}

int32 UArenaHeadsetConfig::GetServerPort()
{
	return ServerPort;
}

uint8 UArenaHeadsetConfig::GetHeadsetId()
{
	return ClientHeadsetId;
}

//------------------ setters

void UArenaHeadsetConfig::SetServerIpAddress(uint8 B1, uint8 B2, uint8 B3, uint8 B4)
{
	ServerIpAddressByte1 = B1;
	ServerIpAddressByte2 = B2;
	ServerIpAddressByte3 = B3;
	ServerIpAddressByte4 = B4;
	UGameplayStatics::SaveGameToSlot(this, SAV_NAME, 0);
}

void UArenaHeadsetConfig::SetServerPort(int32 Port)
{
	check(Port < 65536); //Port is actually an int16
	ServerPort = Port;
	UGameplayStatics::SaveGameToSlot(this, SAV_NAME, 0);
}

void UArenaHeadsetConfig::SetHeadsetId(uint8 NewId)
{
	ClientHeadsetId = NewId;
	UGameplayStatics::SaveGameToSlot(this, SAV_NAME, 0);
}

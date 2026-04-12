// Copyright 2019 Pixel Legend Sp. z o.o.. All Rights Reserved.
// Access to the source code outside of Pixel Legend is prohibited.

#pragma once
#include "GameFramework/SaveGame.h"
#include "ArenaHeadsetConfig.generated.h"

UCLASS()
class QUESTARENA_API UArenaHeadsetConfig : public USaveGame
{
	GENERATED_BODY()

private:

	//------------------ singleton
	static UArenaHeadsetConfig* SaveObject;

public:

	UFUNCTION(BlueprintPure)
	static UArenaHeadsetConfig* GetSaveObject();

	UArenaHeadsetConfig();
	~UArenaHeadsetConfig();

	//------------------ fields

	UPROPERTY()
	uint8 ServerIpAddressByte1;
	
	UPROPERTY()
	uint8 ServerIpAddressByte2;
	
	UPROPERTY()
	uint8 ServerIpAddressByte3;
	
	UPROPERTY()
	uint8 ServerIpAddressByte4;
	
	UPROPERTY()
	int32 ServerPort;
	
	UPROPERTY()
	uint8 ClientHeadsetId;

	//------------------ getters

	UFUNCTION(BlueprintPure, Category = "Headset Config")
	void GetServerIpAddressAsBytes(uint8& B1, uint8& B2, uint8& B3, uint8& B4);

	UFUNCTION(BlueprintPure, Category = "Headset Config")
	void GetServerIpAddressAsString(bool bWithPort, FString& Ip);

	UFUNCTION(BlueprintPure, Category = "Headset Config")
	int32 GetServerPort();

	UFUNCTION(BlueprintPure, Category = "Headset Config")
	uint8 GetHeadsetId();

	//------------------ setters

	UFUNCTION(BlueprintCallable, Category = "Headset Config")
	void SetServerIpAddress(uint8 B1, uint8 B2, uint8 B3, uint8 B4);

	UFUNCTION(BlueprintCallable, Category = "Headset Config")
	void SetServerPort(int32 Port);

	UFUNCTION(BlueprintCallable, Category = "Headset Config")
	void SetHeadsetId(uint8 NewId);

};

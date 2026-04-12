// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StatManager.h"
#include "ArenaPlayerController.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStatsReceived, FGameStats, Stats);

UCLASS(BlueprintType)
class QUESTARENA_API UUserSetupDetails : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void Init(int32 Height, int32 Pos)
	{
		UserHeight = Height;
		UserPos = Pos;
	}

	UFUNCTION(BlueprintCallable)
	const int32 GetHeight() { return UserHeight; }

	UFUNCTION(BlueprintCallable)
	const int32 GetPos() { return UserPos; }

private:

	int32 UserHeight;
	int32 UserPos;
};

UCLASS()
class QUESTARENA_API AArenaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

#if UE_EDITOR
	static const int32 MAX_NUMBER_OF_PLAYERS;
#endif

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=Setter_UserDetails)
	UUserSetupDetails* UserDetails;

	UFUNCTION(BlueprintCallable)
	void Setter_UserDetails(UUserSetupDetails* NewDetails);

	//------------------------------------HANDSHAKES

	UPROPERTY(BlueprintReadOnly)
	uint8 ArenaPlayerId;

	//client sends server its Headset ID to register in the game
	UFUNCTION(Server, reliable, WithValidation)
	void ClientToServer_Handshake(uint8 TargetPlayerId);
	void ClientToServer_Handshake_Implementation(uint8 TargetPlayerId);
	bool ClientToServer_Handshake_Validate(uint8 TargetPlayerId);

	UFUNCTION(Client, Reliable)
	void ServerToClient_Handshake(FVector StartPos, FRotator StartRot);

	//------------------------------------PLAYER INGAME STATE UPDATE

	//registered client informs server that it's ready to play.
	// @todo: actually make players mark themselves as ready instead of calling it on beginplay
	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation)
	void ClientToServer_MarkReadyToPlay();
	bool ClientToServer_MarkReadyToPlay_Validate();
	void ClientToServer_MarkReadyToPlay_Implementation();

	//server only but not replicated - only ROLE_Authority can execute this.
	//PlayerState broadcasts OnPlayerAliveStateChanged on server after successful exec.
	UFUNCTION(BlueprintCallable)
	bool Server_SpawnPlayer();

	//server only but not replicated - only ROLE_Authority can execute this.
	//PlayerState broadcasts OnPlayerAliveStateChanged on server after successful exec.
	UFUNCTION(BlueprintCallable)
	bool Server_KillPlayer(bool Permanent);

	//------------------------------------MVP RESTART

	//@mvp: after the game is finished, all users are presented with a restart button.
	//When one of the players clicks the button, it gets hidden for everybody and the game restarts.

	UFUNCTION(BlueprintCallable, Server, reliable)
	void ClientToServer_RestartRequested();
	void ClientToServer_RestartRequested_Implementation();

	//----------------------------------STAT MANAGEMENT
	FOnStatsReceived OnStatsReceived;

	UFUNCTION(BlueprintCallable)
	void AskServerForStats(const FOnStatsReceived& Delegate);

	UFUNCTION(server, reliable)
	void ClientToServer_AskForStats(AArenaPlayerController* AskingController);
	void ClientToServer_AskForStats_Implementation(AArenaPlayerController* AskingController);

	UFUNCTION(client, reliable)
	void ServerToClient_SendStats(FGameStats Stats);
	void ServerToClient_SendStats_Implementation(FGameStats Stats);
};

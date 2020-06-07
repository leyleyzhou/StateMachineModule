// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameStateMachineGameMode.generated.h"

UCLASS(minimalapi)
class AGameStateMachineGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameStateMachineGameMode();

public:
	APWGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
	virtual bool ReadyToStartMatch_Implementation();
	virtual bool ReadyToEndMatch_Implementation();

protected:
	/** Called when the state transitions to WaitingToStart */
	virtual void HandleMatchIsWaitingToStart();
	/** Called when the state transitions to InProgress */
	virtual void HandleMatchHasStarted();
	/** Called when the map transitions to WaitingPostMatch */
	virtual void HandleMatchHasEnded();

	void Tick_WaittingForStart(float DeltaSeconds);
	void Tick_InProgress(float DeltaSeconds);
	void Tick_WaitingPostMatch(float DeltaSeconds);

protected:
	void _EndMatch();

public:
	void UpdateCountDownSeconds(int32 inSeconds);
	void UpdateFinishMatchSeconds(int32 inSeconds);

	UFUNCTION()
		void OnSafeCircleChanged();
	UFUNCTION()
		void OnDoubleSafeCircleChanged();

	UFUNCTION()
		void OnCharacterLoadAllMap();
protected:
	float	CountDownTimeSecond;		//count down time from
	bool	CountDownStarted = false;

	float	FinishMatchSecond;			//比赛结束后的退出倒计时

	uint8	ReadEndMatch;

};





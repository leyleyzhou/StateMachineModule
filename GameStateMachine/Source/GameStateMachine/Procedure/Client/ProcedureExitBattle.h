// Copyright 2004-2019 Perfect World Co.,Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "System/Procedure/PWProcedureBase.h"
#include "PWProcedureExitBattle.generated.h"

/**
 *
 */
UCLASS()
class PWGAME_API UPWProcedureExitBattle : public UPWProcedureBase
{
	GENERATED_BODY()

public:
	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_ExitBattle; }
	virtual void Enter();
	virtual void Leave();

	void SetNextState(ProcedureState inNextState) { NextState = inNextState; }
protected:
	//load map interface
	virtual void OnPreLoadMap(const FString& MapName) {}
	virtual void OnPostLoadMap(UWorld* LoadedWorld);
	virtual void OnPostLoadAllMaps();
	//load map interface

	ProcedureState NextState = ProcedureState::ProcedureState_Max;
};

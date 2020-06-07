// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "Procedure/PWProcedureBase.h"
#include "PWProcedureBattleDS.generated.h"

/**
 *
 */
UCLASS()
class PWGAME_API UPWProcedureBattleDS : public UPWProcedureBase
{
	GENERATED_BODY()

public:
	UPWProcedureBattleDS();
	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_BattleDS; }
	virtual void Enter();
	virtual void Leave() override;
protected:
	//load map interface
	virtual void OnPostLoadMap(UWorld* LoadedWorld) override;
	virtual void OnPostLoadAllMaps() override;
	//load map interface

public:
	void StartRestart();

private:
	bool	m_IsRestarting = false;
};

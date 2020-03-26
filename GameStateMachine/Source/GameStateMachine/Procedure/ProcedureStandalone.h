// Copyright 2004-2018 Perfect World Co.,Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PWProcedureBase.h"
#include "PWProcedureStandaloneCopy.generated.h"



UCLASS()
class PWGAME_API UPWProcedureStandaloneCopy : public UPWProcedureBase
{
	GENERATED_BODY()
public:
	virtual ProcedureState GetState() override;
	virtual void Enter() override;
	virtual void Leave() override;

	virtual void OnPostLoadMap(UWorld* LoadedWorld);
	virtual void OnPostLoadAllMaps();

	void UpdateUI();

protected:
	bool	m_bIsStandaloneCopyStarted = false;
};

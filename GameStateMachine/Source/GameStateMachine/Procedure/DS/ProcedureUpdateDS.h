// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Procedure/PWProcedureBase.h"
#include "PWProcedureUpdateDS.generated.h"



UCLASS()
class PWGAME_API UPWProcedureUpdateDS : public UPWProcedureBase
{
	GENERATED_BODY()
public:
	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_Update; }
	virtual void Enter();


protected:
	void StartUpdate();

	void OnUpdateFinished();


	FString UpdateRootAddr;
	int32 UnfinishedHttpRequest = 0;
};

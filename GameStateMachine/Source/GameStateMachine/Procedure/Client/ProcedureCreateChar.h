// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "Procedure/PWProcedureBase.h"
#include "PWProcedureCreateChar.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class PWGAME_API UPWProcedureCreateChar : public UPWProcedureBase
{
	GENERATED_BODY()
public:
	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_CreateChar; }
	virtual void Enter();
	virtual void Leave();
};

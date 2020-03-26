// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Procedure/PWProcedureBase.h"
#include "PWProcedurePostLogin.generated.h"

UCLASS()
class PWGAME_API UPWProcedurePostLogin : public UPWProcedureBase
{
	GENERATED_BODY()
public:

	UPWProcedurePostLogin();

	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_PostLogin; }
	virtual void Enter();
	virtual void Leave();

	void SetNextState(ProcedureState inNextState);
	void ChangeToNextState();

	//void OnCurLanguageChanged(const FString& Language);
private:
	ProcedureState NextState = ProcedureState::ProcedureState_Lobby;

public:


	uint8 CanSkipVideo : 1;

	uint8 VideoPlayed : 1;

};

// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "Procedure/Client/PWProcedureLoginBase.h"
#include "PWProcedureLogin.generated.h"

/**
 *
 */
UCLASS()
class PWGAME_API UPWProcedureLogin : public UPWProcedureLoginBase
{
	GENERATED_BODY()

public:
	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_Login; }
	virtual void Enter();
	virtual void Leave();

	virtual void ChangeLoginStatus(ELoginStatus LoginStatus, uint32 ErrorCode);
	virtual void OnBulletinRefreshed();

	void StartRequestHttpServer();
	void StartRequestUpdateServer();
	void OnUpdateServerFinished();

	void SetAccount(const FString& inAccount) { _Account = inAccount; }
	const FString& GetAccount() { return _Account; }
private:
	FString _Account;
	FString UpdateRootAddr;
	int32 UnfinishedHttpRequest = 0;
};

// Copyright 2004-2018 Perfect World Co.,Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "System/Procedure/PWProcedureBase.h"
#include "Interfaces/IHttpRequest.h"
#include "PWProcedurePolicy.generated.h"


class FPWPolicyCheck
{
public:
	FPWPolicyCheck();
	FPWPolicyCheck(UObject* inWorldContext, int32 inFlag);
	~FPWPolicyCheck();

	void Start2GetVersion(const FString& addr);
	void Query_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	UObject* WorldContext;
	int32 Flag;
};

/**
 *
 */
UCLASS()
class PWGAME_API UPWProcedurePolicy : public UPWProcedureBase
{
	GENERATED_BODY()
public:
	virtual ProcedureState GetState() override;
	virtual void Enter() override;
	virtual void Leave() override;

	void InitHttpAddr(const FString& AgreementAddr, const FString& PrivacyAddr);
	void CheckPolicyVersion();
	void SetPolicyVersion2Server(int32 flag);
	int32 GetUserAgreementVersion() { return UserAgreementVersion; }
	int32 GetPrivacyPolicyVersion() { return PrivacyPolicyVersion; }
	UFUNCTION()
		void OnReceiveVersion(int32 flag, int32 version);

	// -- for lobby

	// Get versions from server and set to private vars.
	void SetPolicyVersionFromServer(int32 flag, int32 version);
	// -- for lobby end

	void SetNextState(ProcedureState inNextState);
	ProcedureState GetNextState();

private:
	ProcedureState NextState = ProcedureState::ProcedureState_Lobby;
	int32 UserAgreementVersion = 0;
	int32 NetUserAgreementVersion = 0;		//-1表示没有从http得到结果，其他表示已经获取过了，有可能获取失败，失败了就是0
	int32 PrivacyPolicyVersion = 0;
	int32 NetPrivacyPolicyVersion = 0;		//-1表示没有从http得到结果，其他表示已经获取过了，有可能获取失败，失败了就是0
	uint8 bCheckedUserAgreement = false;
	uint8 bCheckedPrivacyPolicy = false;

	FPWPolicyCheck* m_pAgreementCheck;
	FPWPolicyCheck* m_pPrivacyCheck;
};

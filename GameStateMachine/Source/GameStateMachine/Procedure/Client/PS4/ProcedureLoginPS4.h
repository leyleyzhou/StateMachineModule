// Copyright 2004-2019 Perfect World Co.,Ltd. All Rights Reserved.
// @author Yongquan Fan, create on 2019/4/28.

#pragma once

#include "Procedure/Client/PWProcedureLoginBase.h"
#include "OnlineError.h"
#if PLATFORM_PS4
#   include "Online/PS4/DHOnlinePS4.h"
#endif
#include "PWProcedureLoginPS4.generated.h"

/**
 *
 */
UCLASS()
class PWGAME_API UPWProcedureLoginPS4 : public UPWProcedureLoginBase
{
    GENERATED_BODY()
public:
    virtual ProcedureState GetState() { return ProcedureState::ProcedureState_Login; }
    virtual void Enter();
    virtual void Leave();
    virtual void OnPostLoadMap(UWorld* LoadedWorld);
    virtual void OnPostLoadAllMaps();

    virtual void OnBulletinRefreshed();

    virtual void ChangeLoginStatus(ELoginStatus LoginStatus, uint32 ErrorCode);
    ELoginStatus GetLoginStatus() { return _LoginStatus; }
    void AutoLogin();		//×Ô¶¯µÇÂ¼

    int32 UnfinishedHttpRequest = 0;
    void OnOneHttpRequestFinished();
    FString UpdateRootAddr;
private:
    void ShowError(EOnlineErrorCodePS4 ErrorCode);

    void OnUserLogout();

    void StartRequestHttpServe();

    void StartRequestUpdateServer();
};
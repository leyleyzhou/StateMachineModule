// Copyright 2004-2019 Perfect World Co.,Ltd. All Rights Reserved.
// @author Yongquan Fan, create on 2019/4/28.

#pragma once

#include "Procedure/PWProcedureBase.h"
#include "PWProcedureLoginBase.generated.h"


USTRUCT()
struct FBulletinMsg
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int32 ID = 0;
    UPROPERTY()
        FString Title;
    UPROPERTY()
        FString Content;
    UPROPERTY()
        uint8 IsBulletin : 1;
    UPROPERTY()
        uint8 CanSkip : 1;

    FBulletinMsg() {
        IsBulletin = false;
        CanSkip = false;
    };
};
/**
 *
 */
UCLASS()
class PWGAME_API UPWProcedureLoginBase : public UPWProcedureBase
{
    GENERATED_BODY()
public:
    virtual ProcedureState GetState() { return ProcedureState::ProcedureState_Login; }

    enum class ELoginStatus : uint32
    {
        LS_None = 0,
        LS_LoadDown = 1U << 0,			//加载地图
        LS_LoginDown = 1U << 1,			//登录平台
        LS_HttpServeDown = 1U << 2,		//获取大区、服务器列表
        LS_UpdateServerDown = 1U << 3,	//更新
        LS_BullentDown = 1 << 4,		//一种公告
        LS_AllDown = LS_LoadDown | LS_LoginDown | LS_HttpServeDown | LS_UpdateServerDown | LS_BullentDown,
        LS_HasError = 1U << 31
    };

    virtual void ChangeLoginStatus(ELoginStatus LoginStatus, uint32 ErrorCode) {};

    virtual void CloseBulletinsWidget();
    virtual void OnBulletinRefreshed() {};

    UFUNCTION()
        void PullBulletin();

    void CheckBulletinMsg();

    void OnCloseBulletinWidget();
    UPROPERTY()
        TArray<FBulletinMsg> BulletinMsges;

protected:

    void AddBulletin(FString Title, FString Content, int32 ID);

    ELoginStatus _LoginStatus = ELoginStatus::LS_None;
};
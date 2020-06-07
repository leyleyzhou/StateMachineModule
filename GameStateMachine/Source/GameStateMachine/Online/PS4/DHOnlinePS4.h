// Copyright 2004-2019 Perfect World Co.,Ltd. All Rights Reserved.
// @author Yongquan Fan, create on 2019/4/25.

#pragma once

#include "Online.h"
#include "BaseEventOnce.hpp"
#include "DHCountryPS4.h"
#include "PWGameInstance.h"
#include "OnlinePurchaseInterface.h"
#include "../DHQueuedAsyncProcessor.hpp"
#include "SMPresenceData.h"
#include "DHOnlinePS4.generated.h"

#ifndef WITH_PLAYTOGETHER
#define WITH_PLAYTOGETHER 0
#endif


struct FDHFriendInfo;

enum class EOnlineCheckResultPS4 : uint32
{
    Ok,
    SYS_NetworkConnectionUnavailable,
    SYS_RequiredSystemUpdate,
    APP_RequiredPatchAvailable,
    PSN_UserNotFound,
    PSN_PatchNotChecked,
    PSN_UserNotLoggedIn,
    PSN_NpAgeRestriction,
    PSN_AuthorizationFailed,
    PSN_TryAgainLater,
    UnknownError
};

enum class ECheckUserPrivilegePS4 : uint32
{
    Ok,                 // 验证通过
    TryAgain,           // 再次尝试
    Stop                // 验证终止
};

enum class EOnlineErrorCodePS4 : uint32
{
    None = 0,
    NetworkConnectionUnavailable,
    AuthorizationFailed,
    InMaintenance,
    VersionOrAccountNotSupported,
    JsonParseFailed,
    HttpServeHasError,
    OtherErrors
};

enum class EStoreIconPosPS4 : uint8
{
    SIP_PS4_Center = 0,
    SIP_PS4_Left,
    SIP_PS4_Right
};

struct FEntitlementPS4
{
    FString Label;
    int32 ConsumedCount = 0;
    int32 RemainingCount = 0;
};

enum class EDHAppButton : uint8
{
    Ok,
    YesNo,
    OkCancel,
    TwoButtons
};

enum class EDHAppReturnType : uint8
{
    Ok,
    Yes,
    No,
    Cancel,
    Button1,        // EDHAppButton::TwoButton 时生效
    Button2         // EDHAppButton::TwoButton 时生效
};

struct FDHAppButtons
{
    EDHAppButton Buttons = EDHAppButton::Ok;
    FString ButtonLabel1; // EDHAppButton::TwoButton 时生效
    FString ButtonLabel2; // EDHAppButton::TwoButton 时生效
};

#define STORE_PRODUCT_INVALID_PRICE -1

struct FStoreProductPS4
{
    FString Label;
    FString Name;

    bool CanBePurchased = false;

    int32 OriginalPrice = STORE_PRODUCT_INVALID_PRICE;
    FString DisplayOriginalPrice;

    int32 Price = STORE_PRODUCT_INVALID_PRICE;
    FString DisplayPrice;

    int32 PlusPrice = STORE_PRODUCT_INVALID_PRICE;
    FString DisplayPlusPrice;

    bool HasPlusPrice = false;
};

UCLASS()
class PWGAME_API UDHOnlinePS4 : public UObject, public DHGameInstance<UDHOnlinePS4>
{
    GENERATED_BODY()
public:
    UDHOnlinePS4();

    virtual ~UDHOnlinePS4();

    void Init();
    void OnInitCompleted();

    DECLARE_MULTICAST_DELEGATE_TwoParams(FCheckUserPrivilege, ECheckUserPrivilegePS4, EOnlineErrorCodePS4);
    typedef FCheckUserPrivilege::FDelegate FCheckUserPrivilegeDelegate;
    void CheckUserPrivilegeAndLogin(FCheckUserPrivilegeDelegate Function);

    bool HasLoggedIn();

    FString GetOnlineID();

    FString GetAuthToken();

    uint64 GetAccountID();

    FString GetAccountIDString();

    /*
     * 线上环境
     */
    EOnlineEnvironment::Type GetOnlineEnvironment();

    FString GetOnlineEnvironmentName()
    {
        switch (GetOnlineEnvironment()) {
        case EOnlineEnvironment::Type::Production:
            return TEXT("np");
        case EOnlineEnvironment::Type::Development:
            return TEXT("sp-int");
        case EOnlineEnvironment::Type::Certification:
            return TEXT("prod-qa");
        default:
            return TEXT("Unknown");
        }
    }

    FString GetCountryCode();

    FCountryPS4 GetCountryPS4()
    {
        return GetCountryByCodePS4(GetCountryCode());
    }

    /*
     * Plus 会员标识
     */
    bool PlusAccess();

    void SetPlusAccess(bool PlusAccess);

    DECLARE_MULTICAST_DELEGATE_TwoParams(FUnlockTrophy, const FOnlineError&, int32);
    typedef FUnlockTrophy::FDelegate FUnlockTrophyDelegate;
    void UnlockTrophy(int32 TrophyID, FUnlockTrophyDelegate Function);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FGetUnlockTrophies, const FOnlineError&, const TArray<int32>&);
    typedef FGetUnlockTrophies::FDelegate FGetUnlockTrophiesDelegate;
    void GetUnlockTrophies(FGetUnlockTrophiesDelegate Function);

    /*
     * 打开升级会员对话框
     * @return 是否是会员
     */
    bool ShowUpgradePlusDialog();

    void ShowErrorDialog(EOnlineErrorCodePS4 ErrorCode);

    void ShowLastErrorDialog();

    DECLARE_MULTICAST_DELEGATE_OneParam(FShowMessageDialog, EDHAppReturnType);
    typedef FShowMessageDialog::FDelegate FShowMessageDialogDelegate;
    static void ShowMessageDialog(const FString& Caption, const FString& Text, const FDHAppButtons& Buttons, FShowMessageDialogDelegate Function = {});

    void ShowWaitDialog(const FString& Text);

    void ShowModalMessageDialog(const FString& Text);

    void CloseDialog();

    bool DialogIsVisible();

    static void ShowErrorMessageDialog(const FString& Text, FShowMessageDialogDelegate Function = {})
    {
        ShowMessageDialog(DH_LOCTABLE_STR("GL_MSG_ERRORMSG"), Text, { EDHAppButton::Ok }, Function);
    }

    static bool IsNetErrorCode(int32 ErrorCode);

    void ClearLastErrorCode();

    DECLARE_MULTICAST_DELEGATE_OneParam(FGetEntitlements, const TArray<FEntitlementPS4>&);
    typedef FGetEntitlements::FDelegate FGetEntitlementsDelegate;
    /*
     * 显示兑换码、礼品码对话框
     */
    void ShowRedeemCodeDialog(FGetEntitlementsDelegate Function);

    /*
     * 获取 支付票据
     */
    void QueryEntitlements(FGetEntitlementsDelegate Function);

    /*
     * 获取 PSN Store Products
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FStoreQueryProducts, const TArray<FStoreProductPS4>&);
    typedef FStoreQueryProducts::FDelegate FStoreQueryProductsDelegate;
    void Store_QueryProducts(FStoreQueryProductsDelegate Function);

    /*
     * 显示 PSN Store Icon
     */
    void Store_ShowPsStoreIcon(EStoreIconPosPS4 Pos = EStoreIconPosPS4::SIP_PS4_Center);

    /*
     * 隐藏 PSN Store Icon
     */
    void Store_HidePsStoreIcon();

    void Store_ShowEmptyDialog();

    /*
     * 检查 PSN Store Icon 显示状态
     */
    bool Store_PsStoreIconIsVisible();

    DECLARE_MULTICAST_DELEGATE_TwoParams(FStore_ShowCheckoutDialog, const FOnlineError&, const FString&);
    typedef FStore_ShowCheckoutDialog::FDelegate FStore_ShowCheckoutDialogDelegate;
    void Store_ShowCheckoutDialog(const FString& ProductLabel, FStore_ShowCheckoutDialogDelegate Function);

    DHQueuedAsyncProcessor& GetQueuedAsyncProcessor();

    // 玩家账号登出事件
    FSimpleDelegate& GetOnUserLogout();

    // ----------------- Friend ------------------
    void UpdateFriendList();
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDHFriendListUpdated, const TArray<FDHFriendInfo>&);
    FOnDHFriendListUpdated& GetOnFriendListUpdated();
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnFriendPresenceUpdated, const FDHFriendInfo&);
    FOnFriendPresenceUpdated& GetOnFriendPresenceUpdated();
    //--------------Invitation-----------------
    void CreateSession(uint64 TeamID, int32 TeamSize);
    void LeaveSession(uint64 TeamID);
    void JoinSession(uint64 TeamID);	//被邀请者要先告诉lobby加入队伍，等加入lobby队伍消息返回以后再调用此接口进入PSN的队伍
    void IgnoreInvite(uint64 TeamID);	//忽略PSN的邀请消息

    void SendInvite(const TArray<FString>& InviteList, uint64 TeamID, int32 TeamSize);	//InviteList:DisplayNameList
    void SendPlayTogetherInvite(uint64 TeamID, int32 TeamSize);
    void IgnorePlayTogetherEvent();

    DECLARE_MULTICAST_DELEGATE_TwoParams(FDHOnInvivationReceived, uint64, const FString&);
    FDHOnInvivationReceived& GetInvivationReceivedDelegate();	//这个不用

    DECLARE_MULTICAST_DELEGATE_OneParam(FDHOnInvitationAccepted, uint64);		//TeamID == 0 means failed to get Session
    FDHOnInvitationAccepted& GetInvitationAcceptDelegate();

    DECLARE_MULTICAST_DELEGATE_TwoParams(FDHOnJoinSessionComplete, FName, EOnJoinSessionCompleteResult::Type);
    FDHOnJoinSessionComplete& GetJoinSessionCompleteDelegate();

    DECLARE_MULTICAST_DELEGATE(FOnDHPlayTogetherEventReceived)
    FOnDHPlayTogetherEventReceived& GetOnDHPlayTogetherEventReceived();

    bool HasPlayTogetherEvent();

    void AcceptInvite(uint64 InvitorAccountID);

    bool IsEnterCrossNotCircle();

    void ShowBrowser(const FString& Url);

    void SetPresence(const struct FPWPresenceData* PresenceData);
    void SetPresence(EGameStatus Status);
private:
    struct Impl;
    Impl* _Impl;
};
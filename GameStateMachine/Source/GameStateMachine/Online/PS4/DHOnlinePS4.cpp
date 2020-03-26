#include "DHOnlinePS4.h"
#ifdef PLATFORM_PS4
#include <error_dialog.h>
#include "np/np_error.h"
#include "DHScopeOut.h"
#include "DHJsonUtils.h"
#include "../PS4/OnlineSubsystemPS4/Source/Public/OnlineSubsystemPS4.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/OnlineSessionInterfacePS4.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/OnlineIdentityInterfacePS4.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/OnlineAchievementsInterfacePS4.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/WebApiPS4Types.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/WebApiPS4Task.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/OnlineExternalUIInterfacePS4.h"
#include "OnlineSubsystemSessionSettings.h"
//
//#include "OnlineSubsystemPS4.h"
//#include "OnlineSessionInterfacePS4.h"
//#include "OnlineIdentityInterfacePS4.h"
//#include "OnlineAchievementsInterfacePS4.h"
//#include "WebApiPS4Types.h"
//#include "WebApiPS4Task.h"
//#include "OnlineExternalUIInterfacePS4.h"

#include "Private/Json/json.hpp"
#include "UnrealTemplate.h"
#include "CoreMiscDefines.h"
#include "message_dialog.h"
#include "PS4/PS4Misc.h"
#include "system_service.h"
#include "GenericPlatformMisc.h"
#include "StringConv.h"
#include <string>
#include "PWPresenceData.h"
#include "DHFriendInfo.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/PS4Utilities.h"
#include "../PS4/OnlineSubsystemPS4/Source/Private/OnlineFriendsInterfacePS4.h"



#ifndef UE4_PROJECT_NPTITLEID
#	define UE4_PROJECT_NPTITLEID 0
#endif

#ifndef UE4_PROJECT_NPLICENSEEID
#	define UE4_PROJECT_NPLICENSEEID 0
#endif

#ifndef UE4_PROJECT_NPTITLESECRET
#	define UE4_PROJECT_NPTITLESECRET 0
#endif

#ifndef UE4_PROJECT_NPCLIENTID
#	define UE4_PROJECT_NPCLIENTID 0
#endif

#define DH_DEFINE_CASE(o)       \
    case o: if (CaseLogOnce++ == 0) {UE_LOG(LogTemp, Display, TEXT("Enter switch case, value = " #o));}

using JSON = nlohmann::json;

static void ShowErrorDialogPS4(SceUserServiceUserId SceUserID, uint32 ErrorCode);
static FString ConvertSonyStoreText(const char* Utf8StoreTextWithHtml);
static void InitNpSyncRequest(NpToolkit::Core::RequestBase* RequestBase, SceUserServiceUserId UserID, SceNpServiceLabel ServiceLabel = 0);

enum eDHPushEventType
{
	eDHPushEventType_TeamMemJoin = 0,
	eDHPushEventType_TeamMemLeave,
	eDHPushEventType_TeamMemDataUpdate,
	eDHPushEventType_Max,
};

struct UDHOnlinePS4::Impl
{
	int8 LocalUserNum = 0;

	int8 PlusAccess = 0;

	int8 PsStoreIconVisible = 0;

	SceNpAccountId AccountID = 0;

	FString AccountIDString;

	FString OnlineID;

	FString CountryCode;

	IOnlineIdentityPtr IdentitySys = nullptr;

	IOnlineAchievementsPtr TrophySys = nullptr;

	FDelegateHandle LoginCompleteHandle;

	FDelegateHandle LoginStatusHandle;

	DHQueuedAsyncProcessor QueuedAsyncProcessor;

	EOnlineErrorCodePS4 LastErrorCode = EOnlineErrorCodePS4::None;

	float CachedProductsExpireTime = 0.f;

	bool DialogIsVisible = false;

	FSimpleDelegate OnUserLogout;

	Impl()
	{
		IdentitySys = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetIdentityInterface();
		TrophySys = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetAchievementsInterface();
		SessionSys = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetSessionInterface();
	}

	~Impl()
	{
		if (IdentitySys && LoginCompleteHandle.IsValid()) {
			IdentitySys->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteHandle);
		}
	}

	void Init();
	void AddCallback();

	void OnInitCompleted();
	bool IsInited = false;

	SceNpWebApiPushEventDataType SceWebApiPushDataTypes[(int32)eDHPushEventType_Max];
	void RegistPushEvent();
	static void OnWebApiPushMessageReceived(
		int32_t userCtxId,
		int32_t callbackId,
		const char* pNpServiceName,
		SceNpServiceLabel npServiceLabel,
		const SceNpPeerAddressA* pTo,
		const SceNpOnlineId* pToOnlineId,
		const SceNpPeerAddressA* pFrom,
		const SceNpOnlineId* pFromOnlineId,
		const SceNpWebApiPushEventDataType* pDataType,
		const char* pData,
		size_t dataLen,
		const SceNpWebApiExtdPushEventExtdData* pExtdData,
		size_t extdDataNum,
		void* pUserArg);

	EOnlineCheckResultPS4 CheckUserPrivilege(SceUserServiceUserId SceUserID);

	void TestAndProcessResult(EOnlineCheckResultPS4 CheckResult, FCheckUserPrivilegeDelegate Function);

	void Login(TFunction<void(EOnlineCheckResultPS4)> Function);

	void OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NetID);

	void OnLoginComplete(int32 LocalUserNum, bool WasSuccessful, const FUniqueNetId& UserID, const FString& Error, TFunction<void(EOnlineCheckResultPS4)> Function);

	const FUniqueNetIdPS4* GetNetID()
	{
		return static_cast<const FUniqueNetIdPS4*>(IdentitySys->GetUniquePlayerId(LocalUserNum).Get());
	}

	const SceUserServiceUserId GetServiceUserID()
	{
		return GetNetID()->GetUserId();
	}

	EOnlineEnvironment::Type GetOnlineEnvironment()
	{
		return IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetOnlineEnvironment();
	}

	void WebRequest(ENpApiGroup ApiGroup, SceNpWebApiHttpMethod HttpMethod, const FString& Uri, const FString& RequestBody, TFunction<void(const FOnlineError & Error, const FString & ResponseBody)> Function);

	void Store_QueryProductsWithCachedLabels(const TArray<FString>& Labels, FStoreQueryProductsDelegate Function);

	void Store_QueryProductLabels(TFunction<void(const TArray<FString>&)> Function);

	FOnlineSubsystemPS4* GetSysPS4()
	{
		return (FOnlineSubsystemPS4*)(IOnlineSubsystem::Get(PS4_SUBSYSTEM));
	}

	//------------- Session & Invitation ----------------------------
	IOnlineSessionPtr SessionSys = nullptr;
	TArray< TSharedRef<FOnlineFriend> > FriendList;

	void UpdateFriendList();
	FOnDHFriendListUpdated OnDHFriendListUpdated;
	void OnFriendListUpdated(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	FOnFriendPresenceUpdated OnDHFriendPresenceUpdated;

	FOnDHPlayTogetherEventReceived OnDHPlayTogetherEventReceived;

	void PresenceToFriendInfo(const NpToolkit::Presence::Presence* PresencePS4, FDHFriendInfo* FriendInfo);


	TArray<TSharedRef<const FUniqueNetId>> PlayTogetherUserList;
	TArray<FString> FriendListToInvite;
	uint64 _TeamID = 0;
	void CreateSession(uint64 TeamID, int32 TeamSize);
	void LeaveSession(uint64 TeamID);
	void JoinSession(uint64 TeamID);
	void IgnoreInvite(uint64 TeamID);
	void SendInvite(const TArray<FString>& InviteList, uint64 TeamID, int32 TeamSize);
	void SendPlayTogetherInvite(uint64 TeamID, int32 TeamSize);
	void IgnorePlayTogetherEvent();
	void SendInviteWithSessionCreatedInternal();

	FName FormatSessionName(uint64 TeamID);

	FDelegateHandle CreateSessionCompleteHandle;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	FDelegateHandle OnInviteAcceptedHandle;
	void OnSystemInviteAccepted(const FString& UserId, const FString& SessionId);

	void OnGetSessionCompleted(const FUniqueNetId& UserId, TSharedPtr<FOnlineSessionSearch> SessionId, const FString& SessionInfo, bool bWasSuccessful);


	FDHOnInvivationReceived OnDHInvivationReceived;
	FDelegateHandle SessionInviteReceivedHandle;
	void OnSessionInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult);

	FDelegateHandle OnPlayTogetherEventReceivedHandle;
	void OnPlayTogetherEventReceived(int32 UserIndex, TArray<TSharedPtr<const FUniqueNetId>> UserIdList);

	FDelegateHandle OnFindSessionsCompleteHandle;
	void OnFindSessionsComplete(bool bWasSuccessful);

	FDHOnJoinSessionComplete OnDHJoinSessionComplete;
	FDelegateHandle JoinSessionCompleteHandle;
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	FDHOnInvitationAccepted DHOnInvitationAccepted;
	FDelegateHandle SessionUserInviteAcceptedHandle;
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);
	FString InvitedSessionID;
	FOnlineSessionSearchResult DHInviteResult;
	void DealWithAcceptedSessionUserInvite();

	void MakeAllInvitationUsed();

	FDelegateHandle SessionFailureHandle;
	void OnSessionFailure(const FUniqueNetId& PlayerId, ESessionFailure::Type FailureType);

	TMap<uint64, FOnlineSessionSearchResult>  UnfinishInviteList;

	void ShowBrowser(const FString& Url);

	static void CallbackStatic(NpToolkit::Core::CallbackEvent* evt);
	void GetFriendPresense(NpToolkit::Core::OnlineUser Friend);
	void OnFriendPresenceUpdated(const FDHFriendInfo& Info);
};

static void InitNpSyncRequest(NpToolkit::Core::RequestBase* RequestBase, SceUserServiceUserId UserID, SceNpServiceLabel ServiceLabel)
{
	RequestBase->userId = UserID;
	RequestBase->async = false;
	RequestBase->serviceLabel = ServiceLabel;
	memset(RequestBase->padding, 0, sizeof(RequestBase->padding));
}

void UDHOnlinePS4::Impl::OnLoginComplete(int32 LocalUserNum, bool WasSuccessful, const FUniqueNetId& UserID, const FString& Error, TFunction<void(EOnlineCheckResultPS4)> Function)
{
	int Ret = SCE_TOOLKIT_NP_V2_SUCCESS;
	EOnlineCheckResultPS4 CheckResult = WasSuccessful ? EOnlineCheckResultPS4::Ok : EOnlineCheckResultPS4::PSN_AuthorizationFailed;

	PW_LOG(LogTemp, Display, TEXT("LoginComplete: WasSuccessful=%d, Error=%s"), WasSuccessful ? 1 : 0, *Error);
	on_scope_out{
		if (LoginCompleteHandle.IsValid()) {
			IdentitySys->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteHandle);
			LoginCompleteHandle.Reset();
		}

		if (!Error.IsEmpty()) {
			FString Left, Right;
			if (Error.Split(TEXT(":"), &Left, &Right)) {
				int32 ErrorCode = FParse::HexNumber(*Right);
				switch (ErrorCode) {
					case SCE_NP_ERROR_NOT_SIGNED_UP:
						CheckResult = EOnlineCheckResultPS4::PSN_UserNotLoggedIn;
						break;
					case SCE_NP_ERROR_AGE_RESTRICTION: // 家长控制错误
						CheckResult = EOnlineCheckResultPS4::PSN_NpAgeRestriction;
						break;
				}
			}
		}
		Function(CheckResult);
	};

	return_if_true(!WasSuccessful);

	SceUserServiceUserId SceUserID = 0;

	{ // Initialize User Key Information
		auto NetID = const_cast<FUniqueNetIdPS4*>(static_cast<const FUniqueNetIdPS4*>(&UserID));
		SceUserID = NetID->GetUserId();
		auto Account = IdentitySys->GetUserAccount(UserID);
		AccountID = NetID->GetAccountId();
		OnlineID = Account->GetDisplayName({});
		AccountIDString = FString::Printf(TEXT("%llu"), AccountID);
	}

	{ // Check Plus Access
		NpToolkit::NpUtils::Request::CheckPsPlusAccess Request{};
		InitNpSyncRequest(&Request, SceUserID);
		NpToolkit::Core::Response<NpToolkit::Core::Empty> Response;
		if (NpToolkit::NpUtils::checkPsPlusAccess(Request, &Response) == SCE_TOOLKIT_NP_V2_SUCCESS) {
			PlusAccess = 1;
		}
	}

	{ // Init Trophy
		NpToolkit::Trophy::Request::RegisterTrophyPack Request{};
		InitNpSyncRequest(&Request, SceUserID);
		NpToolkit::Core::Response<NpToolkit::Core::Empty> Response;
		Ret = NpToolkit::Trophy::registerTrophyPack(Request, &Response);
		PW_LOG(LogTemp, Display, TEXT("Init Trophy %s, Ret = 0x%x"),
			(Ret == SCE_NP_TROPHY_ERROR_ALREADY_REGISTERED || Ret == SCE_TOOLKIT_NP_V2_SUCCESS) ? TEXT("Success") : TEXT("Failed"), Ret);
		return_if_true(Ret < 0);
	}

	if (LoginStatusHandle.IsValid()) {
		IdentitySys->ClearOnLoginStatusChangedDelegate_Handle(LocalUserNum, LoginStatusHandle);
		LoginStatusHandle.Reset();
	}

	LoginStatusHandle = IdentitySys->AddOnLoginStatusChangedDelegate_Handle(LocalUserNum, FOnLoginStatusChangedDelegate::CreateRaw(this, &Impl::OnLoginStatusChanged));
}

UDHOnlinePS4::UDHOnlinePS4()
	:_Impl(new Impl)
{}

UDHOnlinePS4::~UDHOnlinePS4()
{
	safe_delete(_Impl);
}

void UDHOnlinePS4::Init()
{
	_Impl->Init();
}

void UDHOnlinePS4::OnInitCompleted()
{
	_Impl->OnInitCompleted();
}


DHQueuedAsyncProcessor& UDHOnlinePS4::GetQueuedAsyncProcessor()
{
	return _Impl->QueuedAsyncProcessor;
}

EOnlineCheckResultPS4 UDHOnlinePS4::Impl::CheckUserPrivilege(SceUserServiceUserId SceUserID)
{
	int RequestID = sceNpCreateRequest();
	int32 CheckNpResult = sceNpCheckNpAvailabilityA(RequestID, SceUserID);
	sceNpDeleteRequest(RequestID);

	EOnlineCheckResultPS4 CheckResult = EOnlineCheckResultPS4::UnknownError;
	int NetState = -1;
	if (sceNetCtlGetState(&NetState) == SCE_OK) {   // 检查网络链路
		switch (NetState) {
		case 3:     // IP address obtained
			break;
		case 0:     // Disconnected
		case 1:     // Connecting (to cable or wireless device)
		case 2:     // Obtaining IP address
		default:    // 网络链路不通
			return EOnlineCheckResultPS4::SYS_NetworkConnectionUnavailable;
		}
	}

	int32 CaseLogOnce = 0;
	switch (CheckNpResult) {
		DH_DEFINE_CASE(SCE_OK)
			CheckResult = EOnlineCheckResultPS4::Ok;
		break;
		DH_DEFINE_CASE(SCE_NP_ERROR_SIGNED_OUT)
			DH_DEFINE_CASE(SCE_NP_ERROR_LOGOUT)
			DH_DEFINE_CASE(SCE_NP_ERROR_NOT_SIGNED_UP)
			CheckResult = EOnlineCheckResultPS4::PSN_UserNotLoggedIn;
		break;
		DH_DEFINE_CASE(SCE_NP_ERROR_USER_NOT_FOUND)
			CheckResult = EOnlineCheckResultPS4::PSN_UserNotFound;
		break;
		DH_DEFINE_CASE(SCE_NP_ERROR_AGE_RESTRICTION)
			CheckResult = EOnlineCheckResultPS4::PSN_NpAgeRestriction;
		break;
		DH_DEFINE_CASE(SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST)
			DH_DEFINE_CASE(SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST_FOR_TITLE)
			CheckResult = EOnlineCheckResultPS4::SYS_RequiredSystemUpdate;
		break;
		DH_DEFINE_CASE(SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST)
			DH_DEFINE_CASE(SCE_NP_ERROR_LATEST_PATCH_PKG_DOWNLOADED)
			CheckResult = EOnlineCheckResultPS4::APP_RequiredPatchAvailable;
		break;
		DH_DEFINE_CASE(SCE_NP_ERROR_PATCH_NOT_CHECKED)      // (network was unavailable) or (may be the Sony patch server is under maintenance)
			CheckResult = EOnlineCheckResultPS4::PSN_PatchNotChecked;
		break;
		DH_DEFINE_CASE(SCE_NP_ERROR_ABORTED)
			DH_DEFINE_CASE(SCE_NP_ERROR_REQUEST_MAX)
			DH_DEFINE_CASE(SCE_NP_ERROR_TIMEOUT)
			CheckResult = EOnlineCheckResultPS4::PSN_TryAgainLater;
		break;
	default:
		UE_LOG(LogTemp, Display, TEXT("Enter switch case, value = OtherErrors"));
		CheckResult = EOnlineCheckResultPS4::UnknownError;
		break;
	}

	return CheckResult;
}

void UDHOnlinePS4::Impl::TestAndProcessResult(EOnlineCheckResultPS4 CheckResult, FCheckUserPrivilegeDelegate Function)
{
	switch (CheckResult) {
	case EOnlineCheckResultPS4::Ok:
		Function.ExecuteIfBound(ECheckUserPrivilegePS4::Ok, EOnlineErrorCodePS4::None);
		return;
	case EOnlineCheckResultPS4::PSN_NpAgeRestriction:               // 阻止玩家进入游戏
		do {
			//GetSysPS4()->GetExternalUIInterface()->ShowPlatformMessageBox(*GetNetID(), EPlatformMessageType::UGCRestricted);
			ShowErrorDialogPS4(GetNetID()->GetUserId(), SCE_NP_ERROR_AGE_RESTRICTION);
		} while (true);
		return;
	case EOnlineCheckResultPS4::PSN_UserNotLoggedIn:                // 显示登陆系统窗口
		GetSysPS4()->GetExternalUIInterface()->ShowLoginUI(LocalUserNum, true, false,
			FOnLoginUIClosedDelegate::CreateLambda([Function](TSharedPtr<const FUniqueNetId> UniqueID, const int ControllerIndex, const FOnlineError& Error) {
				Function.ExecuteIfBound(ECheckUserPrivilegePS4::TryAgain, EOnlineErrorCodePS4::None);
				}));
		return;
	case EOnlineCheckResultPS4::SYS_RequiredSystemUpdate:           // 强制升级系统
		do {
			ShowErrorDialogPS4(GetNetID()->GetUserId(), SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST_FOR_TITLE);
		} while (true);
		return;
	case EOnlineCheckResultPS4::APP_RequiredPatchAvailable:         // 强制打补丁
		do {
			ShowErrorDialogPS4(GetNetID()->GetUserId(), SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST);
		} while (true);
		return;
	case EOnlineCheckResultPS4::SYS_NetworkConnectionUnavailable:   // 网络链路有问题，希望用户重试
		Function.ExecuteIfBound(ECheckUserPrivilegePS4::TryAgain, EOnlineErrorCodePS4::NetworkConnectionUnavailable);
		return;
	case EOnlineCheckResultPS4::PSN_PatchNotChecked:
	case EOnlineCheckResultPS4::PSN_TryAgainLater:
	case EOnlineCheckResultPS4::PSN_UserNotFound:
	default:                                                        // 登录或验证有问题，希望用户重试
		Function.ExecuteIfBound(ECheckUserPrivilegePS4::TryAgain, EOnlineErrorCodePS4::AuthorizationFailed);
		return;
	}
}

void UDHOnlinePS4::Impl::OnLoginStatusChanged(int32 LocalUser, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NetID)
{
	UE_LOG(LogTemp, Display, TEXT("PS4 Online User Login Status Changed, LocalUserNum=%d, OldStatus=%s, NewStatus=%s"),
		LocalUser, ELoginStatus::ToString(OldStatus), ELoginStatus::ToString(NewStatus));

	if (OldStatus == ELoginStatus::Type::LoggedIn && NewStatus != ELoginStatus::Type::LoggedIn) {
		ExecuteNextFrame(FSimpleDelegate::CreateLambda([this] {
			OnUserLogout.ExecuteIfBound();
			}));
	}
}

void UDHOnlinePS4::Impl::Login(TFunction<void(EOnlineCheckResultPS4)> Function)
{
	if (LoginCompleteHandle.IsValid()) {
		IdentitySys->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteHandle);
		LoginCompleteHandle.Reset();
	}

	LoginCompleteHandle = IdentitySys->AddOnLoginCompleteDelegate_Handle(LocalUserNum, FOnLoginCompleteDelegate::CreateRaw(this, &Impl::OnLoginComplete, Function));

	IdentitySys->Login(LocalUserNum, {});
}

void UDHOnlinePS4::CheckUserPrivilegeAndLogin(FCheckUserPrivilegeDelegate Function)
{
	EOnlineCheckResultPS4 CheckResult = _Impl->CheckUserPrivilege(_Impl->GetServiceUserID());
	if (CheckResult != EOnlineCheckResultPS4::Ok) {
		_Impl->TestAndProcessResult(CheckResult, Function);
		return;
	}

	_Impl->Login([this, Function](EOnlineCheckResultPS4 CheckResult) {
		_Impl->TestAndProcessResult(CheckResult, Function);
		});
}

bool UDHOnlinePS4::HasLoggedIn()
{
	return _Impl->IdentitySys->GetLoginStatus(_Impl->LocalUserNum) == ELoginStatus::LoggedIn;
}

FString UDHOnlinePS4::GetOnlineID()
{
	return _Impl->OnlineID;
}

FString UDHOnlinePS4::GetAuthToken()
{
	NpToolkit::Auth::Request::GetAuthCode Request{};
	InitNpSyncRequest(&Request, _Impl->GetServiceUserID());
	strncpy(Request.clientId.id, PREPROCESSOR_TO_STRING(UE4_PROJECT_NPCLIENTID), SCE_NP_CLIENT_ID_MAX_LEN);
	strncpy(Request.scope, "psn:s2s", Request.MAX_SIZE_SCOPE);

	NpToolkit::Core::Response<NpToolkit::Auth::AuthCode> Response;

	int Ret = NpToolkit::Auth::getAuthCode(Request, &Response);
	FString AuthToken;

	on_scope_out{
		PW_LOG(LogTemp, Display, TEXT("GetAuthToken Ret = 0x%x, Token=%s"), Ret, *AuthToken);
	};
	if (Ret < 0 || !Response.get()) {
		return AuthToken;
	}

	AuthToken = ANSI_TO_TCHAR(Response.get()->authCode.code);
	return AuthToken;
}

uint64 UDHOnlinePS4::GetAccountID()
{
	return _Impl->AccountID;
}

FString UDHOnlinePS4::GetAccountIDString()
{
	return _Impl->AccountIDString;
}

EOnlineEnvironment::Type UDHOnlinePS4::GetOnlineEnvironment()
{
	return _Impl->GetOnlineEnvironment();
}

FString UDHOnlinePS4::GetCountryCode()
{
	if (!_Impl->CountryCode.IsEmpty()) {
		return _Impl->CountryCode;
	}

	SceNpCountryCode CountryCode;

	if (sceNpGetAccountCountryA(_Impl->GetServiceUserID(), &CountryCode) == SCE_OK) {
		_Impl->CountryCode = ANSI_TO_TCHAR(CountryCode.data);
	}

	return _Impl->CountryCode;
}

bool UDHOnlinePS4::PlusAccess()
{
	return _Impl->PlusAccess != 0;
}

void UDHOnlinePS4::SetPlusAccess(bool PlusAccess)
{
#if IS_DEVELOPMENT_MODE
	_Impl->PlusAccess = PlusAccess ? 1 : 0;
#endif
}

bool UDHOnlinePS4::ShowUpgradePlusDialog()
{
	TSharedPtr<const FUniqueNetId> UniqueNetID = _Impl->IdentitySys->GetUniquePlayerId(_Impl->LocalUserNum);
	auto Sys = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetExternalUIInterface();

	bool Result = Sys->ShowAccountUpgradeUI(*UniqueNetID.Get());
	_Impl->PlusAccess = Result ? 1 : 0;
	return Result;
}

void UDHOnlinePS4::ShowErrorDialog(EOnlineErrorCodePS4 ErrorCode)
{
	_Impl->LastErrorCode = ErrorCode;
	return_if_true(ErrorCode == EOnlineErrorCodePS4::None);

	ShowErrorDialogPS4(_Impl->GetServiceUserID(), (uint32)ErrorCode);
}

void UDHOnlinePS4::ShowLastErrorDialog()
{
	ShowErrorDialog(_Impl->LastErrorCode);
}

void UDHOnlinePS4::ClearLastErrorCode()
{
	_Impl->LastErrorCode = EOnlineErrorCodePS4::None;
}

static FCriticalSection GMessageBoxCS;
struct FScopedMsgDialogLibrary
{
	bool bInitialized;
	bool bWithTerminate;

	FScopedMsgDialogLibrary(bool WithTerminate = true)
		: bInitialized(false), bWithTerminate(WithTerminate)
	{
		int Result = sceSysmoduleLoadModule(SCE_SYSMODULE_MESSAGE_DIALOG);
		if (Result != SCE_OK) {
			UE_LOG(LogPS4, Warning, TEXT("sceSysmoduleLoadModule(SCE_SYSMODULE_MESSAGE_DIALOG) failed (result 0x%08x)."), Result);
			return;
		}

		Result = sceCommonDialogInitialize();
		if (Result != SCE_OK && Result != SCE_COMMON_DIALOG_ERROR_ALREADY_SYSTEM_INITIALIZED) {
			UE_LOG(LogPS4, Warning, TEXT("sceCommonDialogInitialize() failed (result 0x%08x)."), Result);
			return;
		}

		Result = sceMsgDialogInitialize();
		if (Result != SCE_OK) {
			UE_LOG(LogPS4, Warning, TEXT("sceMsgDialogInitialize() failed (result 0x%08x)."), Result);
			return;
		}

		bInitialized = true;
	}

	~FScopedMsgDialogLibrary()
	{
		if (bInitialized && bWithTerminate) {
			sceMsgDialogTerminate();
		}
	}
};

void UDHOnlinePS4::ShowMessageDialog(const FString& Caption, const FString& Text, const FDHAppButtons& ButtonsPS4, FShowMessageDialogDelegate Function)
{
	FScopeLock Lock(&GMessageBoxCS);

	sceSystemServiceHideSplashScreen();

	SceMsgDialogButtonsParam ButtonsParam = {};
	SceMsgDialogUserMessageParam UserMsgParam = {};

	switch (ButtonsPS4.Buttons) {
	case EDHAppButton::Ok:
		UserMsgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_OK;
		break;
	case EDHAppButton::OkCancel:
		UserMsgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_OK_CANCEL;
		break;
	case EDHAppButton::YesNo:
		UserMsgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_YESNO;
		break;
	case EDHAppButton::TwoButtons:
		UserMsgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_2BUTTONS;
		UserMsgParam.buttonsParam = &ButtonsParam;
		if (!ButtonsPS4.ButtonLabel1.IsEmpty()) {
			ButtonsParam.msg1 = TCHAR_TO_UTF8(*ButtonsPS4.ButtonLabel1);
		}
		if (!ButtonsPS4.ButtonLabel2.IsEmpty()) {
			ButtonsParam.msg2 = TCHAR_TO_UTF8(*ButtonsPS4.ButtonLabel2);
		}
		break;
	}

	EDHAppReturnType ReturnValue = EDHAppReturnType::Ok;
	on_scope_out{
		ExecuteNextFrame(FSimpleDelegate::CreateLambda([Function, ReturnValue] {
			Function.ExecuteIfBound(ReturnValue);
		}));
	};

	FScopedMsgDialogLibrary MsgLibraryScope;

	if (!MsgLibraryScope.bInitialized) {
		// Fallback to generic platform behavior.
		UE_LOG(LogPS4, Warning, TEXT("Failed to initialize MsgDialog library."));
		return;
	}

	FString Context = Caption.IsEmpty() ? Text : FString::Printf(TEXT("%s : %s"), *Caption, *Text);
	std::string Msg = TCHAR_TO_UTF8(*Context);
	UserMsgParam.msg = Msg.c_str();
	UE_LOG(LogPS4, Display, TEXT("ShowMessageDialog: %s"), *Context);

	SceMsgDialogParam Param;
	sceMsgDialogParamInitialize(&Param);
	Param.mode = SCE_MSG_DIALOG_MODE_USER_MSG;
	Param.userMsgParam = &UserMsgParam;

	int Result = sceMsgDialogOpen(&Param);
	if (Result < 0) {
		UE_LOG(LogPS4, Warning, TEXT("sceMsgDialogOpen failed (result 0x%08x)."), Result);
		return;
	}

	while (sceMsgDialogUpdateStatus() == SCE_COMMON_DIALOG_STATUS_RUNNING) {
		FPlatformProcess::Sleep(0.01f);
	}

	SceMsgDialogResult DialogResult = {};
	Result = sceMsgDialogGetResult(&DialogResult);
	if (Result < 0) {
		UE_LOG(LogPS4, Warning, TEXT("sceMsgDialogGetResult failed (result 0x%08x)."), Result);
		return;
	}

	switch (ButtonsPS4.Buttons) {
	case EDHAppButton::Ok:
		ReturnValue = EDHAppReturnType::Ok;
		break;
	case EDHAppButton::YesNo:
		ReturnValue = (DialogResult.result == SCE_COMMON_DIALOG_RESULT_USER_CANCELED ||
			DialogResult.buttonId == SCE_MSG_DIALOG_BUTTON_ID_NO) ? EDHAppReturnType::No : EDHAppReturnType::Yes;
		break;
	case EDHAppButton::OkCancel:
		ReturnValue = DialogResult.result == SCE_COMMON_DIALOG_RESULT_USER_CANCELED ? EDHAppReturnType::Cancel : EDHAppReturnType::Ok;
		break;
	case EDHAppButton::TwoButtons:
		ReturnValue = DialogResult.result == SCE_MSG_DIALOG_BUTTON_ID_BUTTON1 ? EDHAppReturnType::Button1 : EDHAppReturnType::Button2;
		break;
	default:
		break;
	}
}

bool UDHOnlinePS4::DialogIsVisible()
{
	return _Impl->DialogIsVisible;
}

void UDHOnlinePS4::ShowWaitDialog(const FString& Text)
{
	FScopeLock Lock(&GMessageBoxCS);
	_Impl->DialogIsVisible = true;

	sceSystemServiceHideSplashScreen();

	SceMsgDialogButtonsParam ButtonsParam = {};
	SceMsgDialogUserMessageParam UserMsgParam = {};
	UserMsgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_WAIT;

	FScopedMsgDialogLibrary MsgLibraryScope(false);

	if (!MsgLibraryScope.bInitialized) {
		// Fallback to generic platform behavior.
		UE_LOG(LogPS4, Warning, TEXT("Failed to initialize MsgDialog library."));
		return;
	}

	std::string Msg = TCHAR_TO_UTF8(*Text);
	UserMsgParam.msg = Msg.c_str();
	UE_LOG(LogPS4, Display, TEXT("ShowWaitDialog: %s"), *Text);

	SceMsgDialogParam Param;
	sceMsgDialogParamInitialize(&Param);
	Param.mode = SCE_MSG_DIALOG_MODE_USER_MSG;
	Param.userMsgParam = &UserMsgParam;

	int Result = sceMsgDialogOpen(&Param);
	if (Result < 0) {
		UE_LOG(LogPS4, Warning, TEXT("sceMsgDialogOpen failed (result 0x%08x)."), Result);
		return;
	}
}

void UDHOnlinePS4::ShowModalMessageDialog(const FString& Text)
{
	FScopeLock Lock(&GMessageBoxCS);
	_Impl->DialogIsVisible = true;

	sceSystemServiceHideSplashScreen();

	SceMsgDialogButtonsParam ButtonsParam = {};
	SceMsgDialogUserMessageParam UserMsgParam = {};
	UserMsgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_NONE;

	FScopedMsgDialogLibrary MsgLibraryScope(false);

	if (!MsgLibraryScope.bInitialized) {
		// Fallback to generic platform behavior.
		UE_LOG(LogPS4, Warning, TEXT("Failed to initialize MsgDialog library."));
		return;
	}

	std::string Msg = TCHAR_TO_UTF8(*Text);
	UserMsgParam.msg = Msg.c_str();
	UE_LOG(LogPS4, Display, TEXT("ShowWaitDialog: %s"), *Text);

	SceMsgDialogParam Param;
	sceMsgDialogParamInitialize(&Param);
	Param.mode = SCE_MSG_DIALOG_MODE_USER_MSG;
	Param.userMsgParam = &UserMsgParam;

	int Result = sceMsgDialogOpen(&Param);
	if (Result < 0) {
		UE_LOG(LogPS4, Warning, TEXT("sceMsgDialogOpen failed (result 0x%08x)."), Result);
		return;
	}
}

void UDHOnlinePS4::CloseDialog()
{
	return_if_true(!_Impl->DialogIsVisible);
	_Impl->DialogIsVisible = false;

	sceMsgDialogClose();
	sceMsgDialogTerminate();
}

static void ShowErrorDialogPS4(SceUserServiceUserId SceUserID, uint32 ErrorCode)
{
	int Ret = 0;
	if ((Ret = sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG)) != SCE_OK || (Ret = sceErrorDialogInitialize()) != SCE_OK) {
		UE_LOG(LogTemp, Error, TEXT("sceSysmoduleLoadModule or sceErrorDialogInitialize failed, Ret = 0x%08x"), Ret);
		return;
	}

	SceErrorDialogParam Params;
	sceErrorDialogParamInitialize(&Params);
	Params.size = sizeof(SceErrorDialogParam);
	Params.errorCode = ErrorCode;
	Params.userId = SceUserID;

	if ((Ret = sceErrorDialogOpen(&Params)) != SCE_OK) {
		UE_LOG(LogTemp, Error, TEXT("sceSysmoduleLoadModule or sceErrorDialogInitialize failed, Ret = 0x%08x"), Ret);
		return;
	}

	while (sceErrorDialogUpdateStatus() != SCE_ERROR_DIALOG_STATUS_FINISHED);

	sceErrorDialogTerminate();
}

void UDHOnlinePS4::UnlockTrophy(int32 TrophyID, FUnlockTrophyDelegate Function)
{
	auto NetID = _Impl->GetNetID();
	if (!NetID) {
		FOnlineError OnlineError((int32)SCE_TOOLKIT_NP_V2_ERROR_INVALID_USER);
		Function.ExecuteIfBound(OnlineError, TrophyID);
		return;
	}

	_Impl->QueuedAsyncProcessor.Start([UserID = _Impl->GetServiceUserID(), TrophyID, Function](DHQueuedAsyncProcessor::Feature* Feature) {
		NpToolkit::Trophy::Request::Unlock Request{};
		InitNpSyncRequest(&Request, UserID);
		Request.trophyId = TrophyID;

		NpToolkit::Core::Response<NpToolkit::Core::Empty> Response;

		FOnlineError OnlineError(true);
		on_scope_out{
			Feature->Complete([Function, OnlineError, TrophyID] {
				UE_LOG(LogTemp, Display, TEXT("UnlockTrophy: TrophyID = %d, Ret=%x"), TrophyID, OnlineError.NumericErrorCode);
				Function.ExecuteIfBound(OnlineError, TrophyID);
			});
		};

		int Ret = NpToolkit::Trophy::unlock(Request, &Response);

		if (Ret < 0) {
			OnlineError.SetFromErrorCode(Ret);
		}
	});
}

void UDHOnlinePS4::GetUnlockTrophies(FGetUnlockTrophiesDelegate Function)
{
	auto NetID = _Impl->GetNetID();
	if (!NetID) {
		FOnlineError OnlineError((int32)SCE_TOOLKIT_NP_V2_ERROR_INVALID_USER);
		UE_LOG(LogTemp, Error, TEXT("UDHOnlinePS4::GetUnlockTrophies Failed NetID error"));
		Function.ExecuteIfBound(OnlineError, {});
		return;
	}

	_Impl->QueuedAsyncProcessor.Start([UserID = _Impl->GetServiceUserID(), Function](DHQueuedAsyncProcessor::Feature* Feature) {
		NpToolkit::Trophy::Request::GetUnlockedTrophies req;
		InitNpSyncRequest(&req, UserID);
		NpToolkit::Core::Response<NpToolkit::Trophy::UnlockedTrophies> res;
		TArray<int32> Trophies;
		FOnlineError OnlineError(true);
		int32 ret = NpToolkit::Trophy::getUnlockedTrophies(req, &res);
		if (ret == SCE_TOOLKIT_NP_V2_SUCCESS)
		{
			for (int i = 0; i < res.get()->numTrophiesIds; i++)
			{
				Trophies.Add(res.get()->trophiesIds[i]);
			}
		}
		else
		{
			OnlineError.SetFromErrorCode(ret);
		}
		UE_LOG(LogTemp, Display, TEXT("UDHOnlinePS4::GetUnlockTrophies Completed, Ret = %x Trophies.Num = %d"), ret, Trophies.Num());
		Feature->Complete([Function, OnlineError, Trophies]
			{
				Function.ExecuteIfBound(OnlineError, Trophies);
			});
	});
}

void UDHOnlinePS4::ShowRedeemCodeDialog(FGetEntitlementsDelegate Function)
{
	auto NetID = _Impl->GetNetID();
	if (!NetID) {
		Function.ExecuteIfBound({});
		return;
	}

	auto Sys = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetPurchaseInterface();
	Sys->RedeemCode(*NetID, {}, FOnPurchaseRedeemCodeComplete::CreateLambda([Function, this](const FOnlineError& Error, const TSharedRef<FPurchaseReceipt>&) {
		switch (Error.NumericErrorCode) {
		case SCE_TOOLKIT_NP_V2_DIALOG_RESULT_OK:
		case SCE_TOOLKIT_NP_V2_DIALOG_RESULT_USER_PURCHASED:
			break;
		default:
			Function.ExecuteIfBound({});
			return;
		}

		QueryEntitlements(Function);
		}));
}

void UDHOnlinePS4::QueryEntitlements(FGetEntitlementsDelegate Function)
{
	_Impl->QueuedAsyncProcessor.Start([UserID = _Impl->GetServiceUserID(), Function](DHQueuedAsyncProcessor::Feature* Feature) {
		NpToolkit::Commerce::Request::GetServiceEntitlements Request{};
		InitNpSyncRequest(&Request, UserID);
		Request.offset = 0;
		NpToolkit::Core::Response<NpToolkit::Commerce::ServiceEntitlements> Response;

		TArray<FEntitlementPS4> Data;
		on_scope_out{
			UE_LOG(LogTemp, Display, TEXT("GetServiceEntitlements: Success, Entitlements[Size=%d]"), Data.Num());
			Feature->Complete([Function, Data = Forward<decltype(Data)>(Data)]{
				Function.ExecuteIfBound(Data);
			});
		};

		int Ret = NpToolkit::Commerce::getServiceEntitlements(Request, &Response);
		if (Ret < 0) {
			UE_LOG(LogTemp, Display, TEXT("GetServiceEntitlements: Failed, Ret=%x"), Ret);
			return;
		}

		const NpToolkit::Commerce::ServiceEntitlements* Entitlements = Response.get();
		return_if_true(Entitlements == nullptr || Entitlements->numEntitlements <= 0);

		for (int32 i = 0; i < Entitlements->numEntitlements; ++i) {
			auto Entitlement = &Entitlements->entitlements[i];
			if (!Entitlement->isActive) {
				continue;
			}

			FEntitlementPS4 EntitlementPS4;
			EntitlementPS4.Label = ANSI_TO_TCHAR(Entitlement->entitlementLabel.value);
			EntitlementPS4.RemainingCount = Entitlement->remainingCount;
			EntitlementPS4.ConsumedCount = Entitlement->consumedCount;

			Data.Add(EntitlementPS4);
		}
	});
}

void UDHOnlinePS4::Impl::WebRequest(ENpApiGroup ApiGroup, SceNpWebApiHttpMethod HttpMethod, const FString& Uri, const FString& RequestBody, TFunction<void(const FOnlineError & Error, const FString & ResponseBody)> Function)
{
	UE_LOG(LogTemp, Display, TEXT("WebRequest(), Uri=%s"), *Uri);
	std::printf("%s %d\n", __FUNCTION__, __LINE__);
	QueuedAsyncProcessor.Start([this, ApiGroup, HttpMethod, Uri, RequestBody, Function](DHQueuedAsyncProcessor::Feature* Feature) {
		std::printf("%s %d\n", __FUNCTION__, __LINE__);
		FWebApiPS4Task Web(GetSysPS4()->GetUserWebApiContext(*this->GetNetID()));
		Web.SetRequest(ApiGroup, Uri, HttpMethod);
		if (!RequestBody.IsEmpty()) {
			Web.SetRequestBody(RequestBody);
		}
		Web.SetTimeout(10);
		Web.DoWork();
		UE_LOG(LogTemp, Display, TEXT("WebRequest() Completed"));
		Feature->Complete([Function, Error = Web.GetErrorResult(), ResponseBody = Web.GetResponseBody()]{
			UE_LOG(LogTemp, Display, TEXT("WebRequest(), Error=%s, ResponseBody=%s"), *Error.ErrorRaw, *ResponseBody);
			Function(Error, ResponseBody);
			});
		});
}

static bool JsonHasFields(JSON& Json, const std::initializer_list<std::string>& Fields)
{
	for (auto& Field : Fields) {
		return_false_if_false(Json.contains(Field));
	}

	return true;
}

#define JSON_STRING_TO_TCHAR(Json, Field) UTF8_TO_TCHAR(Json[Field].get<std::string>().data())

void UDHOnlinePS4::Store_QueryProducts(FStoreQueryProductsDelegate Function)
{
	ShowWaitDialog({});
	FStoreQueryProductsDelegate Delegate = FStoreQueryProductsDelegate::CreateLambda([this, Function](const TArray<FStoreProductPS4>& Products) {
		CloseDialog();
		Function.ExecuteIfBound(Products);
		});

	_Impl->Store_QueryProductLabels([this, Delegate](const TArray<FString>& Labels) {
		_Impl->Store_QueryProductsWithCachedLabels(Labels, Delegate);
		});
}

void UDHOnlinePS4::Impl::Store_QueryProductsWithCachedLabels(const TArray<FString>& Labels, FStoreQueryProductsDelegate Function)
{

	std::printf("%s %d\n", __FUNCTION__, __LINE__);
	if (Labels.Num() == 0) {
		std::printf("%s %d\n", __FUNCTION__, __LINE__);
		Function.ExecuteIfBound({});
		return;
	}
	std::printf("%s %d\n", __FUNCTION__, __LINE__);

	FString Uri = TEXT("/v1/users/me/container/");
	for (int32 i = 0; i < Labels.Num(); ++i) {
		if (i != 0) {
			Uri.AppendChar(':');
		}
		Uri.Append(Labels[i]);
	}
	Uri.Append(TEXT("?flag=discounts"));

	WebRequest(ENpApiGroup::Commerce, SceNpWebApiHttpMethod::SCE_NP_WEBAPI_HTTP_METHOD_GET, Uri, {}, [this, Function](const FOnlineError& Error, const FString& ResponseBody) {
		std::printf("%s %d\n", __FUNCTION__, __LINE__);
		TArray<FStoreProductPS4> Products;
		on_scope_out{
			CachedProductsExpireTime = FApp::GetCurrentTime() + 600;
			Function.ExecuteIfBound(Products);
		};

		UE_LOG(LogTemp, Display, TEXT("Store_QueryProducts, Error=%s, ResponseBody=%s"), *Error.ErrorRaw, *ResponseBody);
		return_if_true(!Error.WasSuccessful());

		JSON Json = JSON::parse(TCHAR_TO_UTF8(*ResponseBody), nullptr, false);

		if (Json.is_discarded() || !Json.is_array()) {
			UE_LOG(LogTemp, Display, TEXT("Store_QueryProducts, ParseJson Failed, JSON=%s"), *ResponseBody);
			return;
		}

		for (int32 i = 0; i < Json.size(); ++i) {
			JSON& Arr = Json.at(i);
			continue_if_true(!JsonHasFields(Arr, { "label", "name", "container_type", "content_type", "skus" }));

			continue_if_true(Arr["container_type"] != "product" || Arr["skus"].size() == 0);

			FStoreProductPS4 Product{};
			Product.Label = JSON_STRING_TO_TCHAR(Arr, "label");
			Product.Name = JSON_STRING_TO_TCHAR(Arr, "name");
			JSON& SKU = Arr["skus"].at(0);

			continue_if_true(!JsonHasFields(SKU, { "annotation", "price", "display_price" ,"is_purchaseable", "sku_type" }));
			continue_if_true(SKU["sku_type"] != 0);

			Product.CanBePurchased = SKU["is_purchaseable"].get<bool>() && SKU["annotation"].get<int64>() != 0x80000000;
			Product.Price = SKU["price"].get<int32>();
			Product.DisplayPrice = JSON_STRING_TO_TCHAR(SKU, "display_price");

			if (SKU.contains("original_price") && SKU.contains("display_original_price")) {
				Product.OriginalPrice = SKU["original_price"].get<int32>();
				Product.DisplayOriginalPrice = JSON_STRING_TO_TCHAR(SKU, "display_price");
			}
			else {
				Product.OriginalPrice = Product.Price;
				Product.DisplayOriginalPrice = Product.DisplayPrice;
			}

			if (SKU.contains("is_plus_price") && SKU["is_plus_price"].get<bool>() && SKU.contains("plus_upsell_price") && SKU.contains("display_plus_upsell_price")) {
				Product.HasPlusPrice = true;
				Product.PlusPrice = SKU["plus_upsell_price"].get<int32>();
				Product.DisplayPlusPrice = JSON_STRING_TO_TCHAR(SKU, "display_plus_upsell_price");
			}
			else {
				Product.HasPlusPrice = false;
				Product.PlusPrice = Product.Price;
				Product.DisplayPlusPrice = Product.DisplayPrice;
			}

			Products.Add(Product);
		}
		});
}

void UDHOnlinePS4::Impl::Store_QueryProductLabels(TFunction<void(const TArray<FString>&)> Function)
{
	std::printf("%s %d\n", __FUNCTION__, __LINE__);
	QueuedAsyncProcessor.Start([UserID = GetServiceUserID(), Function](DHQueuedAsyncProcessor::Feature* Feature) {
		std::printf("%s %d\n", __FUNCTION__, __LINE__);
		NpToolkit::Commerce::Request::GetProducts Request{};
		InitNpSyncRequest(&Request, UserID);

		NpToolkit::Core::Response<NpToolkit::Commerce::Products> Response;

		TArray<FString> Labels;
		on_scope_out{
			UE_LOG(LogTemp, Display, TEXT("QueryProductLabels: Labels[Size=%d]"), Labels.Num());
			Feature->Complete([Function, Labels = Forward<decltype(Labels)>(Labels)]{
				Function(Labels);
			});
		};

		std::printf("%s %d\n", __FUNCTION__, __LINE__);
		int Ret = NpToolkit::Commerce::getProducts(Request, &Response);
		if (Ret < 0) {
			UE_LOG(LogTemp, Display, TEXT("GetProducts: Failed, Ret=%x"), Ret);
			return;
		}

		auto Products = Response.get();
		for (int32 i = 0; i < Products->numProducts; ++i) {
			Labels.Add(ANSI_TO_TCHAR(Products->products[i].productLabel.value));
		}
	});
}

void UDHOnlinePS4::Impl::Init()
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::Init exec"));
	IOnlineSessionPtr SessionPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetSessionInterface();
	if (CreateSessionCompleteHandle.IsValid() == false)
	{
		CreateSessionCompleteHandle = SessionPtr->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateRaw(this, &Impl::OnCreateSessionComplete));
	}
	if (SessionInviteReceivedHandle.IsValid() == false)
	{
		SessionInviteReceivedHandle = SessionPtr->AddOnSessionInviteReceivedDelegate_Handle(FOnSessionInviteReceivedDelegate::CreateRaw(this, &Impl::OnSessionInviteReceived));
	}
	if (OnFindSessionsCompleteHandle.IsValid() == false)
	{
		OnFindSessionsCompleteHandle = SessionPtr->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateRaw(this, &Impl::OnFindSessionsComplete));
	}
	if (JoinSessionCompleteHandle.IsValid() == false)
	{
		JoinSessionCompleteHandle = SessionPtr->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateRaw(this, &Impl::OnJoinSessionComplete));
	}
	if (SessionUserInviteAcceptedHandle.IsValid() == false)
	{
		SessionUserInviteAcceptedHandle = SessionPtr->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateRaw(this, &Impl::OnSessionUserInviteAccepted));
	}
	if (SessionFailureHandle.IsValid() == false)
	{
		SessionFailureHandle = SessionPtr->AddOnSessionFailureDelegate_Handle(FOnSessionFailureDelegate::CreateRaw(this, &Impl::OnSessionFailure));
	}

	if (OnInviteAcceptedHandle.IsValid() == false)
	{
		OnInviteAcceptedHandle = FCoreDelegates::OnInviteAccepted.AddRaw(this, &Impl::OnSystemInviteAccepted);
	}
#if WITH_PLAYTOGETHER

	if (OnPlayTogetherEventReceivedHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::Init Start listen Play Together event"));
		OnPlayTogetherEventReceivedHandle = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->AddOnPlayTogetherEventReceivedDelegate_Handle(FOnPlayTogetherEventReceivedDelegate::CreateRaw(this, &Impl::OnPlayTogetherEventReceived));
	}
#endif
	RegistPushEvent();
	//AddCallback();
}

void  UDHOnlinePS4::Impl::PresenceToFriendInfo(const NpToolkit::Presence::Presence* PresencePS4, FDHFriendInfo* FriendInfo)
{
	FriendInfo->AccountID = PresencePS4->user.accountId;
	FriendInfo->Name = PS4OnlineIdToString(PresencePS4->user.onlineId);
	UE_LOG(LogTemp, Log, TEXT("==== UDHOnlinePS4::Impl::PresenceToFriendInfo Friend = (%s, %llu)"), *FriendInfo->Name, FriendInfo->AccountID);
	UE_LOG(LogTemp, Log, TEXT("==== numPlatforms = %d"), PresencePS4->numPlatforms);
	//gamestatus
	if (PresencePS4->numPlatforms > 0)
	{
		const FString OurTitleId = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetAppId();
		const bool bIsSameTitleId = OurTitleId.Equals(ANSI_TO_TCHAR(PresencePS4->platforms[0].npTitleId.id));
		if (bIsSameTitleId == true)
		{
			memcpy(&FriendInfo->Status, PresencePS4->platforms[0].binaryGameData, PresencePS4->platforms[0].binaryGameDataSize);
			if (FriendInfo->Status <= EGameStatus::EGameStatus_None || FriendInfo->Status >= EGameStatus::EGameStatus_Max)
			{
				FriendInfo->Status = EGameStatus::EGameStatus_Online;
			}
			return;
		}
	}
	if (PresencePS4->psnOnlineStatus == NpToolkit::Presence::OnlineStatus::online)
	{
		FriendInfo->Status = EGameStatus::EGameStatus_Online;
		return;
	}

	FriendInfo->Status = EGameStatus::EGameStatus_Offline;
	return;
}

void UDHOnlinePS4::Impl::AddCallback()
{
	NpToolkit::Core::Request::AddCallback Request;
	//InitNpSyncRequest(&Request, _Impl->GetServiceUserID());
	Request.callback.callback = &CallbackStatic;
	Request.callback.appData = (void*)(this);
	int32 ret = NpToolkit::Core::addCallback(Request);
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::RegistFriendPresenceCallback ret = %x"), ret);
}

void UDHOnlinePS4::Impl::OnInitCompleted()
{
	IsInited = true;
	DealWithAcceptedSessionUserInvite();
}

void UDHOnlinePS4::Impl::UpdateFriendList()
{
	IOnlineFriendsPtr FriendPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetFriendsInterface();
	//FOnReadFriendsListComplete OnReadFriendsCompleteDelegate = FOnReadFriendsListComplete::CreateRaw(this, &FOnlineAsyncTaskPS4ReadLeaderboardsForFriends::OnReadFriendsComplete);
	FriendPtr->ReadFriendsList(LocalUserNum, EFriendsLists::ToString(EFriendsLists::Default), FOnReadFriendsListComplete::CreateRaw(this, &Impl::OnFriendListUpdated));
}

static EGameStatus GetGameStatusFromOnlineFriendData(const FOnlineFriend* FriendData)
{
	if (FriendData->GetPresence().bIsPlayingThisGame)
	{
		FString Status = FriendData->GetPresence().Status.StatusStr;
		if (Status.IsEmpty())
			return EGameStatus::EGameStatus_Online;

		const TArray<const FPWPresenceData*> ConfigList = PWPresenceDataTable::GetInstance()->GetRows();
		for (const FPWPresenceData* Config : ConfigList)
		{
			if (Config->DefaultDisplay == Status)
			{
				return Config->Status;
			}
			for (const FPWGameStatus& Tmp : Config->Display)
			{
				if (Tmp.GameStatus == Status)
					return Config->Status;
			}
		}
	}
	if (FriendData->GetPresence().bIsOnline)
		return EGameStatus::EGameStatus_Online;

	return EGameStatus::EGameStatus_Offline;
}

void UDHOnlinePS4::Impl::OnFriendListUpdated(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnFriendListUpdated bWasSuccessful = %s ListName = %s ErrorStr = %s"), bWasSuccessful ? TEXT("true") : TEXT("false"), *ListName, *ErrorStr);
	if (bWasSuccessful)
	{
		IOnlineFriendsPtr FriendPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetFriendsInterface();
		FriendPtr->GetFriendsList(LocalUserNum, ListName, FriendList);
		TArray<FDHFriendInfo> FriendInfoList;
		for (const TSharedRef<FOnlineFriend>& FriendData : FriendList)
		{
			FDHFriendInfo& FriendInfo = FriendInfoList.AddDefaulted_GetRef();
			FriendInfo.Name = FriendData->GetDisplayName();
			FriendInfo.Status = GetGameStatusFromOnlineFriendData(&FriendData.Get());
		}
		if (OnDHFriendListUpdated.IsBound())
		{
			OnDHFriendListUpdated.Broadcast(FriendInfoList);
		}
	}
}

void UDHOnlinePS4::Impl::OnSessionInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult)
{
	if (InviteResult.PingInMs == MAX_QUERY_PING)
		return;

	{
		auto NetID = const_cast<FUniqueNetIdPS4*>(static_cast<const FUniqueNetIdPS4*>(&UserId));
		auto Account = IdentitySys->GetUserAccount(UserId);
		SceNpAccountId TmpAccountID = NetID->GetAccountId();
		if (TmpAccountID != AccountID)
			return;
	}

	auto NetID = const_cast<FUniqueNetIdPS4*>(static_cast<const FUniqueNetIdPS4*>(&FromId));
	auto Account = IdentitySys->GetUserAccount(FromId);
	SceNpAccountId InvitorAccountID = NetID->GetAccountId();
	FString InvitorOnlineID = Account->GetDisplayName({});
	OnDHInvivationReceived.Broadcast(InvitorAccountID, InvitorOnlineID);
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnSessionInviteReceived 111 Invitor = %s"), *InvitorOnlineID);
}

void UDHOnlinePS4::Impl::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnJoinSessionComplete SessionName = %s, Result = %d"), *SessionName.ToString(), (int32)Result);
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		_TeamID = 0;
	}
	OnDHJoinSessionComplete.Broadcast(SessionName, Result);
}

void UDHOnlinePS4::Impl::CreateSession(uint64 TeamID, int32 TeamSize)
{
	if (_TeamID != 0 && _TeamID != TeamID)
		return;	//已经有session了必须先LeaveSession

	_TeamID = TeamID;
	FName SessionName = FormatSessionName(TeamID);
	if (SessionSys->GetNamedSession(SessionName) == nullptr)
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.NumPublicConnections = TeamSize;
		SessionSettings.NumPrivateConnections = 0;
		//SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowInvites = true;
		//SessionSettings.bShouldAdvertise = true;
		//SessionSettings.bIsLANMatch = true;
		SessionSettings.Set(SETTING_HOST_MIGRATION, true);
		SessionSys->CreateSession(LocalUserNum, SessionName, SessionSettings);
	}

}

void UDHOnlinePS4::Impl::LeaveSession(uint64 TeamID)
{
	SessionSys->DestroySession(FormatSessionName(TeamID), FOnDestroySessionCompleteDelegate::CreateLambda([=](FName SessionName, bool bWasSuccessful)
		{
			UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::LeaveSession Complete SessionName = %s bWasSuccessful = %s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));
			_TeamID = 0;
		}));
}

void UDHOnlinePS4::Impl::SendInvite(const TArray<FString>& InviteList, uint64 TeamID, int32 TeamSize)
{
	FString strFriend;
	for (auto tmp : InviteList)
	{
		strFriend = strFriend + tmp + TEXT(" ");
	}
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::SendInvite FriendDisplayName = %s TeamID = %llu"), *strFriend, TeamID);
	if (TeamID == 0)
		return;
	if (_TeamID != 0 && TeamID != _TeamID)
		return; //表示当前的已经有队伍了，又以另一个队伍的形式邀请，是不允许的，必须先解散 之前的队伍再邀请

	_TeamID = TeamID;
	FriendListToInvite.Append(InviteList);
	FName SessionName = FormatSessionName(TeamID);

	if (SessionSys->GetNamedSession(SessionName) == nullptr)
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.NumPublicConnections = TeamSize;
		SessionSettings.NumPrivateConnections = 0;
		//SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowInvites = true;
		//SessionSettings.bShouldAdvertise = true;
		//SessionSettings.bIsLANMatch = true;
		SessionSettings.Set(SETTING_HOST_MIGRATION, true);
		SessionSys->CreateSession(LocalUserNum, SessionName, SessionSettings);
	}
	else
	{
		SendInviteWithSessionCreatedInternal();
	}
}

void UDHOnlinePS4::Impl::SendPlayTogetherInvite(uint64 TeamID, int32 TeamSize)
{
	if (TeamID == 0)
		return;
	if (_TeamID != 0 && TeamID != _TeamID)
		return; //表示当前的已经有队伍了，又以另一个队伍的形式邀请，是不允许的，必须先解散 之前的队伍再邀请

	_TeamID = TeamID;
	FName SessionName = FormatSessionName(TeamID);

	if (SessionSys->GetNamedSession(SessionName) == nullptr)
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.NumPublicConnections = TeamSize;
		SessionSettings.NumPrivateConnections = 0;
		//SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowInvites = true;
		//SessionSettings.bShouldAdvertise = true;
		//SessionSettings.bIsLANMatch = true;
		SessionSettings.Set(SETTING_HOST_MIGRATION, true);
		SessionSys->CreateSession(LocalUserNum, SessionName, SessionSettings);
	}
	else
	{
		SendInviteWithSessionCreatedInternal();
	}
}

void UDHOnlinePS4::Impl::SendInviteWithSessionCreatedInternal()
{
	if (_TeamID == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UDHOnlinePS4::Impl::SendInviteWithSessionCreatedInternal error TeamID is 0"));
		return;
	}
	if (PlayTogetherUserList.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::SendInviteWithSessionCreatedInternal send invite to PlayTogetherUsers TeamID = %llu"), _TeamID);
		SessionSys->SendSessionInviteToFriends(LocalUserNum, FormatSessionName(_TeamID), PlayTogetherUserList);
		PlayTogetherUserList.Empty();
		return;
	}

	FString strFriend;
	for (auto tmp : FriendListToInvite)
	{
		strFriend = strFriend + tmp + TEXT(" ");
	}
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::SendInviteWithSessionCreatedInternal send invite to friends, FriendDisplayName = %s TeamID = %llu"), *strFriend, _TeamID);
	if (FriendListToInvite.Num() == 0)
		return;

	TArray< TSharedRef<const FUniqueNetId> > Friends;
	for (auto FriendDisplayName : FriendListToInvite)
	{
		for (auto& FriendData : FriendList)
		{
			if (FriendData->GetDisplayName() == FriendDisplayName)
			{
				IOnlineSessionPtr SessionPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetSessionInterface();
				Friends.Add(FriendData->GetUserId());
				continue;
			}
		}
	}
	SessionSys->SendSessionInviteToFriends(LocalUserNum, FormatSessionName(_TeamID), Friends);
	FriendListToInvite.Empty();
}

void UDHOnlinePS4::Impl::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnCreateSessionComplete SessionName = %s bWasSuccessful = %s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));
	if (bWasSuccessful == false)
	{
		_TeamID = 0;
		return;
	}

	FNamedOnlineSession* Session = SessionSys->GetNamedSession(SessionName);
	return_if_null(Session);

	FString SessionID = Session->SessionInfo->GetSessionId().ToString();

	QueuedAsyncProcessor.Start([=](DHQueuedAsyncProcessor::Feature* Feature) {
		FNpWebApiUserContext WebUserContect = GetSysPS4()->GetUserWebApiContext(*(static_cast<const FUniqueNetIdPS4*>(IdentitySys->GetUniquePlayerId(LocalUserNum).Get())));
		FUniqueNetIdString NetStringSessionID;
		NetStringSessionID.UniqueNetIdStr = SessionID;
		NetStringSessionID.Type = PS4_SUBSYSTEM;
		FString ChangeableData = FString::Printf(TEXT("%llu"), _TeamID);
		FOnlineAsyncTaskPS4SessionPutChangeableSessionData Web(GetSysPS4(), (int32)WebUserContect, &NetStringSessionID, ChangeableData);
		Web.DoWork();
		Feature->Complete([this]()
			{
				SendInviteWithSessionCreatedInternal();
			});
		});
}

void UDHOnlinePS4::Impl::OnSystemInviteAccepted(const FString& UserId, const FString& SessionId)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnSystemInviteAccepted UserId = %s, SessionId = %s"), *UserId, *SessionId);
	//接受邀请的处理在OnSessionUserInviteAccepted， 这里只打一条日志就好了
	//TSharedPtr<FUniqueNetIdPS4> SessionNetIdPS4 = FUniqueNetIdPS4::FromString(SessionId);
	//SessionSys->FindSessionById(*IdentitySys->GetUniquePlayerId(LocalUserNum).Get(), *static_cast<FUniqueNetId*>(SessionNetIdPS4.Get()), *static_cast<FUniqueNetId*>(SessionNetIdPS4.Get()), FOnSingleSessionResultCompleteDelegate::CreateLambda([&](int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult) 
	//{
	//	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnSystemInviteAccepted FindSessionById Complete LocalUserNum = %d bWasSuccessful = %s SessionId = %s "), LocalUserNum, bWasSuccessful ? TEXT("true") : TEXT("false"), *SearchResult.GetSessionIdStr());
	//	if (bWasSuccessful)
	//	{
	//		SessionSys->JoinSession(LocalUserNum, {}, SearchResult);
	//	}
	//}));

	//FString Url = FString(TEXT("/v1/sessions/")) + SessionId + FString(TEXT("?fields=@default,members"));
	//WebRequest(ENpApiGroup::SessionAndInvitation, SceNpWebApiHttpMethod::SCE_NP_WEBAPI_HTTP_METHOD_GET, Url, {}, [&](const FOnlineError& Error, const FString& ResponseBody)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnSystemInviteAccepted[] ErrorCode = %s ResponseBody = %s"), *Error.ErrorCode, *ResponseBody);
	//	JSON Json = JSON::parse(TCHAR_TO_UTF8(*ResponseBody), nullptr, false);
	//	if (Json.is_discarded()) {
	//		return;
	//	}
	//	auto IterValue = Json.find("sessionName");
	//	if (IterValue == Json.end()) {
	//		return;
	//	}
	//	FName SessionName = UTF8_TO_TCHAR(IterValue.value().get<std::string>().data());
	//	SessionSys->JoinSession(LocalUserNum, SessionName, {});
	//});


	//SceUserServiceUserId LocalUserId = FCString::Atoi(*UserId);
	//TSharedPtr<FUniqueNetIdPS4 const> PlayerId = FUniqueNetIdPS4::FindOrCreate(LocalUserId);
	//if (PlayerId->IsValid())
	//{
	//	FOnlineAsyncTaskManagerPS4 * AsyncTaskManager = GetSysPS4()->GetAsyncTaskManager();
	//	if (AsyncTaskManager != nullptr)
	//	{
	//		CurrentSessionSearch = MakeShareable(new FOnlineSessionSearch);
	//		CurrentSessionSearch->bIsLanQuery = false;
	//		CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::InProgress;

	//		FOnlineSessionSearchResult * NewResult = new(CurrentSessionSearch->SearchResults)FOnlineSessionSearchResult();

	//		EPS4Session::Type SessionType = EPS4Session::StandaloneSession;
	//		FOnlineSessionInfoPS4* NewSessionInfo = new FOnlineSessionInfoPS4(SessionType, 0, 0, 0, FUniqueNetIdString(SessionId, PS4_SUBSYSTEM));
	//		NewSessionInfo->Init();
	//		NewResult->Session.SessionInfo = MakeShareable(NewSessionInfo);

	//	}
	//	else
	//	{
	//		UE_LOG_ONLINE_SESSION(Warning, TEXT("UDHOnlinePS4::Impl::OnSystemInviteAccepted - No AsyncTaskManager, couldn't get session data."));
	//	}
	//}
	//else
	//{
	//	UE_LOG_ONLINE_SESSION(Warning, TEXT("UDHOnlinePS4::Impl::OnSystemInviteAccepted - Couldn't find local user to accept invite"));
	//}

	//IOnlineSessionPtr SessionPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetSessionInterface();
	////SessionPtr->OnSessionInviteAccepted
	//TSharedPtr<FOnlineSessionSearch> SearchObject = MakeShareable(new FOnlineSessionSearch);
	//SearchObject->MaxSearchResults = 1024;
	////SearchObject->QuerySettings.Set(SEARCH_OCULUS_MODERATED_ROOMS_ONLY, bSearchModeratedRoomsOnly, EOnlineComparisonOp::Equals);

	////if (!OculusPool.IsEmpty())
	////{
	////	SearchObject->QuerySettings.Set(SETTING_OCULUS_POOL, OculusPool, EOnlineComparisonOp::Equals);
	////}
	//SessionPtr->FindSessions(LocalUserNum, SearchObject.ToSharedRef());
	//if (SessionIdToJoin.IsEmpty())
	//{
	//	SessionIdToJoin = SessionId;
	//}
}

void UDHOnlinePS4::Impl::OnGetSessionCompleted(const FUniqueNetId& UserId, TSharedPtr<FOnlineSessionSearch> SessionId, const FString& SessionInfo, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnGetSessionCompleted UserId = %s SessionId->SearchResults.Num = %d SessionInfo = %s bWasSuccessful = %s"), *UserId.ToString(), SessionId->SearchResults.Num(), *SessionInfo, bWasSuccessful ? TEXT("true") : TEXT("false"));
	//if (bWasSuccessful && SessionId->SearchResults.Num() > 0)
	//{
	//	IOnlineSessionPtr SessionPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetSessionInterface();
	//	SessionPtr->JoinSession(LocalUserNum, FName(), SessionId->SearchResults[0]);
	//}
}

void UDHOnlinePS4::Impl::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnSessionUserInviteAccepted 111 bWasSuccessful = %s ControllerId = %d UserId = %s"), bWasSuccessful ? TEXT("true") : TEXT("false"), ControllerId, *UserId->ToString());
	InvitedSessionID = InviteResult.Session.GetSessionIdStr();
	DHInviteResult = InviteResult;
	DealWithAcceptedSessionUserInvite();
}

void UDHOnlinePS4::Impl::DealWithAcceptedSessionUserInvite()
{
	if (InvitedSessionID.IsEmpty() == true || IsInited == false)
		return;

	FString Url = FString(TEXT("/v1/sessions/")) + InvitedSessionID + FString(TEXT("?fields=@default,members"));
	WebRequest(ENpApiGroup::SessionAndInvitation, SceNpWebApiHttpMethod::SCE_NP_WEBAPI_HTTP_METHOD_GET, Url, {}, [=](const FOnlineError& Error, const FString& ResponseBody)
		{
			UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnSessionUserInviteAccepted 222 ErrorCode = %s ResponseBody = %s"), *Error.ErrorCode, *ResponseBody);
			if (Error.bSucceeded == false)
			{
				DHOnInvitationAccepted.Broadcast(0);
				return;
			}

			JSON Json = JSON::parse(TCHAR_TO_UTF8(*ResponseBody), nullptr, false);
			if (Json.is_discarded()) {
				return;
			}
			auto IterValue = Json.find("sessionName");
			if (IterValue == Json.end()) {
				return;
			}

			uint64 TeamID;
			LexFromString(TeamID, UTF8_TO_TCHAR(IterValue.value().get<std::string>().data()));
			UnfinishInviteList.Add(TeamID, DHInviteResult);
			DHOnInvitationAccepted.Broadcast(TeamID);

		});
	InvitedSessionID.Empty();
	MakeAllInvitationUsed();
}

void UDHOnlinePS4::Impl::OnSessionFailure(const FUniqueNetId& PlayerId, ESessionFailure::Type FailureType)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnSessionFailure PlayerId = %s FailureTyep = %s"), *PlayerId.ToString(), (int32)FailureType);
}

void UDHOnlinePS4::Impl::OnPlayTogetherEventReceived(int32 UserIndex, TArray<TSharedPtr<const FUniqueNetId>> UserIdList)
{
	PlayTogetherUserList.Empty();
	FString strUserIdList;
	for (auto UserId : UserIdList)
	{
		strUserIdList = strUserIdList + TEXT("[") + UserId->ToString() + TEXT("]");
		PlayTogetherUserList.Add(UserId.ToSharedRef());
	}
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnPlayTogetherEventReceived UserIndex = %d UserIdList = %s"), UserIndex, *strUserIdList);
	if (OnDHPlayTogetherEventReceived.IsBound())
	{
		OnDHPlayTogetherEventReceived.Broadcast();
	}
}

void UDHOnlinePS4::Impl::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnFindFriendSessionComplete bWasSuccessful = %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));

	//for (auto& SessionSearchResult : SearchResult)
	//{
	//	if (SessionSearchResult.GetSessionIdStr() == SessionIdToJoin)
	//	{
	//		IOnlineSessionPtr SessionPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetSessionInterface();
	//		SessionPtr->JoinSession(LocalUserNum, FName(), SessionSearchResult);
	//		break;
	//	}
	//}
	//SessionIdToJoin.Empty();
}

void UDHOnlinePS4::Impl::RegistPushEvent()
{
	QueuedAsyncProcessor.Start([this](DHQueuedAsyncProcessor::Feature* Feature) {
		UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::RegistPushEvent exec"));
		SceNpWebApiExtdPushEventFilterParameter filterParam[3];
		SceNpWebApiExtdPushEventExtdDataKey extdDataKey;

		memset(SceWebApiPushDataTypes, 0, sizeof(SceWebApiPushDataTypes));
		snprintf(SceWebApiPushDataTypes[eDHPushEventType_TeamMemJoin].val, SCE_NP_WEBAPI_PUSH_EVENT_DATA_TYPE_LEN_MAX,
			"np:service:session:game:join:to:member");
		snprintf(SceWebApiPushDataTypes[eDHPushEventType_TeamMemLeave].val, SCE_NP_WEBAPI_PUSH_EVENT_DATA_TYPE_LEN_MAX,
			"np:service:session:game:remove:to:member");
		snprintf(SceWebApiPushDataTypes[eDHPushEventType_TeamMemDataUpdate].val, SCE_NP_WEBAPI_PUSH_EVENT_DATA_TYPE_LEN_MAX,
			"np:service:session:game:update:to:member");
		// Push event filter parameters
		FMemory::Memzero(filterParam);
		filterParam[0].dataType = SceWebApiPushDataTypes[0];
		filterParam[1].dataType = SceWebApiPushDataTypes[1];
		filterParam[2].dataType = SceWebApiPushDataTypes[2];

		// Create handle
		int32 WebApiContext = GetSysPS4()->GetWebApiContext();
		int32 ret = sceNpWebApiCreateHandle(WebApiContext);
		if (ret < 0)
		{
			UE_LOG(LogTemp, Display, TEXT("UDHOnlinePS4::Impl::RegistPushEvent failed - sceNpWebApiCreateHandle returned with error code 0x%x"), ret);
		}
		int32 handleId = ret;

		ret = sceNpWebApiSetHandleTimeout(WebApiContext, handleId, 10 * 1000 * 1000);
		if (ret < 0)
		{
			UE_LOG(LogTemp, Display, TEXT("UDHOnlinePS4::Impl::RegistPushEvent failed - sceNpWebApiSetHandleTimeout returned with error code 0x%x"), ret);
		}

		// Create Push event filter. Note that this is a blocking function.
		ret = sceNpWebApiCreateExtdPushEventFilter(
			WebApiContext, handleId,
			"sessionInvitation",
			0,
			filterParam, 3);
		if (ret < 0)
		{
			UE_LOG(LogTemp, Display, TEXT("UDHOnlinePS4::Impl::RegistPushEvent failed - sceNpWebApiCreateExtdPushEventFilter returned with error code 0x%x"), ret);
		}

		int32 filterId = ret;

		// Register Push event callback function
		int32 UserWebApiContext = GetSysPS4()->GetUserWebApiContext(*this->GetNetID());
		ret = sceNpWebApiRegisterExtdPushEventCallbackA(/*this->GetNetID()->GetUserId()*/ UserWebApiContext, filterId, &Impl::OnWebApiPushMessageReceived, this);
		if (ret < 0)
		{
			UE_LOG(LogTemp, Display, TEXT("UDHOnlinePS4::Impl::RegistPushEvent failed - sceNpWebApiRegisterExtdPushEventCallbackA returned with error code 0x%x"), ret);
		}
		});
}

void UDHOnlinePS4::Impl::OnWebApiPushMessageReceived(int32_t userCtxId, int32_t callbackId, const char* pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA* pTo, const SceNpOnlineId* pToOnlineId, const SceNpPeerAddressA* pFrom, const SceNpOnlineId* pFromOnlineId, const SceNpWebApiPushEventDataType* pDataType, const char* pData, size_t dataLen, const SceNpWebApiExtdPushEventExtdData* pExtdData, size_t extdDataNum, void* pUserArg)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::OnWebApiPushMessageReceived userCtxId = %d callbackId = %d pNpServiceName = %s npServiceLabel = %lu DataType = %s, Data = %s pExtdData.Key = %s pExtdData.Data = %s"), userCtxId, callbackId, UTF8_TO_TCHAR(pNpServiceName), npServiceLabel, UTF8_TO_TCHAR(pDataType->val), UTF8_TO_TCHAR(pData), pExtdData->extdDataKey.val, pExtdData ? UTF8_TO_TCHAR(pExtdData->pData) : TEXT(""));
	//FUniqueNetIdPS4 SessionID = FUniqueNetIdPS4::FromString()
	//SessionSys->FindSessionById(IdentitySys->GetUniquePlayerId(LocalUserNum), )
}

FName UDHOnlinePS4::Impl::FormatSessionName(uint64 TeamID)
{
	FString SessionNameStr = FString::Printf(TEXT("%llu"), TeamID);
	return *SessionNameStr;
}

void UDHOnlinePS4::Impl::JoinSession(uint64 TeamID)
{
	if (UnfinishInviteList.Find(TeamID) == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::JoinSession failed cannot find invite info by TeamID = %llu"), TeamID);
		return;
	}
	_TeamID = TeamID;
	const FOnlineSessionSearchResult& InviteResult = UnfinishInviteList.FindRef(TeamID);
	SessionSys->JoinSession(LocalUserNum, FName(), InviteResult);
}

void UDHOnlinePS4::Impl::IgnoreInvite(uint64 TeamID)
{
	UnfinishInviteList.Remove(TeamID);
}

void UDHOnlinePS4::Impl::MakeAllInvitationUsed()
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::MakeAllInvitationUsed exec"));
	FString Url = FString(TEXT("/v1/users/me/invitations?fields=@default")); //=
	WebRequest(ENpApiGroup::SessionAndInvitation, SCE_NP_WEBAPI_HTTP_METHOD_GET, Url, {}, [=](const FOnlineError& Error, const FString& ResponseBody)
		{
			UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::MakeAllInvitationUsed GetInvitationlist Result = %s  ResponseBody = %s"), Error.bSucceeded ? TEXT("true") : TEXT("false"), *ResponseBody);
			if (Error.bSucceeded)
			{
				JSON Json = JSON::parse(TCHAR_TO_UTF8(*ResponseBody), nullptr, false);
				if (Json.is_discarded()) {
					return;
				}
				auto IterValue = Json.find("invitations");
				if (IterValue == Json.end()) {
					return;
				}
				JSON Value = IterValue.value();
				if (!Value.is_array()) {
					return;
				}

				for (int32 i = 0; i < Value.size(); ++i) {
					JSON& Arr = Value.at(i);
					continue_if_true(!JsonHasFields(Arr, { "usedFlag", "invitationId" }));

					bool bUsed = Arr["usedFlag"].get<bool>();
					if (bUsed == true)
						continue;

					FString InvitationID = JSON_STRING_TO_TCHAR(Arr, "invitationId");
					FString RequestBody = TEXT("{\"usedFlag\": true }");
					FString Url = FString(TEXT("/v1/users/me/invitations/")) + InvitationID;
					WebRequest(ENpApiGroup::SessionAndInvitation, SCE_NP_WEBAPI_HTTP_METHOD_PUT, Url, RequestBody, [](const FOnlineError& Error, const FString& ResponseBody)
						{
							UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::MakeAllInvitationUsed  MakeUsed complete, Error = %s, ResponseBody = %s"), *Error.ErrorCode, *ResponseBody);
						});

				}

			}

		});
}

void UDHOnlinePS4::Impl::ShowBrowser(const FString& Url)
{
	FShowWebUrlParams Params;
	GetSysPS4()->GetExternalUIInterface()->ShowWebURL(Url, Params, {});
}

void UDHOnlinePS4::Impl::CallbackStatic(NpToolkit::Core::CallbackEvent* evt)
{
	UDHOnlinePS4::Impl* _Impl = (UDHOnlinePS4::Impl*)evt->appData;
	if (evt->response->getReturnCode() < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UDHOnlinePS4::Impl::CallbackStatic exec returncode = %x "), evt->response->getReturnCode());
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::Impl::CallbackStatic exec service = %d apiCalled = %d"), (int32)evt->service, (int32)evt->apiCalled);
	if (evt->service == NpToolkit::Core::ServiceType::notification)
	{
		if (evt->apiCalled == NpToolkit::Core::FunctionType::notificationUpdateFriendPresence)
		{
			NpToolkit::Core::Response<NpToolkit::Presence::Notification::PresenceUpdate>* tmp = (NpToolkit::Core::Response<NpToolkit::Presence::Notification::PresenceUpdate>*)(evt->response);
			const NpToolkit::Presence::Notification::PresenceUpdate* responseData = tmp->get();
			NpToolkit::Core::OnlineUser remoteUser = responseData->remoteUser;
			_Impl->QueuedAsyncProcessor.AddMainThreadFunction([_Impl, remoteUser]() {
				_Impl->GetFriendPresense(remoteUser);
				});
		}
	}
	else if (evt->service == NpToolkit::Core::ServiceType::friends)
	{
		if (evt->apiCalled == NpToolkit::Core::FunctionType::friendsGetFriends)
		{
			NpToolkit::Core::Response<NpToolkit::Friend::Friends>* tmp = (NpToolkit::Core::Response<NpToolkit::Friend::Friends>*)(evt->response);
			const NpToolkit::Friend::Friends* responseData = tmp->get();
			TArray<FDHFriendInfo> FriendList;
			for (int32 FriendIndex = 0; FriendIndex < responseData->numFriends; ++FriendIndex)
			{
				NpToolkit::Friend::Friend const& Friend = responseData->friends[FriendIndex];
				FDHFriendInfo& Info = FriendList.AddDefaulted_GetRef();
				_Impl->PresenceToFriendInfo(&Friend.presence, &Info);
				Info.Name = PS4OnlineIdToString(Friend.profile.onlineUser.onlineId);
				Info.AccountID = Friend.profile.onlineUser.accountId;
			}
			_Impl->QueuedAsyncProcessor.AddMainThreadFunction([_Impl, FriendList]() {
				_Impl->OnDHFriendListUpdated.Broadcast(FriendList);
				});

		}
	}
}

void UDHOnlinePS4::Impl::GetFriendPresense(NpToolkit::Core::OnlineUser Friend)
{
	QueuedAsyncProcessor.Start([this, UserID = GetServiceUserID(), Friend](DHQueuedAsyncProcessor::Feature* Feature) {
		NpToolkit::Presence::Request::GetPresence req;
		NpToolkit::Core::Response<NpToolkit::Presence::Presence> res;
		InitNpSyncRequest(&req, UserID);
		req.fromUser = Friend.accountId;

		int32 ret = NpToolkit::Presence::getPresence(req, &res);
		if (ret < 0) {
			// Error handling
		}
		const NpToolkit::Presence::Presence* Tmp = res.get();
		UE_LOG(LogTemp, Display, TEXT("UDHOnlinePS4::Impl::GetFriendPresense ret = %x"), ret);
		FDHFriendInfo Info;
		PresenceToFriendInfo(Tmp, &Info);
		UE_LOG(LogTemp, Log, TEXT("==== UDHOnlinePS4::Impl::OnFriendPresenceUpdated Friend = (%s, %llu, %d)"), *Info.Name, Info.AccountID, (int32)Info.Status);
		Feature->Complete([this, Info]
			{
				OnFriendPresenceUpdated(Info);
			});
	});
}

void UDHOnlinePS4::Impl::OnFriendPresenceUpdated(const FDHFriendInfo& Info)
{
	OnDHFriendPresenceUpdated.Broadcast(Info);
}

void UDHOnlinePS4::Impl::IgnorePlayTogetherEvent()
{
	PlayTogetherUserList.Empty();
}

void UDHOnlinePS4::Store_ShowPsStoreIcon(EStoreIconPosPS4 Pos)
{
	return_if_true(_Impl->PsStoreIconVisible == 1);
	UE_LOG(LogPS4, Warning, TEXT("UDHOnlinePS4::Store_ShowPsStoreIcon-----------------------------------"));
	sceNpCommerceShowPsStoreIcon(static_cast<SceNpCommercePsStoreIconPos>(Pos));
	_Impl->PsStoreIconVisible = 1;
}

void UDHOnlinePS4::Store_HidePsStoreIcon()
{
	return_if_true(_Impl->PsStoreIconVisible == 0);
	UE_LOG(LogPS4, Warning, TEXT("UDHOnlinePS4::Store_HidePsStoreIcon-----------------------------------"));
	sceNpCommerceHidePsStoreIcon();
	_Impl->PsStoreIconVisible = 0;
}

void UDHOnlinePS4::Store_ShowEmptyDialog()
{
	IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetExternalUIInterface()->ShowPlatformMessageBox(*_Impl->GetNetID(), EPlatformMessageType::EmptyStore);
}

bool UDHOnlinePS4::Store_PsStoreIconIsVisible()
{
	return _Impl->PsStoreIconVisible != 0;
}

void UDHOnlinePS4::Store_ShowCheckoutDialog(const FString& ProductLabel, FStore_ShowCheckoutDialogDelegate Function)
{
	_Impl->QueuedAsyncProcessor.Start([UserID = _Impl->GetServiceUserID(), ProductLabel, Function](DHQueuedAsyncProcessor::Feature* Feature) {
		NpToolkit::Commerce::Request::DisplayProductBrowseDialog Request{};
		InitNpSyncRequest(&Request, UserID);
		std::string Label = TCHAR_TO_ANSI(*ProductLabel);
		strcpy(Request.productLabel.value, Label.c_str());

		NpToolkit::Core::Response<NpToolkit::Core::Empty> Response;
		int Ret = NpToolkit::Commerce::displayProductBrowseDialog(Request, &Response);
		int32 CaseLogOnce = 0;

		FOnlineError OnlineError(true);
		switch (Ret) {
			DH_DEFINE_CASE(SCE_TOOLKIT_NP_V2_DIALOG_RESULT_USER_CANCELED)
				OnlineError.bSucceeded = false;
			OnlineError.SetFromErrorCode("User Canceled");
			break;
			DH_DEFINE_CASE(SCE_TOOLKIT_NP_V2_DIALOG_RESULT_OK)
				DH_DEFINE_CASE(SCE_TOOLKIT_NP_V2_DIALOG_RESULT_USER_PURCHASED)
				break;
		default:
			UE_LOG(LogTemp, Display, TEXT("Enter switch case, value = OtherErrors"));
			OnlineError.bSucceeded = false;
			OnlineError.SetFromErrorCode(FString::Printf(TEXT("Unexpected error, code=0x%08x"), Ret));
			break;
		}

		UE_LOG(LogTemp, Display, TEXT("Store_Checkout: ProductLabel=%s, Error=%s"), *ProductLabel, *OnlineError.ErrorCode);
		Feature->Complete([Function, OnlineError, ProductLabel] {
			Function.ExecuteIfBound(OnlineError, ProductLabel);
			});
	});
}

bool UDHOnlinePS4::IsNetErrorCode(int32 ErrorCode)
{
	return_true_if_true(ErrorCode >= 0x80410101 && ErrorCode <= 0x804101ea);
	return_true_if_true(ErrorCode == 0x80552C13);
	return false;
}

FSimpleDelegate& UDHOnlinePS4::GetOnUserLogout()
{
	return _Impl->OnUserLogout;
}

//---------------Invivation--------------------------------
void UDHOnlinePS4::UpdateFriendList()
{
	_Impl->UpdateFriendList();
}

UDHOnlinePS4::FOnDHFriendListUpdated& UDHOnlinePS4::GetOnFriendListUpdated()
{
	return _Impl->OnDHFriendListUpdated;
}

UDHOnlinePS4::FOnFriendPresenceUpdated& UDHOnlinePS4::GetOnFriendPresenceUpdated()
{
	return _Impl->OnDHFriendPresenceUpdated;
}

UDHOnlinePS4::FOnDHPlayTogetherEventReceived& UDHOnlinePS4::GetOnDHPlayTogetherEventReceived()
{
	return _Impl->OnDHPlayTogetherEventReceived;
}

bool UDHOnlinePS4::HasPlayTogetherEvent()
{
	return _Impl->PlayTogetherUserList.Num() > 0;
}

void UDHOnlinePS4::CreateSession(uint64 TeamID, int32 TeamSize)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::CreateSession TeamID = %llu, TeamSize = %d"), TeamID, TeamSize);
	_Impl->CreateSession(TeamID, TeamSize);
}

void UDHOnlinePS4::LeaveSession(uint64 TeamID)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::LeaveSession TeamID = %llu"), TeamID);
	_Impl->LeaveSession(TeamID);
}

void UDHOnlinePS4::JoinSession(uint64 TeamID)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::JoinSession TeamID = %llu"), TeamID);
	_Impl->JoinSession(TeamID);
}

void UDHOnlinePS4::IgnoreInvite(uint64 TeamID)
{
	_Impl->IgnoreInvite(TeamID);
}

void UDHOnlinePS4::SendInvite(const TArray<FString>& InviteList, uint64 TeamID, int32 TeamSize)
{
	_Impl->SendInvite(InviteList, TeamID, TeamSize);
}

void UDHOnlinePS4::SendPlayTogetherInvite(uint64 TeamID, int32 TeamSize)
{
	_Impl->SendPlayTogetherInvite(TeamID, TeamSize);
}

void UDHOnlinePS4::IgnorePlayTogetherEvent()
{
	_Impl->IgnorePlayTogetherEvent();
}

UDHOnlinePS4::FDHOnInvivationReceived& UDHOnlinePS4::GetInvivationReceivedDelegate()
{
	return _Impl->OnDHInvivationReceived;
}

UDHOnlinePS4::FDHOnInvitationAccepted& UDHOnlinePS4::GetInvitationAcceptDelegate()
{
	return _Impl->DHOnInvitationAccepted;
}

UDHOnlinePS4::FDHOnJoinSessionComplete& UDHOnlinePS4::GetJoinSessionCompleteDelegate()
{
	return _Impl->OnDHJoinSessionComplete;
}

void UDHOnlinePS4::AcceptInvite(uint64 InvitorAccountID)
{
	//if (_Impl->InviteList.Find(InvitorAccountID) == nullptr)
	//	return;

	//IOnlineSessionPtr SessionPtr = IOnlineSubsystem::Get(PS4_SUBSYSTEM)->GetSessionInterface();
	//FOnlineSessionSearchResult OnlineSessionSearchResult = _Impl->InviteList.FindRef(InvitorAccountID);
	//FNamedOnlineSession* NameSessionPtr = static_cast<FNamedOnlineSession*>(&OnlineSessionSearchResult.Session);
	//SessionPtr->JoinSession(*_Impl->GetNetID(), NameSessionPtr->SessionName, OnlineSessionSearchResult);
}


bool UDHOnlinePS4::IsEnterCrossNotCircle()
{
	int32 value = 0;
	int32 ret = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_ENTER_BUTTON_ASSIGN, &value);
	if (ret != SCE_OK) {
		UE_LOG(LogTemp, Error, TEXT("sceSystemServiceParamGetInt() error ret = 0x[%x]\n"), ret);
		return false;
	}
	else {
		if (value == SCE_SYSTEM_PARAM_ENTER_BUTTON_ASSIGN_CROSS) {
			return true;
		}
		else {
			//value == SCE_SYSTEM_PARAM_ENTER_BUTTON_ASSIGN_CIRCLE
			return false;
		}
	}
}

void UDHOnlinePS4::ShowBrowser(const FString& Url)
{
	_Impl->ShowBrowser(Url);
}

void UDHOnlinePS4::SetPresence(const struct FPWPresenceData* PresenceData)
{
	auto NetID = _Impl->GetNetID();
	if (!NetID) {
		FOnlineError OnlineError((int32)SCE_TOOLKIT_NP_V2_ERROR_INVALID_USER);
		UE_LOG(LogTemp, Error, TEXT("========UDHOnlinePS4::SetPresence() Failed NetID error"));
		return;
	}

	_Impl->QueuedAsyncProcessor.Start([UserID = _Impl->GetServiceUserID(), PresenceData](DHQueuedAsyncProcessor::Feature* Feature) {
		NpToolkit::Presence::Request::SetPresence req;
		NpToolkit::Core::Response<NpToolkit::Core::Empty> res;
		InitNpSyncRequest(&req, UserID);
		req.numLocalizedGameStatuses = PresenceData->Display.Num();
		req.localizedGameStatuses = new NpToolkit::Presence::Request::LocalizedGameStatus[req.numLocalizedGameStatuses];
		for (int32 Index = 0; Index < req.numLocalizedGameStatuses; ++Index)
		{
			strncpy(req.localizedGameStatuses[Index].languageCode.code, TCHAR_TO_UTF8(*PresenceData->Display[Index].Language), SCE_NP_LANGUAGE_CODE_MAX_LEN);
			strncpy(req.localizedGameStatuses[Index].gameStatus, TCHAR_TO_UTF8(*PresenceData->Display[Index].GameStatus), req.localizedGameStatuses[Index].MAX_SIZE_LOCALIZED_GAME_STATUS);
		}

		strncpy(req.defaultGameStatus, TCHAR_TO_UTF8(*PresenceData->DefaultDisplay), req.MAX_SIZE_DEFAULT_GAME_STATUS);

		req.binaryGameDataSize = sizeof(uint8);
		memcpy(req.binaryGameData, &PresenceData->Status, req.binaryGameDataSize);

		int32 ret = NpToolkit::Presence::setPresence(req, &res);
		if (ret < 0) {
			// Error handling
		}
		UE_LOG(LogTemp, Display, TEXT("======== UDHOnlinePS4::SetPresence ret = %x"), ret);
		Feature->Complete([]
			{
				UE_LOG(LogTemp, Display, TEXT("======== UDHOnlinePS4::SetPresence Complete"));
			});
	});
}


void UDHOnlinePS4::SetPresence(EGameStatus Status)
{
	UE_LOG(LogTemp, Log, TEXT("UDHOnlinePS4::SetPresence Status = %d"), (int32)Status);
	const TArray<const FPWPresenceData*> ConfigList = PWPresenceDataTable::GetInstance()->GetRows();
	for (auto Config : ConfigList)
	{
		if (Config->Status == Status)
		{
			SetPresence(Config);
		}
	}

}

static FString ConvertSonyStoreText(const char* Utf8StoreTextWithHtml)
{
	FString StoreTextWithHtml = UTF8_TO_TCHAR(Utf8StoreTextWithHtml);
	FString StoreText;
	int32 TagStart = -1, EscapeStart = -1;
	int32 Index = 0;
	for (TCHAR Char : StoreTextWithHtml.GetCharArray()) {
		// see if we're outside of an HTML tag
		if (TagStart >= 0) {
			// we're inside a tag, look for the end
			if (Char == '>') {
				const FString HtmlTag = StoreTextWithHtml.Mid(TagStart + 1, Index - TagStart - 1);
				if (HtmlTag == TEXT("br")) {
					StoreText.AppendChar(TEXT('\n'));
				}

				// TODO: do we want to parse any more tags?
				TagStart = -1;
			}
		}
		else if (EscapeStart >= 0) {
			if (Char == ';') {
				FString Sequence = StoreTextWithHtml.Mid(EscapeStart + 1, Index - EscapeStart - 1);
				if (!Sequence.IsEmpty()) {
					if (Sequence == TEXT("nbsp")) {
						StoreText.AppendChar(TEXT(' '));
					}
					else if (Sequence == TEXT("amp")) {
						StoreText.AppendChar(TEXT('&'));
					}
					else if (Sequence == TEXT("quot")) {
						StoreText.AppendChar(TEXT('"'));
					}
					else if (Sequence == TEXT("lt")) {
						StoreText.AppendChar(TEXT('<'));
					}
					else if (Sequence == TEXT("gt")) {
						StoreText.AppendChar(TEXT('>'));
					}
				}

				// end escape mode
				EscapeStart = -1;
			}
			else if (Index - EscapeStart >= 5) {
				// never saw a ;, just abort the escape
				for (int32 i = EscapeStart; i <= Index; ++i) {
					StoreText.AppendChar(StoreTextWithHtml[i]);
				}
				EscapeStart = -1;
			}
		}
		else {
			if (Char == '<') {
				TagStart = Index;
			}
			else if (Char == '&') {
				EscapeStart = Index;
			}
			else if (Char == '\n' || Char == '\r') {
				// Skip literal newlines, must use <br> instead
			}
			else {
				StoreText.AppendChar(Char);
			}
		}
		++Index;
	}
	return StoreText;
}

#endif

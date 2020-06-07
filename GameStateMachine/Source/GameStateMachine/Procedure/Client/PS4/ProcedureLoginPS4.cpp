#include "PWProcedureLoginPS4.h"
#ifdef PLATFORM_PS4
#include "PWUIManager.h"
#include "Online/PS4/DHOnlinePS4.h"
#include "Lobby/PWStartGame.h"
#include "PWProcedureManager.h"
#include "Runtime/EngineSettings/Classes/GameMapsSettings.h"
#include "PWClientNet.h"
#include "Client/PWProcedureExitBattle.h"
#include "DHHttpClient.h"
#include "PWGameDataConfig.h"
#include "Client/PWProcedurePolicy.h"
#include "PWSaveGameManager.h"
#include "DHPlatform.h"
#include "Online/DHEndpointTracker.h"
#include "PWLobbyTeamManager.h"
#include "PWUpdateInfo.h"
#include "PWVivoxConfig.h"
#include "PWAssetManager.h"
#include "PWLibrary.h"
#include "DHInternational.h"

static void OnLoginComplete(ECheckUserPrivilegePS4, EOnlineErrorCodePS4, UPWProcedureLoginPS4* Owner);

void UPWProcedureLoginPS4::Enter()
{
    DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_StartLogin);
    UPWGameInstance::Get(this)->ShowLoadingScreen(true);
    _LoginStatus = (ELoginStatus)((uint32)ELoginStatus::LS_None | (uint32)ELoginStatus::LS_LoadDown);	//地图加载在Update阶段做了
    UnfinishedHttpRequest = 0;
    UPWClientNet::GetInstance(this)->Close();

    UDHOnlinePS4* Online = UDHOnlinePS4::GetInstance(this);
    Online->CheckUserPrivilegeAndLogin(UDHOnlinePS4::FCheckUserPrivilegeDelegate::CreateStatic(OnLoginComplete, this));
    PW_LOG(LogTemp, Display, TEXT("Begin Login PSN"));

    //if (_EnterTimes > 0) {
    //    OnPostLoadAllMaps();
    //}
}

void UPWProcedureLoginPS4::Leave()
{
    CloseBulletinsWidget();
    UPWUIManager* UIMgr = UPWUIManager::Get(this);
    check(UIMgr);
    UIMgr->HideUI(EUMGID::EUMGID_Queue);
    UIMgr->HideUI(EUMGID::EUMGID_Login);
    UIMgr->HideUI(EUMGID::EUMGID_Connecting);
    UPWGameInstance::Get(this)->HideLoadingScreen();
}

void UPWProcedureLoginPS4::OnPostLoadMap(UWorld* LoadedWorld)
{
    //UPWUIManager* UIMgr = UPWUIManager::Get(this);
    //UIMgr->PreLoadUI(EUMGID::EUMGID_WW_CreateChar);
    //UIMgr->PreLoadUI(EUMGID::EUMGID_WW_Lobby);
}

void UPWProcedureLoginPS4::OnPostLoadAllMaps()
{
    //ChangeLoginStatus(EPS4LoginStatus::LS_LoadDown, EOnlineErrorCodePS4::None);
    //++_EnterTimes;
}

void UPWProcedureLoginPS4::OnBulletinRefreshed()
{
    ChangeLoginStatus(ELoginStatus::LS_BullentDown, 0);
}

void UPWProcedureLoginPS4::ChangeLoginStatus(ELoginStatus LoginStatus, uint32 ErrorCode)
{
    PW_LOG(LogTemp, Log, TEXT("UPWProcedureLoginPS4::ChangeLoginStatus NewStatus = %u ErrorCode = %u"), (uint32)LoginStatus, ErrorCode);
    _LoginStatus = (ELoginStatus)((uint32)_LoginStatus | (uint32)LoginStatus);

    if (((uint32)_LoginStatus & (uint32)ELoginStatus::LS_HasError)) {                // PSN Login Failed
        ShowError((EOnlineErrorCodePS4)ErrorCode);
        return;
    }

    if (((uint32)_LoginStatus & (uint32)ELoginStatus::LS_LoginDown)) {               // PSN Login Sucess
        if (!(((uint32)_LoginStatus & (uint32)ELoginStatus::LS_HttpServeDown))) {    // Start Get Server List
            StartRequestHttpServe();
            return;
        }
    }

    if (!((uint32)_LoginStatus & (uint32)ELoginStatus::LS_HttpServeDown))
        return;

    if (!((uint32)_LoginStatus & (uint32)ELoginStatus::LS_UpdateServerDown))
        return;

    if (!((uint32)_LoginStatus & (uint32)ELoginStatus::LS_BullentDown))	//表示数据还没拿到，只需等待公告数据返回就好
        return;

    CheckBulletinMsg();

    FPWVivoxConfig* VivoxConfig = GetInstanceOfJson<FPWVivoxConfig>();
    if (VivoxConfig->IsVivoxEnabled)
    {
        UPWVivoxManager::Get(this)->InitializeVivox(1);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Vivox is not Enabled"));
    }

    PW_LOG(LogTemp, Display, TEXT("UPWProcedureLoginPS4::LoginStatus = AllDown"));

    UDHOnlinePS4* PS4 = UDHOnlinePS4::GetInstance(this);
    PS4->OnInitCompleted();
    PWLIB::SetAccountArea(ELicenseAreaPS4::ToString(PS4->GetCountryPS4().LicenseArea));
    UPWAssetManager::Get(this)->Init();


    if (UPWLobbyTeamManager::GetInstance(this)->IsWaittingForEnterTeam() || UPWLobbyTeamManager::GetInstance(this)->HavePlayTogetherEventToDeal())
    {
        AutoLogin();
    }
    else
    {
        UPWUIManager* UIMgr = UPWUIManager::Get(this);
        UIMgr->ForceShowUI(EUMGID::EUMGID_Login);
        UPWStartGame* StartGameWidget = Cast<UPWStartGame>(UIMgr->GetWidget(EUMGID::EUMGID_Login));
        if (StartGameWidget) {
            StartGameWidget->SetWaitForPrivilegeChecks(false);
        }

        UPWGameInstance::Get(this)->HideLoadingScreen();
    }

    PS4->GetOnUserLogout().Unbind();
    PS4->GetOnUserLogout().BindUObject(this, &UPWProcedureLoginPS4::OnUserLogout);
}

void UPWProcedureLoginPS4::AutoLogin()
{
    FString Account, Token;
    bool PlusAccess = false;
    UDHOnlinePS4* PS4 = UDHOnlinePS4::GetInstance(this);
    Account = PS4->GetAccountIDString();
    Token = PS4->GetAuthToken();
    PlusAccess = PS4->PlusAccess();
    const FString CurLang = DHInternational::GetInstance()->GetDisplayLanguage();
    UPWClientNet::GetInstance(this)->LoginReq(Account, Token, PlusAccess, CurLang);
}

void UPWProcedureLoginPS4::OnOneHttpRequestFinished()
{
    UnfinishedHttpRequest--;
    if (UnfinishedHttpRequest == 0)
    {
        DHFiles::IncGlobalCfgVersion();
        UPWAssetManager::Get(this)->Init();
        UPWUIManager::Get(this)->Init();
        DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndUpdate, { ETrackFieldName::Code, 0 });
        ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_UpdateServerDown, 0);
    }
}

void UPWProcedureLoginPS4::ShowError(EOnlineErrorCodePS4 ErrorCode)
{
    PW_LOG(LogTemp, Display, TEXT("LoginStatus=HasError, ErrorCode=0x%08x"), (uint32)_LoginStatus);
    FString ErrorMessage;
    if (UDHOnlinePS4::IsNetErrorCode((int32)ErrorCode)) {           // 网络错误，重试
        ErrorMessage = DH_LOCTABLE_STR("GL_MSG_NET_UNAVAILABLE");
    }
    else {
        switch (ErrorCode) {
        case EOnlineErrorCodePS4::NetworkConnectionUnavailable: // 网络错误，重试
            ErrorMessage = DH_LOCTABLE_STR("GL_MSG_NET_UNAVAILABLE");
            break;
        case EOnlineErrorCodePS4::AuthorizationFailed:
            ErrorMessage = DH_LOCTABLE_STR("GL_MSG_NOPSN");
            break;
        case EOnlineErrorCodePS4::InMaintenance:
            ErrorMessage = DH_LOCTABLE_STR("GL_MSG_MAINTAIN");
            break;
        case EOnlineErrorCodePS4::HttpServeHasError:
            ErrorMessage = DH_LOCTABLE_STR("GL_MSG_SEVERFAILURE");
            break;
        case EOnlineErrorCodePS4::VersionOrAccountNotSupported:
            ErrorMessage = DH_LOCTABLE_STR("GL_MSG_PSNACCOUNT");
            break;
        case EOnlineErrorCodePS4::JsonParseFailed:
            ErrorMessage = DH_LOCTABLE_STR("GL_MSG_SEVERLIST");
            break;
        case EOnlineErrorCodePS4::None:
            UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_Login);
            return;
        case EOnlineErrorCodePS4::OtherErrors:
        default:
            ErrorMessage = DH_LOCTABLE_STR("GL_MSG_UNKNOWN_ERROR");
            break;
        }
    }

    PW_LOG(LogTemp, Error, TEXT("错误提示：%s"), *ErrorMessage);
    UDHOnlinePS4::ShowErrorMessageDialog(ErrorMessage,
        UDHOnlinePS4::FShowMessageDialogDelegate::CreateLambda([this](EDHAppReturnType ReturnType) {
            UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_Login);
            }));
}

void UPWProcedureLoginPS4::OnUserLogout()
{
    UPWProcedureManager* ProcedureMgr = UPWProcedureManager::GetInstance(this);
    switch (ProcedureMgr->GetCurState()) {
    case ProcedureState::ProcedureState_Battle:
    {
        UPWProcedureExitBattle* ProcedureExitBattle = Cast<UPWProcedureExitBattle>(ProcedureMgr->GetProcedure(ProcedureState::ProcedureState_ExitBattle));
        ProcedureExitBattle->SetNextState(ProcedureState::ProcedureState_Login);
        ProcedureMgr->ChangeCurState(ProcedureState::ProcedureState_ExitBattle);
        break;
    }
    case ProcedureState::ProcedureState_ExitBattle:
    {
        UPWProcedureExitBattle* ProcedureExitBattle = Cast<UPWProcedureExitBattle>(ProcedureMgr->GetProcedure(ProcedureState::ProcedureState_ExitBattle));
        ProcedureExitBattle->SetNextState(ProcedureState::ProcedureState_Login);
        break;
    }
    default:
        ProcedureMgr->ChangeCurState(ProcedureState::ProcedureState_Login);
        break;
    }
}

static void OnLoginComplete(ECheckUserPrivilegePS4 CheckResult, EOnlineErrorCodePS4 ErrorCode, UPWProcedureLoginPS4* Owner)
{
    auto Tracker = DHEndpointTracker::GetInstance();
    switch (CheckResult) {
    case ECheckUserPrivilegePS4::Ok:
    {
        auto PS4 = UDHOnlinePS4::GetInstance(Owner);
        Tracker->SetUserID(PS4->GetAccountIDString());
        Tracker->Track(EEndpointTrackEvent::TE_EndLogin, { ETrackFieldName::IsPlus, PS4->PlusAccess() });
        Owner->ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_LoginDown, 0);
        //UDHOnlinePS4::GetInstance(Owner)->Init();
        UPWLobbyTeamManager::GetInstance(Owner)->Init();
        break;
    }
    case ECheckUserPrivilegePS4::TryAgain:
        Tracker->Track(EEndpointTrackEvent::TE_HasError, { ETrackFieldName::Code, (int)ErrorCode });
        Owner->ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HasError, (uint32)ErrorCode);
        break;
    case ECheckUserPrivilegePS4::Stop:
        break;
    }
}

void UPWProcedureLoginPS4::StartRequestHttpServe()
{
    auto Config = GetInstanceOfJson<FPWGameDataConfig>();
    auto PS4 = UDHOnlinePS4::GetInstance(this);
    FCountryPS4 Country = PS4->GetCountryPS4();
    DETNetClient& Net = DETNetClient::Instance();

    FString AreaName = ELicenseAreaPS4::ToString(Country.LicenseArea);
#if IS_DEVELOPMENT_MODE
    AreaName = Config->AreaName;
#endif

    std::initializer_list<DHHttpField> Fields = {
        { TEXT("country"), Country.CountryCode },
        { TEXT("platform"), DHPlatform::GetTargetName(true) },
        { TEXT("env"), PS4->GetOnlineEnvironmentName() },
        { TEXT("area"), AreaName },
        { TEXT("userid"), PS4->GetAccountIDString() },
        { TEXT("ver"), Config->Version }
    };

    DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_StartGetServerList,
        { TEXT("country"), Country.CountryCode },
        { TEXT("platform"), DHPlatform::GetTargetName(true) },
        { TEXT("env"), PS4->GetOnlineEnvironmentName() },
        { TEXT("area"), AreaName },
        { TEXT("userid"), PS4->GetAccountIDString() },
        { TEXT("ver"), Config->Version }
    );

    Net.BeginConnectV2(Config->HttpAddress, Fields, DETNetClient::FBeginConnectV2Delegate::CreateLambda([this](DETNetClient::HttpServeStatus ServeStatus) {
        DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndGetServerList, { ETrackFieldName::Code, (int)ServeStatus });
        switch (ServeStatus) {
        case DETNetClient::HttpServeStatus::HttpServeError:
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HasError, (uint32)EOnlineErrorCodePS4::HttpServeHasError);
            return;
        case DETNetClient::HttpServeStatus::NetUnavailable:
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HasError, (uint32)EOnlineErrorCodePS4::NetworkConnectionUnavailable);
            return;
        case DETNetClient::HttpServeStatus::VersionOrAccountNotSupported:
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HasError, (uint32)EOnlineErrorCodePS4::VersionOrAccountNotSupported);
            return;
        case DETNetClient::HttpServeStatus::JsonParseFailed:
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HasError, (uint32)EOnlineErrorCodePS4::JsonParseFailed);
            return;
        case DETNetClient::HttpServeStatus::InMaintenance:
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HasError, (uint32)EOnlineErrorCodePS4::InMaintenance);
            return;
        case DETNetClient::HttpServeStatus::Ok:
            break;
        default:
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HasError, (uint32)EOnlineErrorCodePS4::OtherErrors);
            return;
        }

        DETNetClient& Net = DETNetClient::Instance();
        UPWProcedurePolicy* ProcedurePolicy = Cast<UPWProcedurePolicy>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_Policy));
        ProcedurePolicy->InitHttpAddr(Net.RealAddr.AgreementUrl, Net.RealAddr.PrivacyUrl);

        /*UPWSaveGameManager* SaveMgr = UPWSaveGameManager::Get(this);
        FString AdverID = SaveMgr->GetOtherData()->GetAdvertisingId();
        if (AdverID.IsEmpty()) {
            FGuid id;
            FPlatformMisc::CreateGuid(id);
            AdverID = id.ToString();
            SaveMgr->GetOtherData()->SetAdvertisingId(AdverID);
        }

        PWAnalytics* AnalyLog = PWAnalytics::Instance();
        AnalyLog->Init(Net.RealAddr.LogUrl, AdverID);
        AnalyLog->StartSession();
        FString Category = UPWProcedureBase::GetStringFromProcedure(ProcedureState::ProcedureState_Update);
        AnalyLog->LogEvent(Category, EAnalyticsAction::AA_GameStart);*/

        ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_HttpServeDown, (uint32)EOnlineErrorCodePS4::None);

        PullBulletin();
        StartRequestUpdateServer();
        }));
}


void UPWProcedureLoginPS4::StartRequestUpdateServer()
{
    DETNetClient& Net = DETNetClient::Instance();
    UpdateRootAddr = Net.RealAddr.UpdateUrl;
    PW_LOG(LogTemp, Log, TEXT("UPWProcedureLoginPS4::StartRequestUpdateServer UpdateRootAddr = %s"), *UpdateRootAddr);
    DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_StartUpdate, { ETrackFieldName::Url, UpdateRootAddr });
    auto GameConfig = GetInstanceOfJson<FPWGameDataConfig>();
    FString Url = UpdateRootAddr + GameConfig->UpdateListFileName;
    DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Get, Url, {}, [this, GameConfig](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
        if (ResultCode != EDHHttpResultCode::Ok)
        {
            PW_LOG(LogTemp, Error, TEXT("UPWProcedureLoginPS4::StartRequestUpdateServer Error, UpdateInfo HttpRequest failed, FileName = %s, ResultCode = %d"), *GameConfig->UpdateListFileName, (int32)ResultCode);
            DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndUpdate, { ETrackFieldName::Code, (int32)ResultCode });
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_UpdateServerDown, (uint32)EOnlineErrorCodePS4::HttpServeHasError);
            return;
        }
        FPWUpdateInfo UpdateInfo;
        if (DHJsonUtils::FromJson(Content, &UpdateInfo) == false)
        {
            PW_LOG(LogTemp, Error, TEXT("UPWProcedureLoginPS4::StartRequestUpdateServer Error, failed to deserialize updateinfo, content = %s"), *Content);
            DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndUpdate, { ETrackFieldName::Code, 2 });
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_UpdateServerDown, 0);
            return;
        }
        PW_LOG(LogTemp, Log, TEXT("UPWProcedureLoginPS4::StartRequestUpdateServer LocalVersion = %s RemoteVersion = %s, FileListSize = %d"), *GameConfig->Version, *UpdateInfo.Version, UpdateInfo.FileList.Num());
        if (UpdateInfo.Version.Equals(GameConfig->Version) == true || UpdateInfo.FileList.Num() == 0)
        {
            DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndUpdate, { ETrackFieldName::Code, 0 });
            ChangeLoginStatus(UPWProcedureLoginPS4::ELoginStatus::LS_UpdateServerDown, 0);
            return;
        }
        for (auto FileName : UpdateInfo.FileList)
        {
            FString TableUrl = UpdateRootAddr + FileName;
            UnfinishedHttpRequest++;
            DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Get, TableUrl, {}, [this, FileName](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
                if (ResultCode == EDHHttpResultCode::Ok)
                {
                    //表格更新
                    DHFiles::AddNewConfig(FileName, Content);
                }
                else
                {
                    PW_LOG(LogTemp, Error, TEXT("UPWProcedureLoginPS4::StartRequestUpdateServer Error, DataTable HttpRequest failed, FileName = %s, ResultCode = %d"), *FileName, (int32)ResultCode);
                }
                OnOneHttpRequestFinished();
                });
        }
        });
}
#endif
// Copyright 2016 P906, Perfect World, Inc.

#include "PWProcedureLogin.h"
#include "PWClientNet.h"
#include "PWGameSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PWUIManager.h"
#include "GameMapsSettings.h"
#include "Online/DHEndpointTracker.h"
#include "PWGameDataConfig.h"
#include "PWUpdateInfo.h"
#include "DHJsonUtils.h"
#include "PWVivoxConfig.h"
#include "DHPlatform.h"
#include "PWAssetManager.h"
#include "PWProcedureManager.h"
#include "Client/PWProcedurePolicy.h"


void UPWProcedureLogin::Enter()
{
	UPWClientNet::GetInstance(this)->Close();
	if (UPWGameSettings::Get()->ClientSettings.bShowDebugMessage)
	{
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("EnableAllScreenMessages"), GetWorld()->GetFirstPlayerController());
	}
	else
	{
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("DisableAllScreenMessages"), GetWorld()->GetFirstPlayerController());
	}
	UPWGameInstance::Get(this)->ShowLoadingScreen(true);
	StartRequestHttpServer();
	_LoginStatus = (ELoginStatus)((uint32)ELoginStatus::LS_LoadDown | (uint32)ELoginStatus::LS_LoginDown);	//地图加载在Update阶段做了,windows不需要先登录平台，所以默认跳过登录阶段
}

void UPWProcedureLogin::Leave()
{
	CloseBulletinsWidget();
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_LoginOld);
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Connecting);
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Queue);
}


void UPWProcedureLogin::ChangeLoginStatus(ELoginStatus LoginStatus, uint32 ErrorCode)
{
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureLogin::ChangeLoginStatus NewStatus = %u ErrorCode = %u"), (uint32)LoginStatus, ErrorCode);
	_LoginStatus = (ELoginStatus)((uint32)_LoginStatus | (uint32)LoginStatus);

	if (!((uint32)_LoginStatus & (uint32)ELoginStatus::LS_HttpServeDown))
		return;

	if (!((uint32)_LoginStatus & (uint32)ELoginStatus::LS_UpdateServerDown))
		return;

	if (!((uint32)_LoginStatus & (uint32)ELoginStatus::LS_BullentDown))	//表示数据还没拿到，只需等待公告数据返回就好
		return;

	PW_LOG(LogTemp, Display, TEXT("UPWProcedureLogin::LoginStatus = AllDown"));

	UPWGameInstance::Get(this)->HideLoadingScreen();
	UPWUIManager::Get(this)->Init();
	UPWAssetManager::Get(this)->Init();
	UPWUIManager::Get(this)->ForceShowUI(EUMGID::EUMGID_LoginOld);
	CheckBulletinMsg();
}

void UPWProcedureLogin::OnBulletinRefreshed()
{
	ChangeLoginStatus(UPWProcedureLoginBase::ELoginStatus::LS_BullentDown, 0);
}

void UPWProcedureLogin::StartRequestHttpServer()
{
	auto Config = GetInstanceOfJson<FPWGameDataConfig>();

	std::initializer_list<DHHttpField> Fields = {
		{ TEXT("platform"), DHPlatform::GetTargetName(true) },
		{ TEXT("area"), Config->AreaName },
		{ TEXT("ver"), Config->Version }
	};

	DETNetClient::Instance().BeginConnectV2(Config->HttpAddress, Fields, DETNetClient::FBeginConnectV2Delegate::CreateLambda([this](DETNetClient::HttpServeStatus ServeStatus) {
		FString ErrorMessage;

		switch (ServeStatus) {
		case DETNetClient::HttpServeStatus::HttpServeError:
			ErrorMessage = DH_LOCTABLE_STR("GL_MSG_SEVERFAILURE");
			break;
		case DETNetClient::HttpServeStatus::NetUnavailable:
			ErrorMessage = DH_LOCTABLE_STR("GL_MSG_NET_UNAVAILABLE");
			break;
		case DETNetClient::HttpServeStatus::JsonParseFailed:
			ErrorMessage = DH_LOCTABLE_STR("GL_MSG_SEVERLIST");
			break;
		case DETNetClient::HttpServeStatus::InMaintenance:
			ErrorMessage = DH_LOCTABLE_STR("GL_MSG_MAINTAIN");
			break;
		case DETNetClient::HttpServeStatus::Ok:
		{
			PullBulletin();
			StartRequestUpdateServer();
			DETNetClient& Net = DETNetClient::Instance();
			UPWProcedurePolicy* ProcedurePolicy = Cast<UPWProcedurePolicy>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_Policy));
			ProcedurePolicy->InitHttpAddr(Net.RealAddr.AgreementUrl, Net.RealAddr.PrivacyUrl);
			ChangeLoginStatus(UPWProcedureLoginBase::ELoginStatus::LS_HttpServeDown, 0);
			return;
		}
		default:
			ErrorMessage = DH_LOCTABLE_STR("GL_MSG_UNKNOWN_ERROR");
			break;
		}
		UPWGameInstance::Get(this)->UpdateLoadingScreenContentVisibility(false);
		UPWUIManager::Get(this)->ShowNoticeUI(ErrorMessage, [this] {
			ExecuteNextFrame([this] {StartRequestHttpServer(); });
			});
		}));

}

void UPWProcedureLogin::StartRequestUpdateServer()
{
	DETNetClient& Net = DETNetClient::Instance();
	UpdateRootAddr = Net.RealAddr.UpdateUrl;
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureLogin::StartRequestUpdateServer UpdateRootAddr = %s"), *UpdateRootAddr);
	DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_StartUpdate, { ETrackFieldName::Url, UpdateRootAddr });
	auto GameConfig = GetInstanceOfJson<FPWGameDataConfig>();
	FString Url = UpdateRootAddr + GameConfig->UpdateListFileName;
	DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Get, Url, {}, [this, GameConfig](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
		if (ResultCode != EDHHttpResultCode::Ok)
		{
			PW_LOG(LogTemp, Error, TEXT("UPWProcedureLogin::StartRequestUpdateServer Error, UpdateInfo HttpRequest failed, FileName = %s, ResultCode = %d"), *GameConfig->UpdateListFileName, (int32)ResultCode);
			DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndUpdate, { ETrackFieldName::Code, (int32)ResultCode });
			OnUpdateServerFinished();
			return;
		}
		FPWUpdateInfo UpdateInfo;
		if (DHJsonUtils::FromJson(Content, &UpdateInfo) == false)
		{
			PW_LOG(LogTemp, Error, TEXT("UPWProcedureLogin::StartRequestUpdateServer Error, failed to deserialize updateinfo, content = %s"), *Content);
			OnUpdateServerFinished();
			return;
		}
		PW_LOG(LogTemp, Log, TEXT("UPWProcedureLogin::StartRequestUpdateServer, LocalVersion = %s RemoteVersion = %s, FileListSize = %d"), *GameConfig->Version, *UpdateInfo.Version, UpdateInfo.FileList.Num());
		if (UpdateInfo.Version.Equals(GameConfig->Version) == true || UpdateInfo.FileList.Num() == 0)
		{
			OnUpdateServerFinished();
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
					PW_LOG(LogTemp, Error, TEXT("UPWProcedureLogin::StartRequestUpdateServer Error, DataTable HttpRequest failed, FileName = %s, ResultCode = %d"), *FileName, (int32)ResultCode);
				}
				UnfinishedHttpRequest--;
				if (UnfinishedHttpRequest == 0)
				{
					DHFiles::IncGlobalCfgVersion();
					OnUpdateServerFinished();
				}
				});
		}
		});
}

void UPWProcedureLogin::OnUpdateServerFinished()
{
	ChangeLoginStatus(UPWProcedureLoginBase::ELoginStatus::LS_UpdateServerDown, 0);
}

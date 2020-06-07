// Copyright 2004-2018 Perfect World Co.,Ltd. All Rights Reserved.

#include "Client/PWProcedurePolicy.h"
#include "PWUIManager.h"
#include "PWDelegateManager.h"
#include "PWProcedureManager.h"
#include "PWPromptBase.h"
#include "PWPromptManager.h"
#include "PWLoadConnectingWidget.h"
#include "PWOfflineNoticeWidget.h"
#include "Lobby/PWPolicyWidget.h"
#include "PWLogManager.h"

#include "HttpModule.h"
#include "HttpRetrySystem.h"
#include "IHttpRequest.h"
#include "IHttpResponse.h"
#include "PlatformHttp.h"
#include "PWClientNet.h"
#include "PWNetDefine.h"

FPWPolicyCheck::FPWPolicyCheck()
{
	WorldContext = nullptr;
}

FPWPolicyCheck::FPWPolicyCheck(UObject* inWorldContext, int32 inFlag)
{
	WorldContext = inWorldContext;
	Flag = inFlag;
}

FPWPolicyCheck::~FPWPolicyCheck()
{
	delete WorldContext;
	WorldContext = nullptr;
}

void FPWPolicyCheck::Start2GetVersion(const FString& addr)
{
	TSharedRef<class IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FPWPolicyCheck::Query_HttpRequestComplete);
	HttpRequest->SetURL(*addr);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();

	FString strFlag = Flag == player_flag32_user_agreement_version ? TEXT("UserAgreement") : TEXT("PrivacyPolicy");
	PW_LOG(LogTemp, Log, TEXT("FPWPolicyCheck::Start2GetVersion, Flag = %s, Addr = %s"), *strFlag, *addr);
}

void FPWPolicyCheck::Query_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	int32 version = 0;
	FString ResponseStr;

	if (bSucceeded &&
		HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			PW_LOG(LogTemp, Log, TEXT("[Query_HttpRequestComplete]: succeed, flag = %d url=%s code=%d response=%s"), Flag, *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			// 			TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(ResponseStr);
			// 			TSharedPtr<FJsonObject> JsonObject;
			// 			if (FJsonSerializer::Deserialize(JsonReader, JsonObject) == false)
			// 			{
			// 				return;
			// 			}
			// 			int32 version = JsonObject->GetIntegerField(TEXT("version"));
			LexFromString(version, *ResponseStr);
		}
		else
		{
			PW_LOG(LogTemp, Log, TEXT("[Query_HttpRequestComplete]: failed, flag = %d url=%s code=%d response=%s"), Flag, *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			//UPWUIManager::Get(WorldContext)->ShowErrorUI(TEXT("ERROR_Http"), HttpResponse->GetResponseCode(), HttpRequest->GetURL());
		}
	}
	else
	{
		PW_LOG(LogTemp, Log, TEXT("[Query_HttpRequestComplete]: failed, flag = %d url=%s "), Flag, *HttpRequest->GetURL());
		//UPWUIManager::Get(WorldContext)->ShowErrorUI(TEXT("ERROR_Http"), Flag, HttpRequest->GetURL());
	}

	UPWDelegateManager::Get(WorldContext)->GetPolicyVersionCallBack.Broadcast(Flag, version);
}


////////////////////////////UPWProcedurePolicy//////////////////////////////////////////////
ProcedureState UPWProcedurePolicy::GetState()
{
	return ProcedureState::ProcedureState_Policy;
}

void UPWProcedurePolicy::Enter()
{
#if !PLATFORM_PS4
	UPWProcedureManager::GetInstance(this)->ChangeCurState(NextState);
	return;
#endif

	TWeakObjectPtr<UPWUIManager> WeakUIManager(UPWUIManager::Get(this));
	FPWUIOpenDelegate UIOpenDelegate = FPWUIOpenDelegate::CreateLambda([WeakUIManager]() {
		if (WeakUIManager.IsValid())
		{
			UPWLoadConnectingWidget* ConnectingWidget = Cast<UPWLoadConnectingWidget>(WeakUIManager.Get()->GetWidget(EUMGID::EUMGID_Connecting));
			if (ConnectingWidget)
			{
				ConnectingWidget->InitType(eLoadConnectType::eLoadConnectType_Login);
			}
		}
		});
	WeakUIManager->ShowUI(EUMGID::EUMGID_Connecting, UIOpenDelegate);
	WeakUIManager->PreLoadUI(EUMGID::EUMGID_UserAgreement);
	WeakUIManager->PreLoadUI(EUMGID::EUMGID_PrivacyPolicy);

	CheckPolicyVersion();
}

void UPWProcedurePolicy::CheckPolicyVersion()
{
	if (NetUserAgreementVersion == -1 || NetPrivacyPolicyVersion == -1)
	{
		return;	//等两个Version都到了再统一Check
	}
	//check whether netversion init failed
	{
		bool bNetVersionInitFailed = false;
		if (NetUserAgreementVersion == 0)
		{
			PW_LOG(LogTemp, Warning, TEXT("UPWProcedurePolicy::CheckPolicyVersion Get UserAgreement version from url failed."));
			bNetVersionInitFailed = true;
		}
		if (NetPrivacyPolicyVersion == 0)
		{
			PW_LOG(LogTemp, Warning, TEXT("UPWProcedurePolicy::CheckPolicyVersion Get PrivacyPolicy version from url failed."));
			bNetVersionInitFailed = true;
		}
		if (bNetVersionInitFailed == true)
		{
			UPWUIManager::Get(this)->ShowOfflineReason("GL_MSG_FWEB");
			return;
		}
	}

	//version check
	{
		if (NetUserAgreementVersion != UserAgreementVersion)
		{
			TWeakObjectPtr<UPWUIManager> WeakUIManager = UPWUIManager::Get(this);
			FPWUIOpenDelegate UIOpenDel = FPWUIOpenDelegate::CreateLambda([WeakUIManager]() {
				WeakUIManager->HideUI(EUMGID::EUMGID_Connecting);
				UPWPolicyWidget* widget = Cast<UPWPolicyWidget>(WeakUIManager->GetWidget(EUMGID::EUMGID_UserAgreement));
				if (widget)
				{
					widget->SetFlag(player_flag32_user_agreement_version);
				}
				});
			WeakUIManager->ShowUI(EUMGID::EUMGID_UserAgreement, UIOpenDel);
			return;
		}
		if (NetPrivacyPolicyVersion != PrivacyPolicyVersion)
		{
			TWeakObjectPtr<UPWUIManager> WeakUIManager = UPWUIManager::Get(this);
			FPWUIOpenDelegate UIOpenDel = FPWUIOpenDelegate::CreateLambda([WeakUIManager]() {
				WeakUIManager->HideUI(EUMGID::EUMGID_Connecting);
				UPWPolicyWidget* widget = Cast<UPWPolicyWidget>(WeakUIManager->GetWidget(EUMGID::EUMGID_PrivacyPolicy));
				if (widget)
				{
					widget->SetFlag(player_flag32_privacy_version);
				}
				});
			UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_PrivacyPolicy, UIOpenDel);
			return;
		}
	}
	//check completed
	UPWProcedureManager::GetInstance(this)->ChangeCurState(NextState);
}

void UPWProcedurePolicy::Leave()
{
}

void UPWProcedurePolicy::InitHttpAddr(const FString& AgreementAddr, const FString& PrivacyAddr)
{
	if (m_pAgreementCheck == nullptr)
	{
		m_pAgreementCheck = new FPWPolicyCheck(this, player_flag32_user_agreement_version);
	}
	m_pAgreementCheck->Start2GetVersion(AgreementAddr);

	if (m_pPrivacyCheck == nullptr)
	{
		m_pPrivacyCheck = new FPWPolicyCheck(this, player_flag32_privacy_version);
	}
	m_pPrivacyCheck->Start2GetVersion(PrivacyAddr);

	UPWDelegateManager::Get(this)->GetPolicyVersionCallBack.AddUniqueDynamic(this, &UPWProcedurePolicy::OnReceiveVersion);
}

void UPWProcedurePolicy::SetPolicyVersion2Server(int32 flag)
{
	int32 Version = 0;
	if (flag == player_flag32_user_agreement_version)
	{
		UserAgreementVersion = NetUserAgreementVersion;
		if (NextState != ProcedureState::ProcedureState_CreateChar)
		{
			UPWClientNet::GetInstance(this)->ModifyPlayerFlagValue(flag, UserAgreementVersion);
		}
	}
	else if (flag == player_flag32_privacy_version)
	{
		PrivacyPolicyVersion = NetPrivacyPolicyVersion;
		if (NextState != ProcedureState::ProcedureState_CreateChar)
		{
			UPWClientNet::GetInstance(this)->ModifyPlayerFlagValue(flag, PrivacyPolicyVersion);
		}
	}
	CheckPolicyVersion();
}

void UPWProcedurePolicy::OnReceiveVersion(int32 flag, int32 version)
{
	if (flag == player_flag32_user_agreement_version)
	{
		NetUserAgreementVersion = version;
	}
	else if (flag == player_flag32_privacy_version)
	{
		NetPrivacyPolicyVersion = version;
	}
	if (UPWProcedureManager::GetInstance(this)->GetCurState() == ProcedureState::ProcedureState_Policy)
	{
		CheckPolicyVersion();
	}
}

void UPWProcedurePolicy::SetNextState(ProcedureState inNextState)
{
	NextState = inNextState;
}

ProcedureState UPWProcedurePolicy::GetNextState()
{
	return NextState;
}

void UPWProcedurePolicy::SetPolicyVersionFromServer(int32 flag, int32 version)
{
	if (flag == player_flag32_user_agreement_version)
	{
		UserAgreementVersion = version;
	}
	else
	{
		PrivacyPolicyVersion = version;
	}
}
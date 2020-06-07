#include "DETNetClient.h"
#include "DETNetMsgEvent.h"
#include "DHHttpClient.h"
#include "DHScopeOut.h"
#include "DHJsonUtils.h"

void DETNetClient::OnConnected()
{
	UE_LOG(LogDETNet, Log, TEXT("OnConnected %s,%d"), *Url, State);
	switch (State)
	{
	case sPlayer_ConnectLogin:
	{
		SetState(sPlayer_VerifyLogin);
		FCMD_C2S_VerifyReq req;
		req.Acct = Acct;
		req.Token = Token;
		Send(req);
	}break;
	case sPlayer_ConnectEntry:
	{
		SetState(sPlayer_VerifyEntry);
		FCMD_C2S_VerifyOkeyReq req;
		req.CharId = CharId;
		req.Okey = Okey;
		req.Acct = Acct;
		req.Ver = Ver;
		Send(req);
	}break;
	case sPlayer_ConnectLobby:
	{
		SetState(sPlayer_VerifyLobby);
		FCMD_C2S_LoginReq req;
		req.CharId = CharId;
		req.Pwd = Pwd;
		req.IsPlus = PlusAccess;
		req.Lang = Language;
		Send(req);
	}break;
	}
}
void DETNetClient::OnConnectionError(const FString& Error)
{
	UE_LOG(LogDETNet, Log, TEXT("OnConnectionError %s,%s"), *Url, *Error);
	int DropType = DropType_Normal;
	switch (State)
	{
	case sPlayer_ConnectLogin:
		DropType = DropType_ConnectLoginFailed;
	case sPlayer_ConnectEntry:
		DropType = DropType_ConnectEntryFailed;
	case sPlayer_ConnectLobby:
		DropType = DropType_ConnectLobbyFailed;
	}
	DETNetMsgEvent::EmitDrop(DropType);
	SetState(sPlayer_Normal);
}
void DETNetClient::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogDETNet, Log, TEXT("OnClosed %s,%d,%s,%d"), *Url, StatusCode, *Reason, int(bWasClean));
	switch (State)
	{
	case sPlayer_WaitLoginClose:
	{
		SetState(sPlayer_ConnectEntry);
		Clear();
		DETNetWS::Connect(Url);
	}break;
	case sPlayer_WaitEntryClose:
	{
		SetState(sPlayer_ConnectLobby);
		Clear();
		DETNetWS::Connect(Url);
	}break;
	default:	//非正常关闭，应该是掉线了
		if (State != sPlayer_Closing)//非主动关闭，需要通知到关闭事件
		{
			DETNetMsgEvent::EmitDrop(DropType_Normal);
		}
		SetState(sPlayer_Normal);
		break;
	}
}
void DETNetClient::OnTick(float DeltaTime)
{
}

DETNetClient DETNetClient::Ins;

DETNetClient& DETNetClient::Instance()
{
	return Ins;
}

DETNetClient::DETNetClient()
	: CharId(0)
	, State(sPlayer_Normal)
{
}
void DETNetClient::SetState(int _State)
{
	UE_LOG(LogDETNet, Log, TEXT("SetState %d,%d"), State, _State);
	State = _State;
}

FString DETNetClient::GetLogAddress()
{
	return RealAddr.LogUrl;
}

TArray<FAddrObj>& DETNetClient::GetAreaList()
{
	return RealAddr.Addrs;
}

void DETNetClient::Close()
{
	if (State != sPlayer_Normal && State != sPlayer_Closing)
	{
		SetState(sPlayer_Closing);
		DETNetWS::Close();
	}
}

void DETNetClient::Disconnect()
{
	DETNetWS::Close();
}

void DETNetClient::BeginConnectV2(const FString& _Url, const std::initializer_list<DHHttpField>& fields, FBeginConnectV2Delegate Delegate)
{
	DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Post, _Url, fields, [this, Delegate](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
		HttpServeStatus ServeStatus = HttpServeStatus::Ok;
		on_scope_out{
			Delegate.ExecuteIfBound(ServeStatus);
		};

		switch (ResultCode) {
		case EDHHttpResultCode::Ok:
			break;
		case EDHHttpResultCode::HttpLostContextError:
			ServeStatus = HttpServeStatus::NetUnavailable;
			return;
		case EDHHttpResultCode::HttpRequestError:
		case EDHHttpResultCode::HttpResponseError:
		default:
			ServeStatus = HttpServeStatus::HttpServeError;
			return;
		}

		if (!DHJsonUtils::FromJson(Content, &RealAddr)) {
			ServeStatus = HttpServeStatus::JsonParseFailed;
			return;
		}

		switch (RealAddr.State) {
		case 1:     // 正常状态，可以登录
			break;
		case 2:     // 系统维护
			ServeStatus = HttpServeStatus::InMaintenance;
			return;
		case 0:
			ServeStatus = HttpServeStatus::VersionOrAccountNotSupported;
			return;
		default:
			ServeStatus = HttpServeStatus::HttpServeError;
			return;
		}

		if (RealAddr.AgreementUrl.IsEmpty() || RealAddr.PrivacyUrl.IsEmpty() || RealAddr.Addrs.Num() == 0 || RealAddr.Addrs[0].Addr.IsEmpty()) {
			ServeStatus = HttpServeStatus::VersionOrAccountNotSupported;
			return;
		}

		UserAgreementUrl = RealAddr.AgreementUrl;
		PrivacyPolicyUrl = RealAddr.PrivacyUrl;
		LoginUrl = RealAddr.Addrs[0].Addr;
		Url = LoginUrl;
		UpdateUrl = RealAddr.UpdateUrl;
		});
}

void DETNetClient::ConnectLogin(const FString& _Acct, const FString& _Token, bool _PlusAccess, const FString& _Language)
{
	SetState(sPlayer_ConnectLogin);
	Acct = _Acct;
	Token = _Token;
	PlusAccess = _PlusAccess;
	Language = _Language;
	Connect(LoginUrl);
}
void DETNetClient::ConnectEntry(const FString& _Url, const int64 _CharId, const FString& _Acct, const FString& _Okey)
{
	SetState(sPlayer_WaitLoginClose);
	DETNetWS::Close();
	Url = _Url;
	CharId = _CharId;
	Okey = _Okey;
	Acct = _Acct;
}
void DETNetClient::ConnectLobby(const FString& _Url, const FString& _Pwd)
{
	SetState(sPlayer_WaitEntryClose);
	DETNetWS::Close();
	Url = _Url;
	Pwd = _Pwd;
}

void DETNetClient::PullBulletin(const FString Culture)
{
	FString Url = RealAddr.AnnouncementUrl;
	if (Url.Len() == 0)
	{
		Url = DefaultAnnouncementUrl;
	}
	UE_LOG(LogDETNet, Log, TEXT("DETNetClient::PullBulletin using Url: %s"), *Url);
	if (Url.Len() == 0)
	{
		return;
	}
	Url += TEXT("/announcement/notice");
	std::initializer_list<DHHttpField> Fields = {
		{ TEXT("opt"), TEXT("list") },
		{ TEXT("acct"),  Acct },
		{ TEXT("type"), TEXT("1") },
		{ TEXT("lang"),  Culture },
		{ TEXT("ver"),  Ver },
	};
	DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Post, Url, Fields, [this](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
		if (ResultCode != EDHHttpResultCode::Ok || !DHJsonUtils::FromJson(Content, &BulletinList))
		{
			BulletinList.List.Empty();
		}
		DETNetMsgEvent::EmitBulletin();
		UE_LOG(LogDETNet, Log, TEXT("PullBulletin Result = %d, Content = %s"), (int32)ResultCode, *Content);
		});
}

void DETNetClient::PullAdvanceNotice(const FString Culture)
{
	FString Url = RealAddr.AnnouncementUrl;
	if (Url.Len() == 0)
	{
		Url = DefaultAnnouncementUrl;
	}
	UE_LOG(LogDETNet, Log, TEXT("DETNetClient::PullAdvanceNotice using Url: %s"), *Url);
	if (Url.Len() == 0)
	{
		return;
	}
	Url += TEXT("/announcement/preview");
	std::initializer_list<DHHttpField> Fields = {
		{ TEXT("opt"), TEXT("list") },
		{ TEXT("acct"),  Acct },
		{ TEXT("type"), TEXT("3") },
		{ TEXT("lang"),  Culture },
		{ TEXT("ver"),  Ver },
	};
	DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Post, Url, Fields, [this](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
		if (ResultCode != EDHHttpResultCode::Ok || !DHJsonUtils::FromJson(Content, &AdvanceNoticeList))
		{
			AdvanceNoticeList.List.Empty();
		}
		DETNetMsgEvent::EmitAdvanceNotice();
		UE_LOG(LogDETNet, Log, TEXT("PullAdvanceNotice Result = %d, Content = %s"), (int32)ResultCode, *Content);
		});
}

void DETNetClient::PullStopService(const FString Culture)
{
	FString Url = RealAddr.AnnouncementUrl;
	if (Url.Len() == 0)
	{
		Url = DefaultAnnouncementUrl;
	}
	UE_LOG(LogDETNet, Log, TEXT("DETNetClient::PullStopService using Url: %s"), *Url);
	if (Url.Len() == 0)
	{
		return;
	}
	Url += TEXT("/announcement/broadcast");
	std::initializer_list<DHHttpField> Fields = {
		{ TEXT("opt"), TEXT("list") },
		{ TEXT("acct"),  Acct },
		{ TEXT("type"), TEXT("2") },
		{ TEXT("lang"),  Culture },
		{ TEXT("ver"),  Ver },
	};
	DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Post, Url, Fields, [this](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
		if (ResultCode != EDHHttpResultCode::Ok || !DHJsonUtils::FromJson(Content, &StopServiceList))
		{
			StopServiceList.List.Empty();
		}
		DETNetMsgEvent::EmitStopService();
		UE_LOG(LogDETNet, Log, TEXT("PullStopService Result = %d, Content = %s"), (int32)ResultCode, *Content);
		});
}


void DETNetClient::PullBattlePassBulletin(const FString Culture)
{
	FString Url = RealAddr.AnnouncementUrl;
	if (Url.Len() == 0)
	{
		Url = DefaultAnnouncementUrl;
	}
	UE_LOG(LogDETNet, Log, TEXT("DETNetClient::PullBattlePassBulletin using Url: %s"), *Url);
	if (Url.Len() == 0)
	{
		return;
	}
	Url += TEXT("/announcement/battlepass");
	std::initializer_list<DHHttpField> Fields = {
		{ TEXT("opt"), TEXT("list") },
		{ TEXT("acct"),  Acct },
		{ TEXT("type"), TEXT("4") },
		{ TEXT("lang"),  Culture },
		{ TEXT("ver"),  Ver },
	};
	DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Post, Url, Fields, [this](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
		if (ResultCode != EDHHttpResultCode::Ok || !DHJsonUtils::FromJson(Content, &BattlePassBulletinList))
		{
			BattlePassBulletinList.List.Empty();
		}
		DETNetMsgEvent::EmitBattlePassBulletin();
		UE_LOG(LogDETNet, Log, TEXT("BattlePassPullBulletin Result = %d, Content = %s"), (int32)ResultCode, *Content);
		});
}

void DETNetClient::SetDefaultAnnounceUrl(const FString DefaultUrl, FString Version)
{
	DefaultAnnouncementUrl = DefaultUrl;
	Ver = Version;
}
const int64 DETNetClient::GetTime()
{
	int Seconds;
	float PartialSeconds;
	UGameplayStatics::GetAccurateRealTime(nullptr, Seconds, PartialSeconds);
	return SrvTimestamp + (int64(Seconds * 1000 + PartialSeconds * 1000) - RcvSrvTime);
}

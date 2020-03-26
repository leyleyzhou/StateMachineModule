#pragma once
#include "DETNetWS.h"
#include "DETNetPlayerData.h"
#include "Delegates/DelegateCombinations.h"
#include "OnlineError.h"
#include "DHHttpClient.h"

//玩家状态枚举
enum
{
	sPlayer_Normal,
	sPlayer_ConnectLogin,		//连接login
	sPlayer_VerifyLogin,		//向login请求验证
	sPlayer_WaitLoginClose,
	sPlayer_ConnectEntry,		//连接entry
	sPlayer_VerifyEntry,		//向entry请求验证登录
	sPlayer_WaitEntryClose,
	sPlayer_ConnectLobby,		//连接lobby
	sPlayer_VerifyLobby,		//向lobby请求验证登录
	sPlayer_EnterLobby,			//登录成功
	sPlayer_Closing,			//关闭中
};
class DETNET_EXPORT DETNetClient : public DETNetWS
{
	DETNetClient();
	static DETNetClient Ins;
public:
	static DETNetClient& Instance();

	enum class HttpServeStatus
	{
		Ok,                 // 一切正常
		InMaintenance,      // 维护中
		HttpServeError,     // Http服务错误
		NetUnavailable,     // 网络错误
		JsonParseFailed,    // Json解析错误 
		VersionOrAccountNotSupported
	};

private:
	void OnConnected()override;
	void OnConnectionError(const FString& Error)override;
	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)override;
	void OnTick(float DeltaTime)override;
private:
	//网络都是异步的，关闭也是异步的，所以每个连接都弄独立的
	FString Acct;
	FString Token;
	bool PlusAccess = false;
	int64 CharId;
	FString Okey;
	FString Ver;
	FString Pwd;
	FString LoginUrl;
	FString Url;
	FString HttpUrl;
	FString Language;

public:
	int State;					//状态
	PlayerData Data;			//数据
	FString UpdateUrl;			//更新地址
	FString UserAgreementUrl;
	FString PrivacyPolicyUrl;
	FClientAddrObj RealAddr;
	FClientBulletinList BulletinList; // 公告列表
	FClientBulletinList AdvanceNoticeList; // 预告列表
	FClientBulletinList StopServiceList; // 停服广播列表
	FClientBulletinList BattlePassBulletinList;//BattlePass 公告列表
	FString DefaultAnnouncementUrl;

	TMap<FString, FCfgObj> CfgMap;
	FString GetLogAddress();
	TArray<FAddrObj>& GetAreaList();

	FString GetVer() { return Ver; }

	int64 SrvTimestamp;		//服务器发送过来的时间戳
	int64 RcvSrvTime;		//服务器发送时间戳时本地时间
public:
	void Disconnect();
	void Close();
	void SetState(int _State);
	void SetLoginUrl(const FString& Url)
	{
		LoginUrl = Url;
	}
	DECLARE_MULTICAST_DELEGATE_OneParam(FBeginConnectV2, HttpServeStatus);
	typedef FBeginConnectV2::FDelegate FBeginConnectV2Delegate;
	void BeginConnectV2(const FString& _Url, const std::initializer_list<DHHttpField>& fields, FBeginConnectV2Delegate Delegate);
	void ConnectLogin(const FString& _Acct, const FString& _Token, bool _PlusAccess, const FString& _Language);
	void ConnectEntry(const FString& _Url, const int64 _CharId, const FString& _Acct, const FString& _Okey);
	void ConnectLobby(const FString& _Url, const FString& _Pwd);

	void PullBulletin(const FString Culture);
	void PullAdvanceNotice(const FString Culture);
	void PullStopService(const FString Culture);
	void PullBattlePassBulletin(const FString Culture);
	void SetDefaultAnnounceUrl(const FString DefaultUrl, FString Version);

	const int64 GetCharId()
	{
		return CharId;
	}
	const int64 GetTime();
};

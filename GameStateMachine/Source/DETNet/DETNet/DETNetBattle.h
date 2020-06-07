#pragma once
#include "DETNetCmn.h"
#include "DETNetTcp.h"
#include "DETNetStruct.h"
#include "DETNetClient.h"

class DETNET_EXPORT DETNetBattle :public DETNetTcp
{
	DETNetBattle();
	static DETNetBattle Ins;
public:
	virtual ~DETNetBattle();
	static DETNetBattle& Instance();

	int Init(const FString& matchAddr, int SrvId, int MapId, const FString& WelcomeAddr, int Port, bool bGameLift);
	TMap<int64, PlayerData> Players;
	int64 BattleId;
private:
	FString _matchAddr;
	bool _bGameLift;
	int  _port;
	FCMD_Server2Server_SayHello HelloCmd;

	virtual void OnConnected();
	virtual void OnConnectionError(const FString& Error);
	virtual void OnClosed(bool Initiative);

	void Exit();
#if UE_SERVER
	void InitGameLiftSDK(unsigned short Port);
	void ProcessEnding();
#endif // UE_SERVER

};
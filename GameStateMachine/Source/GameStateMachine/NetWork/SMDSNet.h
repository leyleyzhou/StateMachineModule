// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "DHCommon.h"
#include "Ticker.h"
#include "DETNet/DETNet/DETNetPlayerData.h"
#include "PWGameInstance.h"
#include "PWDSNet.generated.h"

UCLASS()
class PWGAME_API UPWDSNet : public UObject, public DHGameInstance<UPWDSNet>
{
	GENERATED_BODY()
public:
	UPWDSNet() {};

	~UPWDSNet() {};

public:
	void DealSayHelloRsp(const FString& Data);
	void DealMatchAllocBattleReq(const FString& Data);
	void DealMatchPlayerInReq_GetDecoded(const FString& Data);
	void DealMatchPlayerOutReq(const FString& Data);
	void DealMatchCutDown(const FString& Data);

	void Init();
	void Tick(float DeltaTime);
	void Exit();

	// Send packets
public:
	void AllocBattleRsp();
	void PlayerInRsp(int32 Rst, uint64 CharID);
	void PlayerOutRsp(int32 Rst, uint64 CharID);
	void SendBeginFightNotify(int nMaxTimeSeconds);
	void SendCanWorkNotify();
	void PlayerConnect(uint64 CharID);
	void PlayerLeaveReq(uint64 CharID);
	void PlayerRewardReq(uint64 CharID, FFightResult& ret, FFightTeamInfo& TeamFightInfo);
	void SendSetAchievement(const int64 nCharID, int32 tableId, int32 addNum, int32 exData = 0);

	//Process packets;
public:
	bool OnPlayerInReq(FString token, int64 TeamID, bool IsLeader, int32 CharacterType, PlayerData& PlayerData);
	bool OnPlayerOutReq(uint64 nCharID);
	void OnAllocBattleReq(uint64 nBattleID, int32 nMatchType, int64 TimeStamp);
	void OnCountDownUpdated(int32 nSeconds);

	void OnRobotInReq(TArray<FRobot> Robots);

private:
	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;
};
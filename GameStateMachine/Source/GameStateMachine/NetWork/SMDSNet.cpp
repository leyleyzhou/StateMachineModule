#include "PWDSNet.h"
#include "PWProcedureManager.h"
#include "DS/PWProcedureBattleDS.h"
#include "PWCharacterManager.h"
#include "PWLibrary.h"
#include "GameMode/PWGameMode.h"
#include "GameMode/PWGameModeWereWolf.h"
#include "PWGameSettings.h"
#include "DETNet/DETNet/DETNetStruct.h"
#include "DETNet/DETNet/DETNetMsgEvent.h"
#include "DETNet/DETNet/DETNetBattle.h"
#include "DETNet/DETNet/DETNetPlayerData.h"
#include "PWGameState.h"
#include "IpNetDriver.h"
#include "IPAddress.h"
#include "GenericPlatformMisc.h"


void UPWDSNet::DealSayHelloRsp(const FString& Data)
{
	UE_LOG(LogTemp, Log, TEXT("DealSayHelloRsp : %s"), *Data);
	FCMD_Server2Server_SayHelloRsp Rsp;
	if (Data.Len() > 0)
		DHJsonUtils::FromJson(Data, &Rsp);

	if (Rsp.Result != 0)
	{
		FGenericPlatformMisc::RequestExit(false);
	}

}

void UPWDSNet::DealMatchAllocBattleReq(const FString& Data)
{
	FCMD_Match2Battle_AllocBattleReq req(Data);
	OnAllocBattleReq(req.BattleId, req.MatchType, req.CurTime);
	OnRobotInReq(req.Robots);
}

void UPWDSNet::DealMatchPlayerInReq_GetDecoded(const FString& Data)
{
	FCMD_Match2Battle_PlayerInReq req(Data);
	PlayerData& Player = DETNetBattle::Instance().Players.FindOrAdd(req.CharId);
	Player.CharId = req.CharId;
	OnPlayerInReq(req.Key, req.GroupId, req.Leader, req.RoleType, Player);
}

void UPWDSNet::DealMatchPlayerOutReq(const FString& Data)
{
	FCMD_Match2Battle_PlayerOutReq req(Data);
	OnPlayerOutReq(req.CharId);
}

void UPWDSNet::DealMatchCutDown(const FString& Data)
{
	FCMD_Match2Battle_CutdownNofity req(Data);
	OnCountDownUpdated(req.Seconds);
}

void UPWDSNet::Init()
{
#if WITH_EDITOR
	if (UPWGameSettings::Get()->MatchSetting.bPlayWithLobby == false)
		return;
#endif

	DETNetMsgEvent::Init();

	DETNetMsgEvent::Reg(Server2Server_SayHelloRsp, [this](const FString& Data) {
		DealSayHelloRsp(Data);
		});
	DETNetMsgEvent::Reg(Match2Battle_AllocBattleReq, [this](const FString& Data) {
		DealMatchAllocBattleReq(Data);
		});
	DETNetMsgEvent::Reg(Match2Battle_PlayerInReq, [this](const FString& Data) {
		DealMatchPlayerInReq_GetDecoded(Data);
		});
	DETNetMsgEvent::Reg(Match2Battle_PlayerOutReq, [this](const FString& Data) {
		DealMatchPlayerOutReq(Data);
		});
	DETNetMsgEvent::Reg(Match2Battle_CutdownNofity, [this](const FString& Data) {
		DealMatchCutDown(Data);
		});

	int32 SrvId;
	int32 MapId;
	int32 port;
	{
		//init port
		UIpNetDriver* IpNetDriver = Cast<UIpNetDriver>(GetWorld()->GetNetDriver());
		UE_CLOG(IpNetDriver == nullptr, LogTemp, Error, TEXT("UPWDSNet::Init failed NetDriver isnot UIpNetDriver"));
		port = IpNetDriver->LocalAddr->GetPort();
	}

	FString WelcomeAddr;
	FString MatchAddr;
	bool bUseGameLift = FParse::Param(FCommandLine::Get(), TEXT("gamelift"));
	FParse::Value(FCommandLine::Get(), TEXT("ServerID="), SrvId);
	FParse::Value(FCommandLine::Get(), TEXT("MapID="), MapId);
	/*FParse::Value(FCommandLine::Get(), TEXT("port="), port);*/
	FParse::Value(FCommandLine::Get(), TEXT("ip="), WelcomeAddr);
	FParse::Value(FCommandLine::Get(), TEXT("MatchAddr="), MatchAddr);
	WelcomeAddr = WelcomeAddr + TEXT(":") + FString::FromInt(port);
	DETNetBattle::Instance().Init(MatchAddr, SrvId, MapId, WelcomeAddr, port, bUseGameLift);
	TickDelegate = FTickerDelegate::CreateLambda([this](float DeltaTime) {
		this->Tick(DeltaTime);
		return true;
		});
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void UPWDSNet::Tick(float DeltaTime)
{
	DETNetBattle::Instance().Tick(DeltaTime);
}

void UPWDSNet::Exit()
{
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::Exit exec"));
#if WITH_EDITOR
	if (UPWGameSettings::Get()->MatchSetting.bPlayWithLobby == false)
		return;
#endif

	DETNetBattle::Instance().Close();
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UPWDSNet::AllocBattleRsp()
{
	FCMD_Battle2Match_AllocBattleRsp data;
	data.BattleId = DETNetBattle::Instance().BattleId;
	data.Result = 0;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::AllocBattleRsp BattleId: %llu"), data.BattleId);
}

void UPWDSNet::PlayerInRsp(int32 Rst, uint64 CharID)
{
	FCMD_Battle2Match_PlayerInRsp data;
	data.BattleId = DETNetBattle::Instance().BattleId;
	data.Result = Rst;
	data.CharId = CharID;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::PlayerInRsp Rst = %d, CharID = %llu"), Rst, CharID);
}

void UPWDSNet::PlayerOutRsp(int32 Rst, uint64 CharID)
{
	FCMD_Battle2Match_PlayerOutRsp data;
	data.Result = Rst;
	data.CharId = CharID;
	data.BattleId = DETNetBattle::Instance().BattleId;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::PlayerOutRsp Rst = %d, CharID = %llu"), Rst, CharID);
}

void UPWDSNet::SendBeginFightNotify(int nMaxTimeSeconds)
{
	FCMD_Battle2Match_BeginFightNotify data;
	data.BattleId = DETNetBattle::Instance().BattleId;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::SendBeginFightNotify BattleId: %llu"), data.BattleId);
}

void UPWDSNet::SendCanWorkNotify()
{
	FCMD_Battle2Match_CanWorkNotify data;
	data.BattleId = DETNetBattle::Instance().BattleId;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::SendCanWorkNotify BattleId: %llu"), data.BattleId);
}

void UPWDSNet::PlayerConnect(uint64 CharID)
{
	FCMD_Battle2Match_PlayerConnectNotify data;
	data.CharId = CharID;
	data.BattleId = DETNetBattle::Instance().BattleId;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::PlayerConnect CharId:%llu BattleId: %llu"), data.CharId, data.BattleId);
}

void UPWDSNet::PlayerLeaveReq(uint64 CharID)
{
	FCMD_Battle2Match_PlayerDisconnectNotify data;
	data.CharId = CharID;
	data.BattleId = DETNetBattle::Instance().BattleId;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::PlayerLeaveReq CharId:%llu BattleId: %llu"), data.CharId, data.BattleId);
}

void UPWDSNet::PlayerRewardReq(uint64 CharID, FFightResult& ret, FFightTeamInfo& TeamFightInfo)
{
	FCMD_Battle2Match_PlayerResultReq data;
	data.CharId = CharID;
	data.BattleId = DETNetBattle::Instance().BattleId;
	data.Result = ret;
	data.TeamInfo = TeamFightInfo;
	DETNetBattle::Instance().Send(data);
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::PlayerRewardReq CharId:%llu BattleId: %llu"), data.CharId, data.BattleId);
	for (auto& BH : ret.Behaviours)
	{
		UE_LOG(LogTemp, Log, TEXT("=== Action = %s, Value = %d"), *BH.Action, BH.Value);
	}
}

void UPWDSNet::SendSetAchievement(const int64 nCharID, int32 tableId, int32 addNum, int32 exData)
{
	//Todo by wangxu
	UE_LOG(LogTemp, Error, TEXT("UPWDSNet::SendSetAchievement undo"));
}

bool UPWDSNet::OnPlayerInReq(FString token, int64 TeamID, bool IsLeader, int32 CharacterType, PlayerData& PlayerData)
{
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::OnPlayerInReq token = %s, name = %s, charid = %llu, teamID = %llu CharacterType = %d"), *token, *PlayerData.Name, PlayerData.CharId, TeamID, CharacterType);
	UPWCharacterManager::Get(this)->AddCharacter(token, TeamID, IsLeader, CharacterType, &PlayerData);
	PlayerInRsp(0, PlayerData.CharId);
	return true;
}

bool UPWDSNet::OnPlayerOutReq(uint64 nCharID)
{
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::OnPlayerOutReq Received, nCharID = %llu"), nCharID);
	APWCharacterData_Runtime* Runtime = UPWCharacterManager::Get(this)->GetCharDataByID(nCharID);
	if (Runtime)
	{
		UPWCharacterManager::Get(this)->OnCharKicked(Runtime, TEXT("Player is kicked by lobbyServer"));
		UPWCharacterManager::Get(this)->OnCharLeaveReq(Runtime, TEXT("Player is kicked by lobbyServer"));
		PlayerOutRsp(0, nCharID);
	}
	return true;
}

void UPWDSNet::OnAllocBattleReq(uint64 nBattleID, int32 nMatchType, int64 TimeStamp)
{
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::OnAllocBattleReq Received, BattleID(%llu), MatchType(%d) TimeStamp(%lld)"), nBattleID, nMatchType, TimeStamp);

	//UPWNetManager::Get(this)->SetBattleID(nBattleID);
	APWGameState* PWGS = Cast<APWGameState>(GetWorld()->GetGameState());
	if (PWGS)
	{
		PWGS->MatchType = nMatchType;
	}
	UPWGameSettings* GameSetting = const_cast<UPWGameSettings*>(UPWGameSettings::Get());
	APWGameMode* PWGM = Cast<APWGameMode>(GetWorld()->GetAuthGameMode());
	if (PWGM)
	{
		PWGM->UpdateFinishMatchSeconds(GameSetting->MatchSetting.FinishMatchSecond);
	}
	APWGameModeWereWolf* PWGMWW = Cast<APWGameModeWereWolf>(GetWorld()->GetAuthGameMode());
	if (PWGMWW)
	{
		PWGMWW->UpdateFinishMatchSeconds(GameSetting->MatchSetting.FinishMatchSecond);
		PWGMWW->UpdateCountDownSeconds(60.f * 10.f, true);
	}
	UPWGameInstance* GameInstance = UPWGameInstance::Get(this);
	if (GameInstance)
	{
		GameInstance->SetBattleStartTime(TimeStamp);
	}
	AllocBattleRsp();
}

void UPWDSNet::OnCountDownUpdated(int32 nSeconds)
{
	UE_LOG(LogTemp, Log, TEXT("UPWDSNet::OnCountDownUpdated Second = %d"), nSeconds);
	APWGameMode* PWGM = Cast<APWGameMode>(GetWorld()->GetAuthGameMode());
	if (PWGM)
	{
		PWGM->UpdateCountDownSeconds(nSeconds);
	}
	APWGameModeWereWolf* PWGMWW = Cast<APWGameModeWereWolf>(GetWorld()->GetAuthGameMode());
	if (PWGMWW)
	{
		PWGMWW->UpdateCountDownSeconds(nSeconds, true);
	}
}

void UPWDSNet::OnRobotInReq(TArray<FRobot> Robots)
{
	UE_LOG(LogTemp, Warning, TEXT("UPWDSNet::OnRobotInReq : %d"), Robots.Num());
	int Count = 3001;
	TArray<FString> Names;
	for (auto Robot : Robots)
	{
		PlayerData AIPlayerData;
		AIPlayerData.CharId = Count;
		FRandomStream RandomStream(FMath::Rand());
		AIPlayerData.Sex = RandomStream.RandRange(0, 1);
		AIPlayerData.Name = Robot.Name;
		int WhileCount = 0;
		do {
			WhileCount++;
			FRandomStream RandomStreamName(FMath::Rand());
			AIPlayerData.Name.AppendInt(RandomStreamName.RandRange(0, 9999));
		} while (Names.Contains(AIPlayerData.Name) && WhileCount <= 10000);
		UE_LOG(LogTemp, Warning, TEXT("UPWDSNet::OnRobotInReq : to find robot a proper name do while %d times"), WhileCount);
		if (WhileCount > 10000)
		{
			AIPlayerData.Name.AppendChar('a' + Count % 26);
		}
		if (Names.Contains(AIPlayerData.Name))
		{
			AIPlayerData.Name.AppendChar('A' + Count % 26);
		}

		Names.Add(AIPlayerData.Name);

		UPWCharacterManager::Get(this)->AddHumanAICharacter(AIPlayerData.Name, &AIPlayerData);
		Count++;
	}
}

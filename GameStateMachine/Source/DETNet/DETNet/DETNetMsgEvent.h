#pragma once
#include "DETNetCmn.h"

class DETNET_EXPORT DETNetMsgEvent
{
private:
	static TMap<int32, TArray<std::function<void(const FString&)> > > MsgEvents;
	static TArray<std::function<void(int)>> LoginEvents;			//登录事件
	static TArray<std::function<void(int)>> DropEvents;				//网络断开事件
	static TArray<std::function<void(int, int64)>> ModifyEvents;
	static TArray<std::function<void(int, int)>> ModifyFlagEvents;
	static TArray<std::function<void(int, int64)>> DelEvents;
	static TArray<std::function<void(void)>> HttpEvents;
	static TArray<std::function<void(void)>> BulletinEvents;
	static TArray<std::function<void(void)>> AdvanceNoticeEvents;
	static TArray<std::function<void(void)>> StopServiceEvents;
	static TArray<std::function<void(void)>> BattlePassBulletinEvents;
public:
	static void Emit(int ID, const void* Data);
	static void EmitLogin(int);
	static void EmitDrop(int);
	static void EmitModify(int, int64);
	static void EmitModifyFlag(int, int);
	static void EmitDel(int, int64);
	static void EmitHttp();
	static void EmitBulletin();
	static void EmitAdvanceNotice();
	static void EmitStopService();
	static void EmitBattlePassBulletin();
	static void Reg(int ID, std::function<void(const FString&)> cb);
	static void RegLoginEvent(std::function<void(int)> cb);
	static void RegDropEvent(std::function<void(int)> cb);
	static void RegHttpEvent(std::function<void(void)> cb);
	static void RegBulletinEvent(std::function<void(void)> cb);
	static void RegAdvanceNoticeEvent(std::function<void(void)> cb);
	static void RegStopServiceEvent(std::function<void(void)> cb);
	static void RegBattlePassBulletinBuEvent(std::function<void(void)> cb);
	static void RegModifyEvent(std::function<void(int, int64)> cb);
	static void RegModifyFlagEvent(std::function<void(int, int)> cb);
	static void RegDelEvent(std::function<void(int, int64)> cb);
	static void Init();
};


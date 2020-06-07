#include "DETNetMsgEvent.h"

TMap<int32, TArray<std::function<void(const FString&)>>> DETNetMsgEvent::MsgEvents;
TArray<std::function<void(int)>> DETNetMsgEvent::LoginEvents;
TArray<std::function<void(int)>> DETNetMsgEvent::DropEvents;
TArray<std::function<void(int, int64)>> DETNetMsgEvent::ModifyEvents;
TArray<std::function<void(int, int)>> DETNetMsgEvent::ModifyFlagEvents;
TArray<std::function<void(int, int64)>> DETNetMsgEvent::DelEvents;
TArray<std::function<void(void)>> DETNetMsgEvent::HttpEvents;
TArray<std::function<void(void)>> DETNetMsgEvent::BulletinEvents;
TArray<std::function<void(void)>> DETNetMsgEvent::AdvanceNoticeEvents;
TArray<std::function<void(void)>> DETNetMsgEvent::StopServiceEvents;
TArray<std::function<void(void)>> DETNetMsgEvent::BattlePassBulletinEvents;

void DETNetMsgEvent::Emit(int ID, const void* Data)
{
	auto evs = MsgEvents.Find(ID);
	if (evs)
	{
		for (int i = 0; i < evs->Num(); ++i)
		{
			(*evs)[i](FString(UTF8_TO_TCHAR(Data)));
		}
	}
}
void DETNetMsgEvent::EmitLogin(int Rst)
{
	for (auto i = 0; i < LoginEvents.Num(); ++i)
	{
		(LoginEvents[i])(Rst);
	}
}
void DETNetMsgEvent::EmitDrop(int Rst)
{
	for (auto i = 0; i < DropEvents.Num(); ++i)
	{
		(DropEvents[i])(Rst);
	}
}
void DETNetMsgEvent::EmitModify(int VNT, int64 Id)
{
	for (auto i = 0; i < ModifyEvents.Num(); ++i)
	{
		(ModifyEvents[i])(VNT, Id);
	}
}
void DETNetMsgEvent::EmitModifyFlag(int Idx, int Val)
{
	for (auto i = 0; i < ModifyFlagEvents.Num(); ++i)
	{
		(ModifyFlagEvents[i])(Idx, Val);
	}
}

void DETNetMsgEvent::EmitDel(int VNT, int64 Id)
{
	for (auto i = 0; i < DelEvents.Num(); ++i)
	{
		(DelEvents[i])(VNT, Id);
	}
}
void DETNetMsgEvent::EmitHttp()
{
	for (auto i = 0; i < HttpEvents.Num(); ++i)
	{
		(HttpEvents[i])();
	}
}

void DETNetMsgEvent::EmitBulletin()
{
	for (auto i = 0; i < BulletinEvents.Num(); ++i)
	{
		(BulletinEvents[i])();
	}
}

void DETNetMsgEvent::EmitAdvanceNotice()
{
	for (auto i = 0; i < AdvanceNoticeEvents.Num(); ++i)
	{
		(AdvanceNoticeEvents[i])();
	}
}

void DETNetMsgEvent::EmitStopService()
{
	for (auto i = 0; i < StopServiceEvents.Num(); ++i)
	{
		(StopServiceEvents[i])();
	}
}

void DETNetMsgEvent::EmitBattlePassBulletin()
{
	for (auto i = 0; i < BattlePassBulletinEvents.Num(); ++i)
	{
		(BattlePassBulletinEvents[i])();
	}
}

void DETNetMsgEvent::Reg(int ID, std::function<void(const FString&)> cb)
{
	MsgEvents.FindOrAdd(ID).Add(cb);
}
void DETNetMsgEvent::RegLoginEvent(std::function<void(int)> cb)
{
	LoginEvents.Add(cb);
}
void DETNetMsgEvent::RegDropEvent(std::function<void(int)> cb)
{
	DropEvents.Add(cb);
}

void DETNetMsgEvent::RegHttpEvent(std::function<void(void)> cb)
{
	HttpEvents.Add(cb);
}

void DETNetMsgEvent::RegBulletinEvent(std::function<void(void)> cb)
{
	BulletinEvents.Add(cb);
}

void DETNetMsgEvent::RegAdvanceNoticeEvent(std::function<void(void)> cb)
{
	AdvanceNoticeEvents.Add(cb);
}

void DETNetMsgEvent::RegStopServiceEvent(std::function<void(void)> cb)
{
	StopServiceEvents.Add(cb);
}

void DETNetMsgEvent::RegBattlePassBulletinBuEvent(std::function<void(void)> cb)
{
	BattlePassBulletinEvents.Add(cb);
}

void DETNetMsgEvent::RegModifyEvent(std::function<void(int, int64)> cb)
{
	ModifyEvents.Add(cb);
}

void DETNetMsgEvent::RegModifyFlagEvent(std::function<void(int, int)> cb)
{
	ModifyFlagEvents.Add(cb);
}

void DETNetMsgEvent::RegDelEvent(std::function<void(int, int64)> cb)
{
	DelEvents.Add(cb);
}

#include "DETNetMsgDeal.h"
#include "DETNetMsgEvent.h"
#include "DETNetStruct.h"
#include "DETNetClient.h"
#include "DETNetPlayerData.h"
#include "DETNetBattle.h"
void DealHttpRequestDone()
{
	UE_LOG(LogDETNet, Log, TEXT("DealHttpRequestDone"));
	DETNetClient::Instance().ConnectLogin(TEXT("test1"), TEXT(""), false, TEXT(""));
}
void DealSynTimeNotify(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealSynTimeNotify %s"), *Data);
	FCMD_S2C_SynTimeNotify rsp(Data);
	DETNetClient::Instance().SrvTimestamp = rsp.Timestamp;
	int Seconds;
	float PartialSeconds;
	UGameplayStatics::GetAccurateRealTime(nullptr, Seconds, PartialSeconds);
	DETNetClient::Instance().RcvSrvTime = int64(Seconds * 1000 + PartialSeconds * 1000);

}
void DealSynCfgNotify(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealSynCfgNotify %s"), *Data);
	FCMD_S2C_SynCfgNotify rsp(Data);
	DETNetClient::Instance().CfgMap.Empty();
	for (auto cfg : rsp.Cfg)
	{
		auto& unit = DETNetClient::Instance().CfgMap.FindOrAdd(cfg.Id);
		unit.M_int = cfg.M_int;
		unit.M_str = cfg.M_str;
		unit.M_float = cfg.M_float;
		unit.M_v_int = cfg.M_v_int;
		unit.M_v_str = cfg.M_v_str;
	}

}
void DealVerifyRsp(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealVerifyRsp %s"), *Data);
	FCMD_S2C_VerifyRsp rsp(Data);
	if (rsp.Result == LoginRst_Ok)
	{
		DETNetClient::Instance().ConnectEntry(rsp.Addr, rsp.CharId, rsp.Acct, rsp.Okey);
	}
	else
	{
		DETNetMsgEvent::EmitLogin(rsp.Result);
		DETNetClient::Instance().Close();
	}
}
void DealVerifyOkeyRsp(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealVerifyOkeyRsp %s"), *Data);
	FCMD_S2C_VerifyOkeyRsp rsp(Data);
	if (rsp.Result == LoginRst_Ok)
	{
		DETNetClient::Instance().ConnectLobby(rsp.Addr, rsp.Pwd);
	}
	else
	{
		DETNetMsgEvent::EmitLogin(rsp.Result);
		DETNetClient::Instance().Close();
	}
}
void DealLoginRsp(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealLoginRsp %s"), *Data);
	FCMD_S2C_LoginRsp rsp(Data);
	UE_LOG(LogDETNet, Log, TEXT("DealLoginRsp %d"), rsp.Result);
	if (rsp.Result == LoginRst_Ok)	//这个不需要处理
	{
		return;
	}
	DETNetMsgEvent::EmitLogin(rsp.Result);
	if (rsp.Result < LoginRst_Ok)
	{
		DETNetClient::Instance().Close();
	}
}
void DealCreateCharRsp(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealCreateCharRsp %s"), *Data);
	FCMD_S2C_CreateCharRsp rsp(Data);
}
template <typename InStruct>
void DecodeList(TMap<int64, InStruct>& List, DETNetDecode& decode)
{
	int size;
	decode.Decode(size);
	for (int i = 0; i < size; ++i)
	{
		DecodeStruct(List, decode);
	}
}
template <typename InStruct>
int64 DecodeStruct(TMap<int64, InStruct>& List, DETNetDecode& decode)
{
	uint64 id;
	decode.Decode(id);
	int bufSize;
	decode.Decode(bufSize);
	TArray<uint8> buf;
	buf.SetNumZeroed(bufSize);
	decode.Decode(buf);
	buf.Add('\0');
	auto& Unit = List.FindOrAdd(int64(id));
	DHJsonUtils::FromJson(FString(UTF8_TO_TCHAR(buf.GetData())), &Unit);
	return int64(id);
}
template <typename InStruct>
void RemoveFromList(TMap<int64, InStruct>& List, int64 Id)
{
	List.Remove(Id);
}
void DecodePlayerData(const TArray<uint8>& Buf, PlayerData& Data)
{
	DETNetDecode decode(Buf);
	uint8 vnt;
	do
	{
		if (decode.AvailableToRead() <= 0)
		{
			break;
		}
		decode.Decode(vnt);
		switch (vnt)
		{
		case VNT_CreateTime:
			decode.Decode(Data.CreateTime);
			break;
		case VNT_OnlineTime:
			decode.Decode(Data.OnlineTime);
			break;
		case VNT_CharName:
			decode.Decode(Data.Name);
			break;
		case VNT_Acct:
			decode.Decode(Data.Acct);
			break;
		case VNT_Level:
			decode.Decode(Data.Level);
			break;
		case VNT_Exp:
			decode.Decode(Data.Exp);
			break;
		case VNT_Sex:
			decode.Decode(Data.Sex);
			break;
		case VNT_Race:
			decode.Decode(Data.Race);
			break;
		case VNT_Money:
			decode.Decode(Data.Money);
			break;
		case VNT_MoneyBound:
			decode.Decode(Data.MoneyBound);
			break;
		case VNT_Gold:
			decode.Decode(Data.Gold);
			break;
		case VNT_GoldBound:
			decode.Decode(Data.GoldBound);
			break;
		case VNT_LegendGold:
			decode.Decode(Data.LegendGold);
			break;
		case VNT_DetMedal:
			decode.Decode(Data.DetMedal);
			break;
		case VNT_Flagint32:
		{
			int size;
			decode.Decode(size);
			Data.Flags.SetNum(size);
			for (int i = 0; i < size; ++i)
			{
				int idx, val;
				decode.Decode(idx);
				decode.Decode(val);
				Data.Flags[idx] = val;
			}
		}break;
		case VNT_ItemList:;
		{
			DecodeList(Data.ItemList, decode);
		}break;
		case VNT_AchList:
		{
			DecodeList(Data.AchList, decode);
		}break;
		case VNT_CDList:
		{
			DecodeList(Data.CDList, decode);
		}break;
		case VNT_SkillList:
		{
			DecodeList(Data.SkillList, decode);
		}break;
		case VNT_StateList:
		{
			DecodeList(Data.StateList, decode);
		}break;
		case VNT_CupList:
		{
			DecodeList(Data.CupList, decode);
		}break;
		case VNT_QuestList:
		{
			DecodeList(Data.QuestList, decode);
		}break;
		case VNT_APList:
		{
			DecodeList(Data.APList, decode);
		}break;
		case VNT_UAVList:
		{
			DecodeList(Data.UAVList, decode);
		}break;
		default:
			UE_LOG(LogDETNet, Log, TEXT("DealSynCharInfo lost %d"), vnt);
			break;
		}
	} while (1);
}
void DealSynCharInfo(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealSynCharInfo"));
	FCMD_S2C_SynCharInfo info(Data);
	auto& player = DETNetClient::Instance().Data;
	player.Clear();
	player.CharId = info.CharId;
	DecodePlayerData(info.Data, player);
	DETNetMsgEvent::EmitLogin(LoginRst_Ok);
}
void DealSynCharInfoModify(const FString& Data)
{
	FCMD_S2C_SynCharInfoModify info(Data);
	int64 Id = 0;
	DETNetDecode decode(info.Data);
	switch (info.Type)
	{
	case VNT_OnlineTime:
		decode.Decode(DETNetClient::Instance().Data.OnlineTime);
		break;
	case VNT_CharName:
		decode.Decode(DETNetClient::Instance().Data.Name);
		break;
	case VNT_Level:
		decode.Decode(DETNetClient::Instance().Data.Level);
		break;
	case VNT_Exp:
		decode.Decode(DETNetClient::Instance().Data.Exp);
		break;
	case VNT_Sex:
		decode.Decode(DETNetClient::Instance().Data.Sex);
		break;
	case VNT_Race:
		decode.Decode(DETNetClient::Instance().Data.Race);
		break;
	case VNT_Money:
		decode.Decode(DETNetClient::Instance().Data.Money);
		break;
	case VNT_MoneyBound:
		decode.Decode(DETNetClient::Instance().Data.MoneyBound);
		break;
	case VNT_Gold:
		decode.Decode(DETNetClient::Instance().Data.Gold);
		break;
	case VNT_GoldBound:
		decode.Decode(DETNetClient::Instance().Data.GoldBound);
		break;
	case VNT_LegendGold:
		decode.Decode(DETNetClient::Instance().Data.LegendGold);
		break;
	case VNT_DetMedal:
		decode.Decode(DETNetClient::Instance().Data.DetMedal);
		break;
	case VNT_Flagint32:
	{
		int idx, val;
		decode.Decode(idx);
		decode.Decode(val);
		DETNetClient::Instance().Data.Flags[idx] = val;
		Id = idx;
	}break;
	case VNT_ItemList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.ItemList, decode);
	}break;
	case VNT_AchList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.AchList, decode);
	}break;
	case VNT_CDList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.CDList, decode);
	}break;
	case VNT_SkillList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.SkillList, decode);
	}break;
	case VNT_StateList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.StateList, decode);
	}break;
	case VNT_CupList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.CupList, decode);
	}break;
	case VNT_QuestList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.QuestList, decode);
	}break;
	case VNT_APList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.APList, decode);
	}break;
	case VNT_UAVList:
	{
		Id = DecodeStruct(DETNetClient::Instance().Data.UAVList, decode);
	}break;
	}
	DETNetMsgEvent::EmitModify(info.Type, Id);
	UE_LOG(LogDETNet, Log, TEXT("DealSynCharInfoModify,%d,%lld"), info.Type, Id);
}
void DealSynCharInfoDel(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealSynCharInfoDel %s"), *Data);
	FCMD_S2C_SynCharInfoDel info(Data);
	int64 Id = info.Id;
	switch (info.Type)
	{
	case VNT_ItemList:;
	{
		RemoveFromList(DETNetClient::Instance().Data.ItemList, info.Id);
	}break;
	case VNT_AchList:
	{
		RemoveFromList(DETNetClient::Instance().Data.AchList, info.Id);
	}break;
	case VNT_CDList:
	{
		RemoveFromList(DETNetClient::Instance().Data.CDList, info.Id);
	}break;
	case VNT_SkillList:
	{
		RemoveFromList(DETNetClient::Instance().Data.SkillList, info.Id);
	}break;
	case VNT_StateList:
	{
		RemoveFromList(DETNetClient::Instance().Data.StateList, info.Id);
	}break;
	case VNT_CupList:
	{
		RemoveFromList(DETNetClient::Instance().Data.CupList, info.Id);
	}break;
	case VNT_QuestList:
	{
		RemoveFromList(DETNetClient::Instance().Data.QuestList, info.Id);
	}break;
	case VNT_APList:
	{
		RemoveFromList(DETNetClient::Instance().Data.APList, info.Id);
	}break;
	case VNT_UAVList:
	{
		RemoveFromList(DETNetClient::Instance().Data.UAVList, info.Id);
	}break;
	}
	DETNetMsgEvent::EmitDel(info.Type, Id);
	UE_LOG(LogDETNet, Log, TEXT("DealSynCharInfoDel,%d,%lld"), info.Type, Id);
}
void LoginRst(int Rst)
{
	UE_LOG(LogDETNet, Log, TEXT("LoginRst %d"), Rst);
}
void DropRst(int Rst)
{
	UE_LOG(LogDETNet, Log, TEXT("DropRst %d"), Rst);
}

void DealMatchAllocBattleReq(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealMatchAllocBattleReq %s"), *Data);
	FCMD_Match2Battle_AllocBattleReq req(Data);
	DETNetBattle::Instance().BattleId = req.BattleId;
}
void DealMatchPlayerInReq(const FString& Data)
{
	//UE_LOG(LogDETNet, Log, TEXT("DealMatchPlayerInReq %s"), *Data);
	FCMD_Match2Battle_PlayerInReq req(Data);
	PlayerData& Player = DETNetBattle::Instance().Players.FindOrAdd(req.CharId);
	DecodePlayerData(req.Data, Player);
	UE_LOG(LogDETNet, Log, TEXT("DealMatchPlayerInReq,%llu"), Player.CharId);
}

void DealPlayerSetFlag(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("PlayerSetFlag successfully"));
}

void DealAcBeginNotify(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealAcBeginNotify %s"), *Data);
	FCMD_S2C_AcBeginNotify notify(Data);
}
void DealAcEndNotify(const FString& Data)
{
	UE_LOG(LogDETNet, Log, TEXT("DealAcEndNotify %s"), *Data);
	FCMD_S2C_AcEndNotify notify(Data);
}

void DETNetMsgEvent::Init()
{
	DETNetMsgEvent::Reg(S2C_SynTimeNotify, DealSynTimeNotify);
	DETNetMsgEvent::Reg(S2C_SynCfgNotify, DealSynCfgNotify);
	DETNetMsgEvent::Reg(S2C_VerifyRsp, DealVerifyRsp);
	DETNetMsgEvent::Reg(S2C_VerifyOkeyRsp, DealVerifyOkeyRsp);
	DETNetMsgEvent::Reg(S2C_LoginRsp, DealLoginRsp);
	DETNetMsgEvent::Reg(S2C_CreateCharRsp, DealCreateCharRsp);
	DETNetMsgEvent::Reg(S2C_SynCharInfo, DealSynCharInfo);
	DETNetMsgEvent::Reg(S2C_SynCharInfoModify, DealSynCharInfoModify);
	DETNetMsgEvent::Reg(S2C_SynCharInfoDel, DealSynCharInfoDel);
	DETNetMsgEvent::Reg(Match2Battle_AllocBattleReq, DealMatchAllocBattleReq);
	DETNetMsgEvent::Reg(Match2Battle_PlayerInReq, DealMatchPlayerInReq);

	DETNetMsgEvent::Reg(S2C_PlayerSetFlagRsp, DealPlayerSetFlag);

	DETNetMsgEvent::Reg(S2C_AcBeginNotify, DealAcBeginNotify);
	DETNetMsgEvent::Reg(S2C_AcEndNotify, DealAcEndNotify);

	DETNetMsgEvent::RegLoginEvent(LoginRst);
	DETNetMsgEvent::RegDropEvent(DropRst);
	//DETNetMsgEvent::HttpEvents.Add(DealHttpRequestDone);
}

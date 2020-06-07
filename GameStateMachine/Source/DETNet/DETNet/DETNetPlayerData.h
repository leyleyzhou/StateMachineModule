#pragma once
#include "DETNetCmn.h"
#include "DETNetStruct.h"

class DETNET_EXPORT PlayerData
{
public:
	int64 CharId;
	int CreateTime;
	int OnlineTime;
	int Level;
	int Exp;
	int Sex;
	int Race;
	int Money;						//D币
	int MoneyBound;					//绑定D币
	int Gold;						//金币
	int GoldBound;					//绑定金币
	int LegendGold;					//传奇币
	int DetMedal;					//Det奖章
	FString Acct;
	FString Name;
	TArray<int> Flags;
	TMap<int64, FSItem> ItemList;
	TMap<int64, FSAch> AchList;
	TMap<int64, FSCD> CDList;
	TMap<int64, FSSkill> SkillList;
	TMap<int64, FSState> StateList;
	TMap<int64, FCupInfo> CupList;
	TArray<int>   AchieveCupList;
	TMap<int64, FSQuest> QuestList;
	TMap<int64, FSAdditionPurchase> APList;
	TMap<int64, FSUAV> UAVList;
	void Clear()
	{
		CharId = 0;
		CreateTime = 0;
		OnlineTime = 0;
		Level = 0;
		Exp = 0;
		Sex = 0;
		Race = 0;
		Money = 0;
		MoneyBound = 0;
		Gold = 0;
		GoldBound = 0;
		LegendGold = 0;
		DetMedal = 0;
		Acct.Empty();
		Name.Empty();
		Flags.Empty();
		ItemList.Empty();
		AchList.Empty();
		CDList.Empty();
		SkillList.Empty();
		StateList.Empty();
		CupList.Empty();
		AchieveCupList.Empty();
		QuestList.Empty();
		APList.Empty();
		UAVList.Empty();
	}
};

enum Login_Rst
{
	LoginRst_NoChar = 1,
	LoginRst_Ok = 0,

	LoginRst_NoArea = -1,			//服务器还没有准本完成，没有可用的服务器
	LoginRst_Ban = -2,				//被服务器禁止登陆
	LoginRst_LoginDBErr = -3,		//登陆失败（错误码）
	LoginRst_VerifyFailed = -4,		//平台（ps4）校验失败

	LoginRst_Ver = -11,				//客户端与服务器版本不一致
	LoginRst_EntryWrongStatus = -12,//登陆失败（错误码）
	LoginRst_OnEntry = -13,			//登陆失败（错误码）
	LoginRst_EntryDBErr = -14,		//登陆失败（错误码）
	LoginRst_EntryJson = -15,		//登陆失败（错误码）
	LoginRst_InvalidOkey = -16,		//登陆失败（错误码）
	LoginRst_EntryTimeout = -17,	//登陆失败（错误码）

	LoginRst_Online = -21,			//已经在线，请重试
	LoginRst_NoLobby = -22,			//登陆失败（错误码）
	LoginRst_CentreTimeout = -23,	//登陆失败（错误码）

	LoginRst_LobbyWrongStatus = -31,//登陆失败（错误码）
	LoginRst_LobbyWrongStatus2 = -32,//登陆失败（错误码）
	LoginRst_LobbyWrongStatus3 = -33,//登陆失败（错误码）
	LoginRst_LobbyTimeout = -34,	//登陆失败（错误码）
	LoginRst_LostPlayer = -35,		//登陆失败（错误码）
	LoginRst_InvalidPwd = -36,		//登陆失败（错误码）
	LoginRst_LobbyDBErr = -37,		//登陆失败（错误码）
	LoginRst_LobbyDBErr2 = -38,		//登陆失败（错误码）
	LoginRst_LobbyDBErr3 = -39,		//登陆失败（错误码）
	LoginRst_LobbyDBErr4 = -40,		//登陆失败（错误码）
};

enum Drop_Type
{
	DropType_Normal = 0,				//已经进入游戏了，然后与服务器断开连接
	DropType_GetHttpFailed = 1,			//登陆阶段，连接失败（返回码）
	DropType_ConnectLoginFailed = 2,	//登陆阶段，连接失败（返回码）
	DropType_ConnectEntryFailed = 3,	//登陆阶段，连接失败（返回码）
	DropType_ConnectLobbyFailed = 4,	//登陆阶段，连接失败（返回码）
};

enum Create_Rsp
{
	CreateRst_OK = 0,					//成功
	CreateRst_Err_Wrong_Status = -1,	//错误状态
	CreateRst_Err_Lost_Item = -2,		//错误装备
	CreateRst_Err_Wrong_Sex = -4,		//装备性别错误
	CreateRst_Err_Wrong_BodyPart = -5,	//部位错误
	CreateRst_Err_Wrong_Data = -6,		//数据错误
	CreateRst_Err_Other = -10,			//其他错误
};

enum VNT
{
	VNT_Invalid,
	VNT_CreateTime,
	VNT_OnlineTime,
	VNT_CharName,
	VNT_Acct,
	VNT_Level,
	VNT_Exp,
	VNT_Sex,
	VNT_Race,
	VNT_Money,
	VNT_MoneyBound,
	VNT_Gold,
	VNT_GoldBound,
	VNT_LegendGold,
	VNT_DetMedal,
	VNT_Flagint32,
	VNT_ItemList,
	VNT_AchList,
	VNT_CDList,
	VNT_SkillList,
	VNT_StateList,
	VNT_CupList,
	VNT_QuestList,
	VNT_APList,
	VNT_UAVList,
};

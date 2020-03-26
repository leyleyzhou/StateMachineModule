#pragma once
#include "Json/DHJsonUtils.h"
#include "DETNetStruct.generated.h"

USTRUCT()
struct FICmd
{
	GENERATED_BODY()
		virtual int GetID()const
	{
		return 0;
	}
	virtual ~FICmd() {}
};
const int MSG_Battle = 10;
enum
{
	Server2Server_Heartbeat = 0,
	Server2Server_SayHello = 1,
	Server2Server_SayHelloRsp = 2,
	Lobby2Match_LeaveBattleUnExcepted = 1216,
	Match2Battle_AllocBattleReq = 10001,
	Battle2Match_AllocBattleRsp = 10002,
	Match2Battle_PlayerInReq = 10003,
	Battle2Match_PlayerInRsp = 10004,
	Match2Battle_PlayerOutReq = 10005,
	Battle2Match_PlayerOutRsp = 10006,
	Battle2Match_PlayerResultReq = 10014,
	Match2Battle_PlayerResultRsp = 10015,
	Battle2Match_PlayerConnectNotify = 10007,
	Battle2Match_PlayerDisconnectNotify = 10008,
	Battle2Match_CanWorkNotify = 10010,
	Battle2Match_BeginFightNotify = 10009,
	Match2Battle_CutdownNofity = 10013,
	Battle2Match_SyncStatus = 10016,
	Match2Battle_ForceExit = 10017,
	BattleMgr2Match_BattleSrvAddr = 10102,
	LuaPublish_MatchBattleNotify = 10204,
	LuaPublish_DoExitBattleNotify = 10206,
	C2S_GMReq = 1050,
	C2S_VerifyReq = 1001,
	S2C_VerifyRsp = 1002,
	C2S_VerifyOkeyReq = 1003,
	S2C_VerifyOkeyRsp = 1004,
	S2C_SynWaitInfo = 1005,
	C2S_LoginReq = 1006,
	S2C_LoginRsp = 1007,
	C2S_CreateCharReq = 1008,
	S2C_CreateCharRsp = 1009,
	S2C_SynCharInfo = 1010,
	S2C_KickNotify = 1011,
	S2C_SynCharInfoModify = 1012,
	S2C_SynCharInfoDel = 1013,
	S2C_SynCfgNotify = 1020,
	S2C_SynTimeNotify = 1021,
	S2C_SynSkillNotify = 1025,
	S2C_AcBeginNotify = 1022,
	S2C_AcEndNotify = 1023,
	C2S_PlayerInfoGetReq = 1201,
	S2C_PlayerInfoGetRsp = 1202,
	S2C_TextUINotify = 1024,
	S2C_ApplyResultNotify = 1099,
	C2S_ApplyReq = 1100,
	S2C_ApplyRsp = 1101,
	C2S_ApplyWolfReq = 1130,
	S2C_ApplyWolfRsp = 1131,
	C2S_UnApplyWolfReq = 1132,
	S2C_UnApplyWolfRsp = 1133,
	C2S_UnExceptedLeaveBattleReq = 1137,
	C2S_TeamInviteJoinReq = 1104,
	S2C_TeamInviteJoinRsp = 1105,
	C2S_TeamReplyInviteJoin = 1106,
	S2C_TeamReplyInviteJoinRsp = 1124,
	C2S_TeamLeaveReq = 1107,
	C2S_TeamKickReq = 1109,
	C2S_TeamChatReq = 1111,
	C2S_TeamDissolveReq = 1113,
	C2S_TeamSelectTypeReq = 1115,
	C2S_TeamApplyJoinReq = 1117,
	C2S_TeamSetCloseJoinReq = 1119,
	C2S_TeamTransLeaderReq = 1121,
	C2S_ChangeClothesReq = 1181,
	S2C_ChangeClothesRsp = 1182,
	C2S_ChangeLanguageReq = 1183,
	S2C_ChangeLanguageRsp = 1184,
	S2C_TeamPlayerLeaveNotify = 1108,
	S2C_TeamPlayerJoinNotify = 1126,
	S2C_TeamChatNotify = 1112,
	S2C_TeamDissolveNotify = 1114,
	S2C_TeamMatchTypeChangeNotify = 1116,
	S2C_TeamPlayerReadyNotify = 1118,
	S2C_TeamLeadeChangeNotify = 1122,
	S2C_TeamCloseJoinNotify = 1120,
	S2C_TeamInviteJoinNotify = 1123,
	S2C_TeamMemberNotify = 1134,
	S2C_MatchBattleNotify = 1135,
	S2C_BattleAwardNotify = 1136,
	C2S_CreateGroupReq = 1140,
	S2C_CreateGroupRsp = 1141,
	C2S_JoinGroupReq = 1142,
	S2C_JoinGroupRsp = 1143,
	C2S_LeaveGroupReq = 1144,
	S2C_LeaveGroupRsp = 1145,
	C2S_WolfWaitingReq = 1138,
	S2C_WolfWaitingRsp = 1139,
	C2S_SelectModuleReq = 1128,
	S2C_SelectModuleRsp = 1129,
	S2C_TeamSynTeamInfo = 1127,
	S2C_SupermarketBuyRsp = 1153,
	S2C_SupermarketGetRsp = 1151,
	C2S_SupermarketBuyReq = 1152,
	C2S_SupermarketGetReq = 1150,
	S2C_PS4BuyRsp = 1155,
	C2S_PS4BuyReq = 1154,
	S2C_NotifyPs4SupermarketInfo = 1156,
	C2S_SetAchievementReq = 1170,
	S2C_SetAchievementRsp = 1171,
	C2S_GetAchievementPriceReq = 1172,
	S2C_GetAchievementPriceRsp = 1173,
	C2S_ResetSkillTreeReq = 1174,
	S2C_ResetSkillTreeRsp = 1175,
	C2S_UnlockSkillReq = 1176,
	S2C_UnlockSkillRsp = 1177,
	S2C_SynCoolDownDel = 1178,
	S2C_SynStateDel = 1179,
	S2C_SyncMails = 1180,
	C2S_SetGunPaintingReq = 1204,
	S2C_SetGunPaintingRsp = 1205,
	C2S_PlayerUseItemReq = 1208,
	S2C_PlayerUseItemRsp = 1209,
	C2S_SetItemNotNewReq = 1210,
	S2C_SetItemNotNewRsp = 1211,
	C2S_OpenChestReq = 1212,
	S2C_OpenChestRsp = 1213,
	C2S_ChestInfoReq = 1214,
	S2C_ChestInfoRsp = 1215,
	C2S_DecomposeItemReq = 1216,
	S2C_DecomposeItemRsp = 1217,
	C2S_BattleVerifyReq = 2001,
	C2S_PlayerCupProcessReq = 1301,
	C2S_PlayerSetPlusReq = 1311,
	C2S_PlayerSetFlagReq = 1206,
	S2C_PlayerSetFlagRsp = 1207,
	C2S_MaintainGmReq = 1051,
	C2S_PlayerDelMailReq = 1401,
	S2C_PlayerDelMailRsp = 1402,
	C2S_PlayerReadMailReq = 1405,
	S2C_PlayerReadMailRsp = 1406,
	C2S_PlayerGetMailAttachReq = 1403,
	S2C_PlayerGetMailAttachRsp = 1404,
	C2S_ActiveBattlepass = 1501,
	C2S_AddQuestSlotNum = 1502,
	C2S_SetQuestSlotNum = 1503,
	C2S_GetQuestAward = 1504,
	S2C_CurrentQuestStatus = 1505,
	S2C_BPExp2GoldNotify = 1218,
	C2S_SetUAVFlagReq = 1219,
	S2C_SetUAVFlagRsp = 1220,
	C2S_BPEndAfterSecReq = 1221,
	S2C_BPEndAfterSecRsp = 1222,

};

USTRUCT()
struct FAttachUnit
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Num = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FAttachUnit, type, Type, id, Id, num, Num)

USTRUCT()
struct FMailUnit
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int64 OnlyId = 0;
	UPROPERTY()
		int64 OwnerId = 0;
	UPROPERTY()
		int32 MailId = 0;
	UPROPERTY()
		FString Title;
	UPROPERTY()
		FString Content;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		FString Sender;
	UPROPERTY()
		int32 Status = 0;
	UPROPERTY()
		int32 ExpireTime = 0;
	UPROPERTY()
		int32 CreateTime = 0;
	UPROPERTY()
		int32 Reason = 0;
	UPROPERTY()
		int32 ObjType = 0;
	UPROPERTY()
		int32 UiType = 0;
	UPROPERTY()
		FString FilePath;
	UPROPERTY()
		FString ObjDesc;
	UPROPERTY()
		TArray<FAttachUnit> AttachList;
	UPROPERTY()
		FString Attach;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		int32 Send = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FMailUnit, id, Id, onlyid, OnlyId, ownerid, OwnerId, mailid, MailId, title, Title, content, Content, type, Type, sender, Sender, status, Status, expiretime, ExpireTime, createtime, CreateTime, reason, Reason, objtype, ObjType, uitype, UiType, filepath, FilePath, objdesc, ObjDesc, attachlist, AttachList, attach, Attach, excelid, ExcelId, send, Send)

USTRUCT()
struct FSItem
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		int32 Num = 0;
	UPROPERTY()
		int32 BeginTime = 0;
	UPROPERTY()
		int32 KeepSeconds = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		int32 SynBattle = 0;
	UPROPERTY()
		TArray<int> Param;
};

MAKE_DH_JSON_FIELD_NAMES(FSItem, basestruct, baseStruct, excelid, ExcelId, num, Num, begintime, BeginTime, keepseconds, KeepSeconds, type, Type, synbattle, SynBattle, param, Param)

USTRUCT()
struct FSAch
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 CurNum = 0;
	UPROPERTY()
		int32 TargetNum = 0;
	UPROPERTY()
		int32 IsGet = 0;
	UPROPERTY()
		TArray<int> Param;
};

MAKE_DH_JSON_FIELD_NAMES(FSAch, basestruct, baseStruct, curnum, CurNum, targetnum, TargetNum, isget, IsGet, param, Param)

USTRUCT()
struct FSCD
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		int32 CntLeft = 0;
	UPROPERTY()
		int32 BeginTime = 0;
	UPROPERTY()
		int32 KeepSeconds = 0;
	UPROPERTY()
		int32 Cnt = 0;
	UPROPERTY()
		TArray<int> Param;
};

MAKE_DH_JSON_FIELD_NAMES(FSCD, basestruct, baseStruct, excelid, ExcelId, cntleft, CntLeft, begintime, BeginTime, keepseconds, KeepSeconds, cnt, Cnt, param, Param)

USTRUCT()
struct FSSkill
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 Cnt = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FSSkill, basestruct, baseStruct, cnt, Cnt)

USTRUCT()
struct FSState
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		int32 BeginTime = 0;
	UPROPERTY()
		int32 KeepSeconds = 0;
	UPROPERTY()
		TArray<int> Param;
};

MAKE_DH_JSON_FIELD_NAMES(FSState, basestruct, baseStruct, excelid, ExcelId, begintime, BeginTime, keepseconds, KeepSeconds, param, Param)

USTRUCT()
struct FSCup
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		int32 Process = 0;
	UPROPERTY()
		bool IsFinish = false;
};

MAKE_DH_JSON_FIELD_NAMES(FSCup, basestruct, baseStruct, type, Type, process, Process, isfinish, IsFinish)

USTRUCT()
struct FSQuestSlot
{
	GENERATED_BODY()

		UPROPERTY()
		int32 SlotID = 0;
	UPROPERTY()
		int32 Progress = 0;
	UPROPERTY()
		int32 Completed = 0;
	UPROPERTY()
		int32 watcherSlotId = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FSQuestSlot, slotid, SlotID, progress, Progress, completed, Completed, watcherslotid, watcherSlotId)

USTRUCT()
struct FSQuest
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		int64 StartAt = 0;
	UPROPERTY()
		int64 ExpireAt = 0;
	UPROPERTY()
		int32 QstSrc = 0;
	UPROPERTY()
		int32 QstSrcId = 0;
	UPROPERTY()
		int32 Completed = 0;
	UPROPERTY()
		TMap<FString, FSQuestSlot> Progress;
	UPROPERTY()
		TMap<FString, int> AwardStatus;
	UPROPERTY()
		TMap<FString, int> ExtData;
};

MAKE_DH_JSON_FIELD_NAMES(FSQuest, basestruct, baseStruct, excelid, ExcelId, startat, StartAt, expireat, ExpireAt, qstsrc, QstSrc, qstsrcid, QstSrcId, completed, Completed, progress, Progress, awardstatus, AwardStatus, extdata, ExtData)

USTRUCT()
struct FSAdditionPurchase
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int64 PurchasedAt = 0;
	UPROPERTY()
		int32 ExpireType = 0;
	UPROPERTY()
		int64 ExpiredAt = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FSAdditionPurchase, basestruct, baseStruct, purchasedat, PurchasedAt, expiretype, ExpireType, expiredat, ExpiredAt)

USTRUCT()
struct FSUAV
{
	GENERATED_BODY()

		UPROPERTY()
		int64 ID = 0;
	UPROPERTY()
		int32 ExpireType = 0;
	UPROPERTY()
		int64 ExpireValue = 0;
	UPROPERTY()
		TArray<int> Flags;
};

MAKE_DH_JSON_FIELD_NAMES(FSUAV, basestruct, baseStruct, expiretype, ExpireType, expirevalue, ExpireValue, flags, Flags)

USTRUCT()
struct FFightBehaviour
{
	GENERATED_BODY()

		UPROPERTY()
		FString Action;
	UPROPERTY()
		FString Target;
	UPROPERTY()
		int32 Value = 0;
	UPROPERTY()
		int32 AwardExp = 0;
	UPROPERTY()
		int32 AwardGold = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FFightBehaviour, action, Action, target, Target, value, Value, awardexp, AwardExp, awardgold, AwardGold)

USTRUCT()
struct FFightResult
{
	GENERATED_BODY()

		UPROPERTY()
		int32 AliveTime = 0;
	UPROPERTY()
		int32 EndMatchReason = 0;
	UPROPERTY()
		TArray<FFightBehaviour> Behaviours;
};

MAKE_DH_JSON_FIELD_NAMES(FFightResult, alivetime, AliveTime, endmatchreason, EndMatchReason, behaviours, Behaviours)

USTRUCT()
struct FFightBusinessCardInfo
{
	GENERATED_BODY()

		UPROPERTY()
		FString MemberName;
	UPROPERTY()
		int32 BusinessCardID = 0;
	UPROPERTY()
		int32 HumanKillHuman = 0;
	UPROPERTY()
		int32 HumanKillWolf = 0;
	UPROPERTY()
		int32 HumanWin = 0;
	UPROPERTY()
		int32 WolfKillHuman = 0;
	UPROPERTY()
		int32 WolfPreventEvacuate = 0;
	UPROPERTY()
		int32 WolfKillAllHuman = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FFightBusinessCardInfo, membername, MemberName, businesscardid, BusinessCardID, humankillhuman, HumanKillHuman, humankillwolf, HumanKillWolf, humanwin, HumanWin, wolfkillhuman, WolfKillHuman, wolfpreventevacuate, WolfPreventEvacuate, wolfkillallhuman, WolfKillAllHuman)

USTRUCT()
struct FFightTeamInfo
{
	GENERATED_BODY()

		UPROPERTY()
		TArray<FFightBusinessCardInfo> Card;
};

MAKE_DH_JSON_FIELD_NAMES(FFightTeamInfo, card, Card)

USTRUCT()
struct FSupermarketItem
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int64 Num = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FSupermarketItem, id, Id, num, Num)

USTRUCT()
struct FPs4SupermarketInfo
{
	GENERATED_BODY()

		UPROPERTY()
		FString Id;
	UPROPERTY()
		FString ProId;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		TArray<FSupermarketItem> OutPut;
};

MAKE_DH_JSON_FIELD_NAMES(FPs4SupermarketInfo, id, Id, proid, ProId, type, Type, output, OutPut)

USTRUCT()
struct FCupInfo
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		int32 Process = 0;
	UPROPERTY()
		bool IsFinish = false;
};

MAKE_DH_JSON_FIELD_NAMES(FCupInfo, id, Id, type, Type, process, Process, isfinish, IsFinish)

USTRUCT()
struct FGoodsUnit
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 ItemId = 0;
	UPROPERTY()
		TArray<int> BundleItemIds;
	UPROPERTY()
		TArray<int> Price;
	UPROPERTY()
		int32 PriceType = 0;
	UPROPERTY()
		int32 IsNew = 0;
	UPROPERTY()
		int32 Hot = 0;
	UPROPERTY()
		int32 Discount = 0;
	UPROPERTY()
		int32 LimitCnt = 0;
	UPROPERTY()
		int32 LeftCnt = 0;
	UPROPERTY()
		int32 LimitTime = 0;
	UPROPERTY()
		int32 Gift = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		int32 Order = 0;
	UPROPERTY()
		FString Url;
	UPROPERTY()
		TArray<int> Param;
};

MAKE_DH_JSON_FIELD_NAMES(FGoodsUnit, id, Id, itemid, ItemId, bundleitemids, BundleItemIds, price, Price, pricetype, PriceType, isnew, IsNew, hot, Hot, discount, Discount, limitcnt, LimitCnt, leftcnt, LeftCnt, limittime, LimitTime, gift, Gift, type, Type, order, Order, url, Url, param, Param)

USTRUCT()
struct FDisItem
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Discount = 0;
	UPROPERTY()
		int32 MallType = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FDisItem, id, Id, discount, Discount, malltype, MallType)

USTRUCT()
struct FRobot
{
	GENERATED_BODY()

		UPROPERTY()
		FString Name;
};

MAKE_DH_JSON_FIELD_NAMES(FRobot, name, Name)

USTRUCT()
struct FSkillObj
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		TArray<int> PreSkillIds;
	UPROPERTY()
		int32 Cost = 0;
	UPROPERTY()
		int32 Type = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FSkillObj, id, Id, preskillids, PreSkillIds, cost, Cost, type, Type)

USTRUCT()
struct FCfgObj
{
	GENERATED_BODY()

		UPROPERTY()
		FString Id;
	UPROPERTY()
		int32 M_int = 0;
	UPROPERTY()
		FString M_str;
	UPROPERTY()
		float M_float = 0.0;
	UPROPERTY()
		TArray<int> M_v_int;
	UPROPERTY()
		TArray<FString> M_v_str;
};

MAKE_DH_JSON_FIELD_NAMES(FCfgObj, id, Id, m_int, M_int, m_str, M_str, m_float, M_float, m_v_int, M_v_int, m_v_str, M_v_str)

USTRUCT()
struct FGMAddItem
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Num = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FGMAddItem, id, Id, num, Num)

USTRUCT()
struct FAddrObj
{
	GENERATED_BODY()

		UPROPERTY()
		FString Addr;
	UPROPERTY()
		FString Name;
};

MAKE_DH_JSON_FIELD_NAMES(FAddrObj, addr, Addr, name, Name)

USTRUCT()
struct FClientAddrObj
{
	GENERATED_BODY()

		UPROPERTY()
		TArray<FAddrObj> Addrs;
	UPROPERTY()
		FString UpdateUrl;
	UPROPERTY()
		FString LogUrl;
	UPROPERTY()
		FString PrivacyUrl;
	UPROPERTY()
		FString AgreementUrl;
	UPROPERTY()
		FString AnnouncementUrl;
	UPROPERTY()
		int32 State = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FClientAddrObj, addrs, Addrs, updateurl, UpdateUrl, logurl, LogUrl, privacyurl, PrivacyUrl, agreementurl, AgreementUrl, announcementurl, AnnouncementUrl, state, State)

USTRUCT()
struct FGMSetFlag
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Val = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FGMSetFlag, id, Id, val, Val)

USTRUCT()
struct FGMUserDefine
{
	GENERATED_BODY()

		UPROPERTY()
		FString CmdId;
	UPROPERTY()
		int32 Param1 = 0;
	UPROPERTY()
		int32 Param2 = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FGMUserDefine, cmdid, CmdId, param1, Param1, param2, Param2)

USTRUCT()
struct FHttpAreaObjName
{
	GENERATED_BODY()

		UPROPERTY()
		FString Lang;
	UPROPERTY()
		FString Name;
};

MAKE_DH_JSON_FIELD_NAMES(FHttpAreaObjName, lang, Lang, name, Name)

USTRUCT()
struct FHttpAreaObj
{
	GENERATED_BODY()

		UPROPERTY()
		FString AreaName;
	UPROPERTY()
		FString Url;
	UPROPERTY()
		int32 Area = 0;
	UPROPERTY()
		TArray<FHttpAreaObjName> Names;
	UPROPERTY()
		TArray<FString> Addrs;
	UPROPERTY()
		int32 State = 0;
	UPROPERTY()
		FString UpdateUrl;
	UPROPERTY()
		FString LogUrl;
};

MAKE_DH_JSON_FIELD_NAMES(FHttpAreaObj, areaname, AreaName, url, Url, area, Area, names, Names, addrs, Addrs, state, State, updateurl, UpdateUrl, logurl, LogUrl)

USTRUCT()
struct FHttpList
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		FString UserAgreement;
	UPROPERTY()
		FString PrivacyPolicy;
	UPROPERTY()
		TArray<FHttpAreaObj> List;
};

MAKE_DH_JSON_FIELD_NAMES(FHttpList, type, Type, useragreement, UserAgreement, privacypolicy, PrivacyPolicy, list, List)

USTRUCT()
struct FPS4GoodsUnit
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		TMap<FString, int> Items;
};

MAKE_DH_JSON_FIELD_NAMES(FPS4GoodsUnit, type, Type, items, Items)

USTRUCT()
struct FTeamMember
{
	GENERATED_BODY()

		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		bool IsOnline = false;
	UPROPERTY()
		bool Ready = false;
	UPROPERTY()
		bool IsTemp = false;
};

MAKE_DH_JSON_FIELD_NAMES(FTeamMember, charid, CharId, name, Name, isonline, IsOnline, ready, Ready, istemp, IsTemp)

USTRUCT()
struct FBulletinUrl
{
	GENERATED_BODY()

		UPROPERTY()
		int32 ButtonId = 0;
	UPROPERTY()
		FString Url;
};

MAKE_DH_JSON_FIELD_NAMES(FBulletinUrl, buttonid, ButtonId, url, Url)

USTRUCT()
struct FClientBulletinInfo
{
	GENERATED_BODY()

		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		FString Ver;
	UPROPERTY()
		FString Lang;
	UPROPERTY()
		int64 BeginTime = 0;
	UPROPERTY()
		int64 EndTime = 0;
	UPROPERTY()
		int64 EventTime = 0;
	UPROPERTY()
		TArray<FBulletinUrl> Urls;
	UPROPERTY()
		FString Title;
	UPROPERTY()
		FString Text;
};

MAKE_DH_JSON_FIELD_NAMES(FClientBulletinInfo, id, Id, type, Type, ver, Ver, lang, Lang, begintime, BeginTime, endtime, EndTime, eventtime, EventTime, urls, Urls, title, Title, text, Text)

USTRUCT()
struct FClientBulletinList
{
	GENERATED_BODY()

		UPROPERTY()
		TArray<FClientBulletinInfo> List;
	UPROPERTY()
		int64 ServerTime = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FClientBulletinList, list, List, servertime, ServerTime)

USTRUCT()
struct FGunPatingUseInfo
{
	GENERATED_BODY()

		UPROPERTY()
		int64 Id = 0;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		int32 Code = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FGunPatingUseInfo, id, Id, excelid, ExcelId, code, Code)

USTRUCT()
struct FItemBuyRsp
{
	GENERATED_BODY()

		UPROPERTY()
		int32 ItemId = 0;
	UPROPERTY()
		int64 ItemGuid = 0;
};

MAKE_DH_JSON_FIELD_NAMES(FItemBuyRsp, itemid, ItemId, itemguid, ItemGuid)


USTRUCT()
struct FCMD_Server2Server_Heartbeat : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return Server2Server_Heartbeat;
	}
};

USTRUCT()
struct FCMD_Server2Server_SayHello : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		FString LocIp;
	UPROPERTY()
		FString PubIp;
	UPROPERTY()
		FString Ver;
	UPROPERTY()
		TArray<int> Params;
	UPROPERTY()
		FString Params2;
	UPROPERTY()
		TArray<uint8> Params3;

	virtual int GetID()const override
	{
		return Server2Server_SayHello;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Server2Server_SayHello, id, Id, type, Type, locip, LocIp, pubip, PubIp, ver, Ver, params, Params, params2, Params2, params3, Params3)

USTRUCT()
struct FCMD_Server2Server_SayHelloRsp : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 GId = 0;
	UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		FString Ver;
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		TArray<int> Params;
	UPROPERTY()
		FString Params2;
	UPROPERTY()
		TArray<uint8> Params3;
	UPROPERTY()
		FString Addr;
	UPROPERTY()
		TArray<uint8> MainCfg;
	UPROPERTY()
		TArray<uint8> AppCfg;

	virtual int GetID()const override
	{
		return Server2Server_SayHelloRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Server2Server_SayHelloRsp, gid, GId, id, Id, type, Type, ver, Ver, result, Result, params, Params, params2, Params2, params3, Params3, addr, Addr, maincfg, MainCfg, appcfg, AppCfg)

USTRUCT()
struct FCMD_Lobby2Match_LeaveBattleUnExcepted : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Lobby2Match_LeaveBattleUnExcepted;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Lobby2Match_LeaveBattleUnExcepted, charid, CharId, battleid, BattleId)

USTRUCT()
struct FCMD_Match2Battle_AllocBattleReq : public FICmd
{
	GENERATED_BODY()
		FCMD_Match2Battle_AllocBattleReq(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 BattleId = 0;
	UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int64 CurTime = 0;
	UPROPERTY()
		TArray<FRobot> Robots;

	virtual int GetID()const override
	{
		return Match2Battle_AllocBattleReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Match2Battle_AllocBattleReq, battleid, BattleId, matchtype, MatchType, curtime, CurTime, robots, Robots)

USTRUCT()
struct FCMD_Battle2Match_AllocBattleRsp : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Battle2Match_AllocBattleRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_AllocBattleRsp, result, Result, battleid, BattleId)

USTRUCT()
struct FCMD_Match2Battle_PlayerInReq : public FICmd
{
	GENERATED_BODY()
		FCMD_Match2Battle_PlayerInReq(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int64 GroupId = 0;
	UPROPERTY()
		int32 RoleType = 0;
	UPROPERTY()
		bool Leader = false;
	UPROPERTY()
		FString Key;
	UPROPERTY()
		TArray<uint8> Data;

	virtual int GetID()const override
	{
		return Match2Battle_PlayerInReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Match2Battle_PlayerInReq, charid, CharId, groupid, GroupId, roletype, RoleType, leader, Leader, key, Key, data, Data)

USTRUCT()
struct FCMD_Battle2Match_PlayerInRsp : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Battle2Match_PlayerInRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_PlayerInRsp, result, Result, charid, CharId, battleid, BattleId)

USTRUCT()
struct FCMD_Match2Battle_PlayerOutReq : public FICmd
{
	GENERATED_BODY()
		FCMD_Match2Battle_PlayerOutReq(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;

	virtual int GetID()const override
	{
		return Match2Battle_PlayerOutReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Match2Battle_PlayerOutReq, charid, CharId)

USTRUCT()
struct FCMD_Battle2Match_PlayerOutRsp : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Battle2Match_PlayerOutRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_PlayerOutRsp, result, Result, charid, CharId, battleid, BattleId)

USTRUCT()
struct FCMD_Battle2Match_PlayerResultReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int64 BattleId = 0;
	UPROPERTY()
		FFightResult Result {};
	UPROPERTY()
		FFightTeamInfo TeamInfo {};

	virtual int GetID()const override
	{
		return Battle2Match_PlayerResultReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_PlayerResultReq, charid, CharId, battleid, BattleId, result, Result, teaminfo, TeamInfo)

USTRUCT()
struct FCMD_Match2Battle_PlayerResultRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_Match2Battle_PlayerResultRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int64 CharId = 0;

	virtual int GetID()const override
	{
		return Match2Battle_PlayerResultRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Match2Battle_PlayerResultRsp, result, Result, charid, CharId)

USTRUCT()
struct FCMD_Battle2Match_PlayerConnectNotify : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Battle2Match_PlayerConnectNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_PlayerConnectNotify, charid, CharId, battleid, BattleId)

USTRUCT()
struct FCMD_Battle2Match_PlayerDisconnectNotify : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Battle2Match_PlayerDisconnectNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_PlayerDisconnectNotify, charid, CharId, battleid, BattleId)

USTRUCT()
struct FCMD_Battle2Match_CanWorkNotify : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Battle2Match_CanWorkNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_CanWorkNotify, battleid, BattleId)

USTRUCT()
struct FCMD_Battle2Match_BeginFightNotify : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return Battle2Match_BeginFightNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_BeginFightNotify, battleid, BattleId)

USTRUCT()
struct FCMD_Match2Battle_CutdownNofity : public FICmd
{
	GENERATED_BODY()
		FCMD_Match2Battle_CutdownNofity(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 BattleId = 0;
	UPROPERTY()
		int32 Seconds = 0;

	virtual int GetID()const override
	{
		return Match2Battle_CutdownNofity;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Match2Battle_CutdownNofity, battleid, BattleId, seconds, Seconds)

USTRUCT()
struct FCMD_Battle2Match_SyncStatus : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString Msg;

	virtual int GetID()const override
	{
		return Battle2Match_SyncStatus;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_Battle2Match_SyncStatus, msg, Msg)

USTRUCT()
struct FCMD_Match2Battle_ForceExit : public FICmd
{
	GENERATED_BODY()
		FCMD_Match2Battle_ForceExit(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}

	virtual int GetID()const override
	{
		return Match2Battle_ForceExit;
	}
};

USTRUCT()
struct FCMD_BattleMgr2Match_BattleSrvAddr : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 BattleId = 0;
	UPROPERTY()
		FString Addr;

	virtual int GetID()const override
	{
		return BattleMgr2Match_BattleSrvAddr;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_BattleMgr2Match_BattleSrvAddr, battleid, BattleId, addr, Addr)

USTRUCT()
struct FCMD_LuaPublish_MatchBattleNotify : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return LuaPublish_MatchBattleNotify;
	}
};

USTRUCT()
struct FCMD_LuaPublish_DoExitBattleNotify : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString CharId;
	UPROPERTY()
		FString BattleId;

	virtual int GetID()const override
	{
		return LuaPublish_DoExitBattleNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_LuaPublish_DoExitBattleNotify, charid, CharId, battleid, BattleId)

USTRUCT()
struct FCMD_C2S_GMReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString Cmd;
	UPROPERTY()
		FGMAddItem AddItem {};
	UPROPERTY()
		FGMSetFlag SetFlag {};
	UPROPERTY()
		int64 Timestamp = 0;
	UPROPERTY()
		FGMUserDefine UserDefine {};

	virtual int GetID()const override
	{
		return C2S_GMReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_GMReq, cmd, Cmd, additem, AddItem, setflag, SetFlag, timestamp, Timestamp, userdefine, UserDefine)

USTRUCT()
struct FCMD_C2S_VerifyReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString Acct;
	UPROPERTY()
		FString Token;

	virtual int GetID()const override
	{
		return C2S_VerifyReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_VerifyReq, acct, Acct, token, Token)

USTRUCT()
struct FCMD_S2C_VerifyRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_VerifyRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Acct;
	UPROPERTY()
		FString Addr;
	UPROPERTY()
		FString Okey;
	UPROPERTY()
		int32 BanTime = 0;
	UPROPERTY()
		FString BanReason;

	virtual int GetID()const override
	{
		return S2C_VerifyRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_VerifyRsp, result, Result, charid, CharId, acct, Acct, addr, Addr, okey, Okey, bantime, BanTime, banreason, BanReason)

USTRUCT()
struct FCMD_C2S_VerifyOkeyReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString Okey;
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Acct;
	UPROPERTY()
		FString Ver;

	virtual int GetID()const override
	{
		return C2S_VerifyOkeyReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_VerifyOkeyReq, okey, Okey, charid, CharId, acct, Acct, ver, Ver)

USTRUCT()
struct FCMD_S2C_VerifyOkeyRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_VerifyOkeyRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Pwd;
	UPROPERTY()
		FString Addr;

	virtual int GetID()const override
	{
		return S2C_VerifyOkeyRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_VerifyOkeyRsp, result, Result, charid, CharId, pwd, Pwd, addr, Addr)

USTRUCT()
struct FCMD_S2C_SynWaitInfo : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynWaitInfo(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Wait = 0;

	virtual int GetID()const override
	{
		return S2C_SynWaitInfo;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynWaitInfo, wait, Wait)

USTRUCT()
struct FCMD_C2S_LoginReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Pwd;
	UPROPERTY()
		FString Lang;
	UPROPERTY()
		bool IsPlus = false;

	virtual int GetID()const override
	{
		return C2S_LoginReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_LoginReq, charid, CharId, pwd, Pwd, lang, Lang, isplus, IsPlus)

USTRUCT()
struct FCMD_S2C_LoginRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_LoginRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_LoginRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_LoginRsp, result, Result)

USTRUCT()
struct FCMD_C2S_CreateCharReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		int32 Sex = 0;
	UPROPERTY()
		int32 Race = 0;
	UPROPERTY()
		TMap<FString, int> Flags;

	virtual int GetID()const override
	{
		return C2S_CreateCharReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_CreateCharReq, charid, CharId, name, Name, sex, Sex, race, Race, flags, Flags)

USTRUCT()
struct FCMD_S2C_CreateCharRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_CreateCharRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_CreateCharRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_CreateCharRsp, result, Result)

USTRUCT()
struct FCMD_S2C_SynCharInfo : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynCharInfo(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		TArray<uint8> Data;

	virtual int GetID()const override
	{
		return S2C_SynCharInfo;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynCharInfo, charid, CharId, data, Data)

USTRUCT()
struct FCMD_S2C_KickNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_KickNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		FString Reason;

	virtual int GetID()const override
	{
		return S2C_KickNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_KickNotify, reason, Reason)

USTRUCT()
struct FCMD_S2C_SynCharInfoModify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynCharInfoModify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		TArray<uint8> Data;

	virtual int GetID()const override
	{
		return S2C_SynCharInfoModify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynCharInfoModify, type, Type, data, Data)

USTRUCT()
struct FCMD_S2C_SynCharInfoDel : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynCharInfoDel(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		int64 Id = 0;

	virtual int GetID()const override
	{
		return S2C_SynCharInfoDel;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynCharInfoDel, type, Type, id, Id)

USTRUCT()
struct FCMD_S2C_SynCfgNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynCfgNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		TArray<FCfgObj> Cfg;

	virtual int GetID()const override
	{
		return S2C_SynCfgNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynCfgNotify, cfg, Cfg)

USTRUCT()
struct FCMD_S2C_SynTimeNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynTimeNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 Timestamp = 0;

	virtual int GetID()const override
	{
		return S2C_SynTimeNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynTimeNotify, timestamp, Timestamp)

USTRUCT()
struct FCMD_S2C_SynSkillNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynSkillNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		TArray<FSkillObj> Skill;

	virtual int GetID()const override
	{
		return S2C_SynSkillNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynSkillNotify, skill, Skill)

USTRUCT()
struct FCMD_S2C_AcBeginNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_AcBeginNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 TypeId = 0;
	UPROPERTY()
		int32 EndTime = 0;
	UPROPERTY()
		FString Param;

	virtual int GetID()const override
	{
		return S2C_AcBeginNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_AcBeginNotify, id, Id, typeid, TypeId, endtime, EndTime, param, Param)

USTRUCT()
struct FCMD_S2C_AcEndNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_AcEndNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Id = 0;

	virtual int GetID()const override
	{
		return S2C_AcEndNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_AcEndNotify, id, Id)

USTRUCT()
struct FCMD_C2S_PlayerInfoGetReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;

	virtual int GetID()const override
	{
		return C2S_PlayerInfoGetReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerInfoGetReq, charid, CharId)

USTRUCT()
struct FCMD_S2C_PlayerInfoGetRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_PlayerInfoGetRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		int32 Level = 0;
	UPROPERTY()
		int32 Sex = 0;
	UPROPERTY()
		int32 Race = 0;
	UPROPERTY()
		TArray<int> Flag;
	UPROPERTY()
		FSUAV UAV {};

	virtual int GetID()const override
	{
		return S2C_PlayerInfoGetRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_PlayerInfoGetRsp, charid, CharId, name, Name, level, Level, sex, Sex, race, Race, flag, Flag, uav, UAV)

USTRUCT()
struct FCMD_S2C_TextUINotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TextUINotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		TArray<FString> Arr;

	virtual int GetID()const override
	{
		return S2C_TextUINotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TextUINotify, id, Id, arr, Arr)

USTRUCT()
struct FCMD_S2C_ApplyResultNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_ApplyResultNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int64 BattleId = 0;
	UPROPERTY()
		int32 MapId = 0;
	UPROPERTY()
		FString Addr;
	UPROPERTY()
		FString Key;
	UPROPERTY()
		bool ShowOnlineId = false;
	UPROPERTY()
		int32 ShowDistance = 0;
	UPROPERTY()
		int64 CurTime = 0;

	virtual int GetID()const override
	{
		return S2C_ApplyResultNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_ApplyResultNotify, result, Result, matchtype, MatchType, battleid, BattleId, mapid, MapId, addr, Addr, key, Key, showonlineid, ShowOnlineId, showdistance, ShowDistance, curtime, CurTime)

USTRUCT()
struct FCMD_C2S_ApplyReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int32 Single = 0;

	virtual int GetID()const override
	{
		return C2S_ApplyReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_ApplyReq, matchtype, MatchType, single, Single)

USTRUCT()
struct FCMD_S2C_ApplyRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_ApplyRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_ApplyRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_ApplyRsp, matchtype, MatchType, result, Result)

USTRUCT()
struct FCMD_C2S_ApplyWolfReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int32 MapId = 0;
	UPROPERTY()
		int32 RoleType = 0;

	virtual int GetID()const override
	{
		return C2S_ApplyWolfReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_ApplyWolfReq, matchtype, MatchType, mapid, MapId, roletype, RoleType)

USTRUCT()
struct FCMD_S2C_ApplyWolfRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_ApplyWolfRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int32 RoleType = 0;

	virtual int GetID()const override
	{
		return S2C_ApplyWolfRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_ApplyWolfRsp, result, Result, matchtype, MatchType, roletype, RoleType)

USTRUCT()
struct FCMD_C2S_UnApplyWolfReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_UnApplyWolfReq;
	}
};

USTRUCT()
struct FCMD_S2C_UnApplyWolfRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_UnApplyWolfRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_UnApplyWolfRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_UnApplyWolfRsp, result, Result)

USTRUCT()
struct FCMD_C2S_UnExceptedLeaveBattleReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 BattleId = 0;

	virtual int GetID()const override
	{
		return C2S_UnExceptedLeaveBattleReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_UnExceptedLeaveBattleReq, battleid, BattleId)

USTRUCT()
struct FCMD_C2S_TeamInviteJoinReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		FString Acct;

	virtual int GetID()const override
	{
		return C2S_TeamInviteJoinReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamInviteJoinReq, charid, CharId, name, Name, acct, Acct)

USTRUCT()
struct FCMD_S2C_TeamInviteJoinRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamInviteJoinRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		FString Acct;
	UPROPERTY()
		int32 Modules = 0;

	virtual int GetID()const override
	{
		return S2C_TeamInviteJoinRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamInviteJoinRsp, result, Result, charid, CharId, name, Name, acct, Acct, modules, Modules)

USTRUCT()
struct FCMD_C2S_TeamReplyInviteJoin : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 InviterId = 0;
	UPROPERTY()
		int32 Accept = 0;

	virtual int GetID()const override
	{
		return C2S_TeamReplyInviteJoin;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamReplyInviteJoin, inviterid, InviterId, accept, Accept)

USTRUCT()
struct FCMD_S2C_TeamReplyInviteJoinRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamReplyInviteJoinRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		int32 Accept = 0;
	UPROPERTY()
		int32 Modules = 0;

	virtual int GetID()const override
	{
		return S2C_TeamReplyInviteJoinRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamReplyInviteJoinRsp, charid, CharId, name, Name, accept, Accept, modules, Modules)

USTRUCT()
struct FCMD_C2S_TeamLeaveReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_TeamLeaveReq;
	}
};

USTRUCT()
struct FCMD_C2S_TeamKickReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;

	virtual int GetID()const override
	{
		return C2S_TeamKickReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamKickReq, charid, CharId)

USTRUCT()
struct FCMD_C2S_TeamChatReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString Content;

	virtual int GetID()const override
	{
		return C2S_TeamChatReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamChatReq, content, Content)

USTRUCT()
struct FCMD_C2S_TeamDissolveReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_TeamDissolveReq;
	}
};

USTRUCT()
struct FCMD_C2S_TeamSelectTypeReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 MatchType = 0;

	virtual int GetID()const override
	{
		return C2S_TeamSelectTypeReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamSelectTypeReq, matchtype, MatchType)

USTRUCT()
struct FCMD_C2S_TeamApplyJoinReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 TeamId = 0;

	virtual int GetID()const override
	{
		return C2S_TeamApplyJoinReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamApplyJoinReq, teamid, TeamId)

USTRUCT()
struct FCMD_C2S_TeamSetCloseJoinReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Close = 0;

	virtual int GetID()const override
	{
		return C2S_TeamSetCloseJoinReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamSetCloseJoinReq, close, Close)

USTRUCT()
struct FCMD_C2S_TeamTransLeaderReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 LeaderId = 0;

	virtual int GetID()const override
	{
		return C2S_TeamTransLeaderReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_TeamTransLeaderReq, leaderid, LeaderId)

USTRUCT()
struct FCMD_C2S_ChangeClothesReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		TMap<FString, int> Flags;

	virtual int GetID()const override
	{
		return C2S_ChangeClothesReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_ChangeClothesReq, flags, Flags)

USTRUCT()
struct FCMD_S2C_ChangeClothesRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_ChangeClothesRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_ChangeClothesRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_ChangeClothesRsp, result, Result)

USTRUCT()
struct FCMD_C2S_ChangeLanguageReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString Lang;

	virtual int GetID()const override
	{
		return C2S_ChangeLanguageReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_ChangeLanguageReq, lang, Lang)

USTRUCT()
struct FCMD_S2C_ChangeLanguageRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_ChangeLanguageRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;

	virtual int GetID()const override
	{
		return S2C_ChangeLanguageRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_ChangeLanguageRsp, rst, Rst)

USTRUCT()
struct FCMD_S2C_TeamPlayerLeaveNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamPlayerLeaveNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int32 Kicked = 0;

	virtual int GetID()const override
	{
		return S2C_TeamPlayerLeaveNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamPlayerLeaveNotify, charid, CharId, kicked, Kicked)

USTRUCT()
struct FCMD_S2C_TeamPlayerJoinNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamPlayerJoinNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;

	virtual int GetID()const override
	{
		return S2C_TeamPlayerJoinNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamPlayerJoinNotify, charid, CharId)

USTRUCT()
struct FCMD_S2C_TeamChatNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamChatNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Content;

	virtual int GetID()const override
	{
		return S2C_TeamChatNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamChatNotify, charid, CharId, content, Content)

USTRUCT()
struct FCMD_S2C_TeamDissolveNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamDissolveNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 TeamId = 0;

	virtual int GetID()const override
	{
		return S2C_TeamDissolveNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamDissolveNotify, teamid, TeamId)

USTRUCT()
struct FCMD_S2C_TeamMatchTypeChangeNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamMatchTypeChangeNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 TeamId = 0;
	UPROPERTY()
		int32 MatchType = 0;

	virtual int GetID()const override
	{
		return S2C_TeamMatchTypeChangeNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamMatchTypeChangeNotify, teamid, TeamId, matchtype, MatchType)

USTRUCT()
struct FCMD_S2C_TeamPlayerReadyNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamPlayerReadyNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 TeamId = 0;
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		int32 Ready = 0;
	UPROPERTY()
		int32 Timeout = 0;

	virtual int GetID()const override
	{
		return S2C_TeamPlayerReadyNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamPlayerReadyNotify, teamid, TeamId, charid, CharId, ready, Ready, timeout, Timeout)

USTRUCT()
struct FCMD_S2C_TeamLeadeChangeNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamLeadeChangeNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 TeamId = 0;
	UPROPERTY()
		int64 OldLederId = 0;
	UPROPERTY()
		int64 NewLeaderId = 0;

	virtual int GetID()const override
	{
		return S2C_TeamLeadeChangeNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamLeadeChangeNotify, teamid, TeamId, oldlederid, OldLederId, newleaderid, NewLeaderId)

USTRUCT()
struct FCMD_S2C_TeamCloseJoinNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamCloseJoinNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 TeamId = 0;
	UPROPERTY()
		int32 Close = 0;

	virtual int GetID()const override
	{
		return S2C_TeamCloseJoinNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamCloseJoinNotify, teamid, TeamId, close, Close)

USTRUCT()
struct FCMD_S2C_TeamInviteJoinNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamInviteJoinNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		int32 Invite = 0;
	UPROPERTY()
		int32 Timeout = 0;

	virtual int GetID()const override
	{
		return S2C_TeamInviteJoinNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamInviteJoinNotify, charid, CharId, name, Name, invite, Invite, timeout, Timeout)

USTRUCT()
struct FCMD_S2C_TeamMemberNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamMemberNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int64 TeamId = 0;
	UPROPERTY()
		int64 LeaderId = 0;
	UPROPERTY()
		TArray<FTeamMember> Members;

	virtual int GetID()const override
	{
		return S2C_TeamMemberNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamMemberNotify, matchtype, MatchType, teamid, TeamId, leaderid, LeaderId, members, Members)

USTRUCT()
struct FCMD_S2C_MatchBattleNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_MatchBattleNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int32 Status = 0;

	virtual int GetID()const override
	{
		return S2C_MatchBattleNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_MatchBattleNotify, matchtype, MatchType, status, Status)

USTRUCT()
struct FCMD_S2C_BattleAwardNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_BattleAwardNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 TotalExp = 0;
	UPROPERTY()
		int32 TotalGold = 0;
	UPROPERTY()
		int32 RoleType = 0;
	UPROPERTY()
		bool IsWinner = false;
	UPROPERTY()
		int32 EndMatchReason = 0;
	UPROPERTY()
		int32 PlusGold = 0;
	UPROPERTY()
		TArray<FFightBehaviour> Behaviours;
	UPROPERTY()
		FFightTeamInfo TeamInfo {};

	virtual int GetID()const override
	{
		return S2C_BattleAwardNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_BattleAwardNotify, totalexp, TotalExp, totalgold, TotalGold, roletype, RoleType, iswinner, IsWinner, endmatchreason, EndMatchReason, plusgold, PlusGold, behaviours, Behaviours, teaminfo, TeamInfo)

USTRUCT()
struct FCMD_C2S_CreateGroupReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_CreateGroupReq;
	}
};

USTRUCT()
struct FCMD_S2C_CreateGroupRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_CreateGroupRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int64 GroupId = 0;

	virtual int GetID()const override
	{
		return S2C_CreateGroupRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_CreateGroupRsp, rst, Rst, groupid, GroupId)

USTRUCT()
struct FCMD_C2S_JoinGroupReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 GroupId = 0;

	virtual int GetID()const override
	{
		return C2S_JoinGroupReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_JoinGroupReq, groupid, GroupId)

USTRUCT()
struct FCMD_S2C_JoinGroupRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_JoinGroupRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int64 GroupId = 0;

	virtual int GetID()const override
	{
		return S2C_JoinGroupRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_JoinGroupRsp, rst, Rst, groupid, GroupId)

USTRUCT()
struct FCMD_C2S_LeaveGroupReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_LeaveGroupReq;
	}
};

USTRUCT()
struct FCMD_S2C_LeaveGroupRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_LeaveGroupRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;

	virtual int GetID()const override
	{
		return S2C_LeaveGroupRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_LeaveGroupRsp, rst, Rst)

USTRUCT()
struct FCMD_C2S_WolfWaitingReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_WolfWaitingReq;
	}
};

USTRUCT()
struct FCMD_S2C_WolfWaitingRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_WolfWaitingRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Waiting = 0;

	virtual int GetID()const override
	{
		return S2C_WolfWaitingRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_WolfWaitingRsp, waiting, Waiting)

USTRUCT()
struct FCMD_C2S_SelectModuleReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Module = 0;

	virtual int GetID()const override
	{
		return C2S_SelectModuleReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SelectModuleReq, module, Module)

USTRUCT()
struct FCMD_S2C_SelectModuleRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SelectModuleRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int32 Module = 0;

	virtual int GetID()const override
	{
		return S2C_SelectModuleRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SelectModuleRsp, result, Result, module, Module)

USTRUCT()
struct FCMD_S2C_TeamSynTeamInfo : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_TeamSynTeamInfo(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 TeamId = 0;
	UPROPERTY()
		int64 LeaderId = 0;
	UPROPERTY()
		int32 MatchType = 0;
	UPROPERTY()
		int32 CloseJoin = 0;
	UPROPERTY()
		int32 MapId = 0;
	UPROPERTY()
		TMap<FString, bool> Mem;

	virtual int GetID()const override
	{
		return S2C_TeamSynTeamInfo;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_TeamSynTeamInfo, teamid, TeamId, leaderid, LeaderId, matchtype, MatchType, closejoin, CloseJoin, mapid, MapId, mem, Mem)

USTRUCT()
struct FCMD_S2C_SupermarketBuyRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SupermarketBuyRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		TArray<FItemBuyRsp> Items;
	UPROPERTY()
		int32 Type = 0;

	virtual int GetID()const override
	{
		return S2C_SupermarketBuyRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SupermarketBuyRsp, result, Result, excelid, ExcelId, items, Items, type, Type)

USTRUCT()
struct FCMD_S2C_SupermarketGetRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SupermarketGetRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		FString Show;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		TArray<FGoodsUnit> Goods;

	virtual int GetID()const override
	{
		return S2C_SupermarketGetRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SupermarketGetRsp, show, Show, type, Type, goods, Goods)

USTRUCT()
struct FCMD_C2S_SupermarketBuyReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 GoodsId = 0;
	UPROPERTY()
		int32 Num = 0;
	UPROPERTY()
		int32 Type = 0;

	virtual int GetID()const override
	{
		return C2S_SupermarketBuyReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SupermarketBuyReq, goodsid, GoodsId, num, Num, type, Type)

USTRUCT()
struct FCMD_C2S_SupermarketGetReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Type = 0;

	virtual int GetID()const override
	{
		return C2S_SupermarketGetReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SupermarketGetReq, type, Type)

USTRUCT()
struct FCMD_S2C_PS4BuyRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_PS4BuyRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;
	UPROPERTY()
		FString Label;
	UPROPERTY()
		TMap<FString, int> Items;

	virtual int GetID()const override
	{
		return S2C_PS4BuyRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_PS4BuyRsp, result, Result, label, Label, items, Items)

USTRUCT()
struct FCMD_C2S_PS4BuyReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString Code;
	UPROPERTY()
		FString Label;
	UPROPERTY()
		int32 Paid = 0;
	UPROPERTY()
		FString Currency;

	virtual int GetID()const override
	{
		return C2S_PS4BuyReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PS4BuyReq, code, Code, label, Label, paid, Paid, currency, Currency)

USTRUCT()
struct FCMD_S2C_NotifyPs4SupermarketInfo : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_NotifyPs4SupermarketInfo(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		TArray<FPs4SupermarketInfo> Infos;

	virtual int GetID()const override
	{
		return S2C_NotifyPs4SupermarketInfo;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_NotifyPs4SupermarketInfo, infos, Infos)

USTRUCT()
struct FCMD_C2S_SetAchievementReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		int32 AddNum = 0;
	UPROPERTY()
		int32 ExData = 0;

	virtual int GetID()const override
	{
		return C2S_SetAchievementReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SetAchievementReq, excelid, ExcelId, addnum, AddNum, exdata, ExData)

USTRUCT()
struct FCMD_S2C_SetAchievementRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SetAchievementRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_SetAchievementRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SetAchievementRsp, result, Result)

USTRUCT()
struct FCMD_C2S_GetAchievementPriceReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 ExcelId = 0;

	virtual int GetID()const override
	{
		return C2S_GetAchievementPriceReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_GetAchievementPriceReq, excelid, ExcelId)

USTRUCT()
struct FCMD_S2C_GetAchievementPriceRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_GetAchievementPriceRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_GetAchievementPriceRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_GetAchievementPriceRsp, result, Result)

USTRUCT()
struct FCMD_C2S_ResetSkillTreeReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_ResetSkillTreeReq;
	}
};

USTRUCT()
struct FCMD_S2C_ResetSkillTreeRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_ResetSkillTreeRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_ResetSkillTreeRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_ResetSkillTreeRsp, result, Result)

USTRUCT()
struct FCMD_C2S_UnlockSkillReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Id = 0;

	virtual int GetID()const override
	{
		return C2S_UnlockSkillReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_UnlockSkillReq, id, Id)

USTRUCT()
struct FCMD_S2C_UnlockSkillRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_UnlockSkillRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Id = 0;
	UPROPERTY()
		int32 Result = 0;

	virtual int GetID()const override
	{
		return S2C_UnlockSkillRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_UnlockSkillRsp, id, Id, result, Result)

USTRUCT()
struct FCMD_S2C_SynCoolDownDel : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynCoolDownDel(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 ExcelId = 0;

	virtual int GetID()const override
	{
		return S2C_SynCoolDownDel;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynCoolDownDel, excelid, ExcelId)

USTRUCT()
struct FCMD_S2C_SynStateDel : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SynStateDel(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 ExcelId = 0;

	virtual int GetID()const override
	{
		return S2C_SynStateDel;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SynStateDel, excelid, ExcelId)

USTRUCT()
struct FCMD_S2C_SyncMails : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SyncMails(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		TArray<FMailUnit> Mails;

	virtual int GetID()const override
	{
		return S2C_SyncMails;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SyncMails, mails, Mails)

USTRUCT()
struct FCMD_C2S_SetGunPaintingReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		TMap<FString, int> Items;

	virtual int GetID()const override
	{
		return C2S_SetGunPaintingReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SetGunPaintingReq, items, Items)

USTRUCT()
struct FCMD_S2C_SetGunPaintingRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SetGunPaintingRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		TArray<FGunPatingUseInfo> Info;

	virtual int GetID()const override
	{
		return S2C_SetGunPaintingRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SetGunPaintingRsp, info, Info)

USTRUCT()
struct FCMD_C2S_PlayerUseItemReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 ItemId = 0;

	virtual int GetID()const override
	{
		return C2S_PlayerUseItemReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerUseItemReq, itemid, ItemId)

USTRUCT()
struct FCMD_S2C_PlayerUseItemRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_PlayerUseItemRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int32 ExcelId = 0;
	UPROPERTY()
		TMap<FString, int> Items;

	virtual int GetID()const override
	{
		return S2C_PlayerUseItemRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_PlayerUseItemRsp, rst, Rst, excelid, ExcelId, items, Items)

USTRUCT()
struct FCMD_C2S_SetItemNotNewReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 ItemId = 0;

	virtual int GetID()const override
	{
		return C2S_SetItemNotNewReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SetItemNotNewReq, itemid, ItemId)

USTRUCT()
struct FCMD_S2C_SetItemNotNewRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SetItemNotNewRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int64 ItemId = 0;
	UPROPERTY()
		int32 Rst = 0;

	virtual int GetID()const override
	{
		return S2C_SetItemNotNewRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SetItemNotNewRsp, itemid, ItemId, rst, Rst)

USTRUCT()
struct FCMD_C2S_OpenChestReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Type = 0;

	virtual int GetID()const override
	{
		return C2S_OpenChestReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_OpenChestReq, type, Type)

USTRUCT()
struct FCMD_S2C_OpenChestRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_OpenChestRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		TArray<int> Items;

	virtual int GetID()const override
	{
		return S2C_OpenChestRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_OpenChestRsp, rst, Rst, items, Items)

USTRUCT()
struct FCMD_C2S_ChestInfoReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Type = 0;

	virtual int GetID()const override
	{
		return C2S_ChestInfoReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_ChestInfoReq, type, Type)

USTRUCT()
struct FCMD_S2C_ChestInfoRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_ChestInfoRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int32 Type = 0;
	UPROPERTY()
		TArray<float> Weight;
	UPROPERTY()
		TArray<int> ItemMin;
	UPROPERTY()
		TArray<int> Item;

	virtual int GetID()const override
	{
		return S2C_ChestInfoRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_ChestInfoRsp, rst, Rst, type, Type, weight, Weight, itemmin, ItemMin, item, Item)

USTRUCT()
struct FCMD_C2S_DecomposeItemReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		TMap<FString, int> Items;

	virtual int GetID()const override
	{
		return C2S_DecomposeItemReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_DecomposeItemReq, items, Items)

USTRUCT()
struct FCMD_S2C_DecomposeItemRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_DecomposeItemRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;

	virtual int GetID()const override
	{
		return S2C_DecomposeItemRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_DecomposeItemRsp, rst, Rst)

USTRUCT()
struct FCMD_C2S_BattleVerifyReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 CharId = 0;
	UPROPERTY()
		FString Key;

	virtual int GetID()const override
	{
		return C2S_BattleVerifyReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_BattleVerifyReq, charid, CharId, key, Key)

USTRUCT()
struct FCMD_C2S_PlayerCupProcessReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		TArray<FCupInfo> CupList;

	virtual int GetID()const override
	{
		return C2S_PlayerCupProcessReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerCupProcessReq, cuplist, CupList)

USTRUCT()
struct FCMD_C2S_PlayerSetPlusReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		bool IsPlus = false;

	virtual int GetID()const override
	{
		return C2S_PlayerSetPlusReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerSetPlusReq, isplus, IsPlus)

USTRUCT()
struct FCMD_C2S_PlayerSetFlagReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Flag = 0;
	UPROPERTY()
		int32 Value = 0;

	virtual int GetID()const override
	{
		return C2S_PlayerSetFlagReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerSetFlagReq, flag, Flag, value, Value)

USTRUCT()
struct FCMD_S2C_PlayerSetFlagRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_PlayerSetFlagRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;

	virtual int GetID()const override
	{
		return S2C_PlayerSetFlagRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_PlayerSetFlagRsp, rst, Rst)

USTRUCT()
struct FCMD_C2S_MaintainGmReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		FString XmlData;

	virtual int GetID()const override
	{
		return C2S_MaintainGmReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_MaintainGmReq, xmldata, XmlData)

USTRUCT()
struct FCMD_C2S_PlayerDelMailReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		TArray<int64> OnlyIds;

	virtual int GetID()const override
	{
		return C2S_PlayerDelMailReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerDelMailReq, onlyids, OnlyIds)

USTRUCT()
struct FCMD_S2C_PlayerDelMailRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_PlayerDelMailRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int64 OnlyId = 0;

	virtual int GetID()const override
	{
		return S2C_PlayerDelMailRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_PlayerDelMailRsp, rst, Rst, onlyid, OnlyId)

USTRUCT()
struct FCMD_C2S_PlayerReadMailReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 OnlyId = 0;

	virtual int GetID()const override
	{
		return C2S_PlayerReadMailReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerReadMailReq, onlyid, OnlyId)

USTRUCT()
struct FCMD_S2C_PlayerReadMailRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_PlayerReadMailRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int64 OnlyId = 0;

	virtual int GetID()const override
	{
		return S2C_PlayerReadMailRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_PlayerReadMailRsp, rst, Rst, onlyid, OnlyId)

USTRUCT()
struct FCMD_C2S_PlayerGetMailAttachReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 OnlyId = 0;

	virtual int GetID()const override
	{
		return C2S_PlayerGetMailAttachReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_PlayerGetMailAttachReq, onlyid, OnlyId)

USTRUCT()
struct FCMD_S2C_PlayerGetMailAttachRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_PlayerGetMailAttachRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int64 OnlyId = 0;
	UPROPERTY()
		TMap<FString, int> Items;

	virtual int GetID()const override
	{
		return S2C_PlayerGetMailAttachRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_PlayerGetMailAttachRsp, rst, Rst, onlyid, OnlyId, items, Items)

USTRUCT()
struct FCMD_C2S_ActiveBattlepass : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int32 Rst = 0;

	virtual int GetID()const override
	{
		return C2S_ActiveBattlepass;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_ActiveBattlepass, rst, Rst)

USTRUCT()
struct FCMD_C2S_AddQuestSlotNum : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 QuestId = 0;
	UPROPERTY()
		int32 SlotId = 0;
	UPROPERTY()
		int32 Count = 0;

	virtual int GetID()const override
	{
		return C2S_AddQuestSlotNum;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_AddQuestSlotNum, questid, QuestId, slotid, SlotId, count, Count)

USTRUCT()
struct FCMD_C2S_SetQuestSlotNum : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 QuestId = 0;
	UPROPERTY()
		int32 SlotId = 0;
	UPROPERTY()
		int32 Count = 0;

	virtual int GetID()const override
	{
		return C2S_SetQuestSlotNum;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SetQuestSlotNum, questid, QuestId, slotid, SlotId, count, Count)

USTRUCT()
struct FCMD_C2S_GetQuestAward : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		TArray<int64> QuestId;

	virtual int GetID()const override
	{
		return C2S_GetQuestAward;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_GetQuestAward, questid, QuestId)

USTRUCT()
struct FCMD_S2C_CurrentQuestStatus : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_CurrentQuestStatus(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 WeekQuest_Week = 0;

	virtual int GetID()const override
	{
		return S2C_CurrentQuestStatus;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_CurrentQuestStatus, weekquest_week, WeekQuest_Week)

USTRUCT()
struct FCMD_S2C_BPExp2GoldNotify : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_BPExp2GoldNotify(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Exp = 0;
	UPROPERTY()
		int32 Gold = 0;

	virtual int GetID()const override
	{
		return S2C_BPExp2GoldNotify;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_BPExp2GoldNotify, exp, Exp, gold, Gold)

USTRUCT()
struct FCMD_C2S_SetUAVFlagReq : public FICmd
{
	GENERATED_BODY()
		UPROPERTY()
		int64 UAVId = 0;
	UPROPERTY()
		int32 FlagType = 0;
	UPROPERTY()
		int32 Value = 0;

	virtual int GetID()const override
	{
		return C2S_SetUAVFlagReq;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_C2S_SetUAVFlagReq, uavid, UAVId, flagtype, FlagType, value, Value)

USTRUCT()
struct FCMD_S2C_SetUAVFlagRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_SetUAVFlagRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 Rst = 0;
	UPROPERTY()
		int64 UAVId = 0;
	UPROPERTY()
		int32 FlagType = 0;
	UPROPERTY()
		int32 Value = 0;

	virtual int GetID()const override
	{
		return S2C_SetUAVFlagRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_SetUAVFlagRsp, rst, Rst, uavid, UAVId, flagtype, FlagType, value, Value)

USTRUCT()
struct FCMD_C2S_BPEndAfterSecReq : public FICmd
{
	GENERATED_BODY()

		virtual int GetID()const override
	{
		return C2S_BPEndAfterSecReq;
	}
};

USTRUCT()
struct FCMD_S2C_BPEndAfterSecRsp : public FICmd
{
	GENERATED_BODY()
		FCMD_S2C_BPEndAfterSecRsp(const FString& Data = "")
	{
		if (Data.Len() > 0)DHJsonUtils::FromJson(Data, this);
	}
	UPROPERTY()
		int32 EndAfter = 0;

	virtual int GetID()const override
	{
		return S2C_BPEndAfterSecRsp;
	}
};
MAKE_DH_JSON_FIELD_NAMES(FCMD_S2C_BPEndAfterSecRsp, endafter, EndAfter)
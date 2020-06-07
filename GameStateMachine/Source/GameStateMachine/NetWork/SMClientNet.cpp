
#include "PWClientNet.h"
#include "PWGameDataConfig.h"
#include "DETNet/DETNetClient.h"
#include "DETNet/DETNetMsgEvent.h"
#include "PWProcedureManager.h"
#include "PWSaveGameManager.h"
#include "PWLogManager.h"
#include "Client/PWProcedurePolicy.h"
#include "Client/PWProcedureLobby.h"
#include "PWPromptManager.h"
#include "PWUIManager.h"
#include "PWQueueWidget.h"
#include "PWOfflineNoticeWidget.h"
#include "PWGameInstance.h"
#include "Client/PWProcedureBattle.h"
#include "PWTrophyMananger.h"
#include "PWDelegateManager.h"
#include "PWShopManager.h"
#include "DETNet/DETNetStruct.h"
#include "DETNet/DETNet/DETNetBattle.h"
#include "PWCharacterData_Runtime.h"
#include "Lobby/PWStartGame.h"
#if PLATFORM_PS4
#   include "Online/PS4/DHOnlinePS4.h"
#endif
#include "Client/PWProcedureExitBattle.h"
#include "System/Mail/PWMailManager.h"
#include "Client/PWProcedurePostLogin.h"
#include "Online/DHEndpointTracker.h"
#include "PWStorageManager.h"
#include "Client/PWProcedureLoginBase.h"
#include "PWLobbyTeamManager.h"
#include "Werewolf/Lobby/PWLobbyWidgetV2.h"
#include "TimerManager.h"
#include "PWNetDefine.h"
#include "PWLobbyDataDefine.h"
#include "PWLobbyDataManager.h"
#include "PWShopNetManagerV2.h"
#include "PWQuest/PWQuestManager.h"
#include "PWInventoryNetManagerV2.h"
#include "PWBattlePassNetManagerV2.h"
#include "DHInternational.h"
#include "PWActivityManager.h"
#include "PWQuest/PWWeekQuestMgr.h"
#include "PWQuest/PWActivityQuestMgr.h"
#include "PWBattlePassManager.h"

#define USE_NEW_BATTLEPASSLOBBYDATA 1

void UPWClientNet::Init()
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::Init exec"));
	_RegistEventHandler();
	_RegistMsgHandler();
	// @FIXME 修复 Url
	auto Config = GetInstanceOfJson<FPWGameDataConfig>();
	if (Config)
	{
		DETNetClient::Instance().SetDefaultAnnounceUrl(Config->DefaultAnnouncementUrl, Config->Version);
	}
	_bInited = true;
}

void UPWClientNet::Close()
{
	DETNetClient::Instance().Close();
	_IsLoginSended = false;
}

/////////////////////////Requirement/////////////////////////////////////////////////
void UPWClientNet::LoginReq(const FString& Account, const FString& Token, bool PlusAccess, const FString& Lang)
{
	return_if_true(_IsLoginSended || _bInited == false);
	DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_StartEntry);
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::LoginReq Account = %s, Token = %s, PlusAccess = %d"), *Account, *Token, PlusAccess ? 1 : 0);
	//#if PLATFORM_PS4
	//    int AreaID = 1;
	//    switch (UDHOnlinePS4::GetInstance(this)->GetOnlineEnvironment()) {
	//        case EOnlineEnvironment::Certification:
	//            AreaID = 1;
	//            break;
	//        case EOnlineEnvironment::Production:
	//            AreaID = 2;
	//            break;
	//        case EOnlineEnvironment::Development:
	//            AreaID = 3;
	//            break;
	//    }
	//    DETNetClient::Instance().SetAreadID(AreaID, {});
	//#endif
	DETNetClient::Instance().ConnectLogin(Account, Token, PlusAccess, Lang);
	_IsLoginSended = true;
}

void UPWClientNet::CreateCharReq(const FString& Name, int32 Sex, int32 Race, const TArray<int32>& BodyPartIds)
{
	if (!_bInited)
		return;
	DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndCreateCharacter,
		{ ETrackFieldName::Name, Name }, { ETrackFieldName::Sex, Sex }
	);
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::CreateCharReq"));
	FCMD_C2S_CreateCharReq req;
	req.CharId = DETNetClient::Instance().GetCharId();
	req.Name = Name;
	req.Sex = Sex;
	req.Race = Race;
	for (int32 index = 0; index < (int32)EBodyPart::MAX; ++index)
	{
		req.Flags.FindOrAdd(FString::FromInt(index)) = BodyPartIds[index];
	}
	req.Flags.FindOrAdd(FString::FromInt(30)) = 3300000;

	UPWProcedurePolicy* PolicyProcedure = Cast<UPWProcedurePolicy>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_Policy));
	if (PolicyProcedure)
	{
		req.Flags.FindOrAdd(FString::FromInt(player_flag32_user_agreement_version)) = PolicyProcedure->GetUserAgreementVersion();
		req.Flags.FindOrAdd(FString::FromInt(player_flag32_privacy_version)) = PolicyProcedure->GetPrivacyPolicyVersion();
	}

	DETNetClient::Instance().Send(req);
}

void UPWClientNet::ModifyPlayerFlagValue(int32 Flag, int32 NewValue)
{
	FCMD_C2S_PlayerSetFlagReq Req;
	Req.Flag = Flag;
	Req.Value = NewValue;
	DETNetClient::Instance().Send(Req);
}

void UPWClientNet::ApplyReq(int32 MatchType, int32 Single)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::ApplyReq MatchType = %d, Single = %d"), MatchType, Single);
	FCMD_C2S_ApplyReq req;
	req.MatchType = MatchType;
	req.Single = Single;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::ApplyWolfReq(int32 CharacterType)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::ApplyWolfReq CharacterType = %d"), CharacterType);
	FCMD_C2S_ApplyWolfReq req;
	req.RoleType = CharacterType;
	DETNetClient::Instance().Send(req);
}


void UPWClientNet::UnapplyWolfReq()
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::UnapplyWolfReq"));
	FCMD_C2S_UnApplyWolfReq req;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::SelectModuleReq(int32 Module)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SelectModuleReq Moudle = %d"), Module);
	FCMD_C2S_SelectModuleReq req;
	req.Module = Module;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::LeaveBattleUnExcepted(int64 InBattleID)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::LeaveBattleUnExcepted...........InBattleID = %llu"), InBattleID);
	FCMD_C2S_UnExceptedLeaveBattleReq req;
	req.BattleId = InBattleID;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::GetPlayerInfoReq(int64 InCharID)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::GetPlayerInfoReq CharID = %llu"), InCharID);
	FCMD_C2S_PlayerInfoGetReq req;
	req.CharId = InCharID;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::ChangeClothsReq(TMap<FString, int>& Flags)
{
	if (!_bInited)
		return;
	if (Flags.Num() > 0)
	{
		for (auto& Pair : Flags)
		{
			PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::ChangeClothsReq Num = %d, First Item BodyPartString = %s, ItemID = %d"), Flags.Num(), *Pair.Key, Pair.Value);
			break;
		}
	}
	else
	{
		PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::ChangeClothsReq Num = %d And Now Return"), Flags.Num());
		return;
	}

	FCMD_C2S_ChangeClothesReq req;
	req.Flags = Flags;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::ChangeWeaponSkinReq(TMap<int64, int32>& InItems)
{
	if (!_bInited)
		return;
#if WITH_EDITOR
	TArray<int64> ItemKeys1;
	InItems.GenerateKeyArray(ItemKeys1);
	for (auto key : ItemKeys1)
	{
		PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::ChangeWeaponSkinReq  num %d ItemID = %llu, code result = %d"), ItemKeys1.Num(), key, InItems[key]);
	}
#endif
	TArray<int64> ItemKeys;
	FCMD_C2S_SetGunPaintingReq req;
	InItems.GenerateKeyArray(ItemKeys);
	for (auto key : ItemKeys)
	{
		req.Items.Add(FString::Printf(TEXT("%lld"), key), InItems[key]);
	}
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::PS4ChargeSuccess(const FString& code, const FString& label)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::PS4ChargeSuccess code = %s, label = %s"), *code, *label);

	FCMD_C2S_PS4BuyReq req;
	req.Code = code;
	req.Label = label;
	DETNetClient::Instance().Send(req);
}

//void UPWClientNet::ChangeLanguageReq(const FString& Lang)
//{
//	if (!_bInited)
//		return;
//
//	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::ChangeLanguageReq Lang = %s"), *Lang);
//
//	FCMD_C2S_ChangeLanguageReq req;
//	req.Lang = Lang;
//	DETNetClient::Instance().Send(req);
//}

void UPWClientNet::GetShopInfoReq(const EPWLobbyShopType InShopType)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::GetShopInfoReq"));
	FCMD_C2S_SupermarketGetReq req;
	req.Type = (int32)(InShopType);
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::BuyInShop(int32 ID)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::BuyInShop ID = %d"), ID);
	FCMD_C2S_SupermarketBuyReq req;
	req.GoodsId = ID;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::BuyBattlePassLevel(int32 ID, int32 Num)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("BuyBattlePassLevel ID = %d,Num = %d"), ID, Num);
	FCMD_C2S_SupermarketBuyReq req;
	req.GoodsId = ID;
	req.Num = Num;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::SendSetUAVFlagReq(int32 InCompanionID, int32 InSkinItemID)
{
	if (!_bInited)
	{
		return;
	}

	PW_LOG(LogPWNet, Log, TEXT("SendChangeCompanionSkin CompanionID = %d, InSkinItemID = %d"), InCompanionID, InSkinItemID);
	FCMD_C2S_SetUAVFlagReq Request;

	// According design by the server team, FlagType is relevant to index of array FSUAV::Flags. 
	// And now 0 represents the index of Companion Skin.
	Request.FlagType = 0;

	Request.UAVId = InCompanionID;
	Request.Value = InSkinItemID;

	DETNetClient::Instance().Send(Request);
}

void UPWClientNet::OpenChestReq(int32 Type)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OpenChestReq Type = %d"), Type);

	FCMD_C2S_OpenChestReq Req;
	Req.Type = Type;
	DETNetClient::Instance().Send(Req);
}

void UPWClientNet::GetChestInfoReq(int32 Type)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::GetChestInfoReq Type = %d"), Type);

	FCMD_C2S_ChestInfoReq Req;
	Req.Type = Type;
	DETNetClient::Instance().Send(Req);
}

void UPWClientNet::SendPlayerUseItem(int64 id)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendPlayerUseItem ID = %d"), id);
	FCMD_C2S_PlayerUseItemReq req;
	req.ItemId = id;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::SendSetItemNotNew(int64 id)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendSetItemNotNew ID = %d"), id);
	FCMD_C2S_SetItemNotNewReq req;
	req.ItemId = id;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::SendDonateReq(const TMap<FString, int>& Items)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendDonateRequest"));
	FCMD_C2S_DecomposeItemReq Req;
	Req.Items = Items;
	DETNetClient::Instance().Send(Req);
}

void UPWClientNet::SendDelMailReq(TArray<int64>& MailIDs)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendDelMailReq"));
	int32 count = MailIDs.Num();
	for (int32 i = 0; i < count; i++)
	{
		PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendDelMailReq ID = %lld"), MailIDs[i]);
	}
	FCMD_C2S_PlayerDelMailReq req;
	req.OnlyIds.Append(MailIDs);
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::SendPlayerReadMailReq(int64 MailID)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendPlayerReadMailReq ID = %lld"), MailID);
	FCMD_C2S_PlayerReadMailReq req;
	req.OnlyId = MailID;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::SendPlayerGetMailAttachReq(int64 MailID)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendPlayerGetMailAttachReq ID = %lld"), MailID);
	FCMD_C2S_PlayerGetMailAttachReq req;
	req.OnlyId = MailID;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::AddItemReq(int32 ItemID, int32 Num)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::AddItemReq ItemID = %d, ItemNum = %d"), ItemID, Num);
	FCMD_C2S_GMReq req;
	req.Cmd = "additem";
	req.AddItem.Id = ItemID;
	req.AddItem.Num = Num;

	DETNetClient::Instance().Send(req);
}

void UPWClientNet::SetServerTimestamp(int64 inTimestamp)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SetServerTimestamp Timestamp = %lld"), inTimestamp);
	FCMD_C2S_GMReq req;
	req.Cmd = "settimestamp";
	req.Timestamp = inTimestamp;

	DETNetClient::Instance().Send(req);
}
void UPWClientNet::GMUDReq(FString CmdID, int32 Param1, int32 Param2)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::GMUDReq CmdID = %s Param1=%d Param2=%d"), *CmdID, Param1, Param2);
	FCMD_C2S_GMReq req;
	req.Cmd = "userdefine";
	req.UserDefine.CmdId = CmdID;
	req.UserDefine.Param1 = Param1;
	req.UserDefine.Param2 = Param2;

	DETNetClient::Instance().Send(req);
}


void UPWClientNet::SendWaitingReq()
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendWaitingReq"));
	FCMD_C2S_WolfWaitingReq req;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::CreateTeamReq()
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::CreateTeamReq exec"));
	FCMD_C2S_CreateGroupReq req;
	DETNetClient::Instance().Send(req);
}

void UPWClientNet::JoinTeamReq(int64 TeamID)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::JoinTeamReq TeamID = %lld"), TeamID);

	FCMD_C2S_JoinGroupReq Request;
	Request.GroupId = TeamID;

	DETNetClient::Instance().Send(Request);
}

void UPWClientNet::LeaveTeamReq()
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::LeaveTeamReq exec"));

	FCMD_C2S_LeaveGroupReq Request;
	DETNetClient::Instance().Send(Request);
}

void UPWClientNet::AddQuestSlotNum(int64 QuestID, int32 SlotID, int32 Count)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::AddQuestSlotNum QuestID = %lld, SlotID = %d, Count = %d"), QuestID, SlotID, Count);
	FCMD_C2S_AddQuestSlotNum Request;
	Request.QuestId = QuestID;
	Request.SlotId = SlotID;
	Request.Count = Count;
	DETNetClient::Instance().Send(Request);
}

void UPWClientNet::SetQuestSlotNum(int64 QuestID, int32 SlotID, int32 Count)
{
	if (!_bInited)
		return;
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SetQuestSlotNum QuestID = %lld, SlotID = %d, Count = %d"), QuestID, SlotID, Count);
	FCMD_C2S_SetQuestSlotNum Request;
	Request.QuestId = QuestID;
	Request.SlotId = SlotID;
	Request.Count = Count;
	DETNetClient::Instance().Send(Request);
}

void UPWClientNet::GetQuestAward(const TArray<int64>& QuestList)
{
	if (!_bInited)
		return;
	FString Quest;
	for (int64 QuestID : QuestList)
	{
		Quest = Quest + FString::Printf(TEXT(" %lld,"), QuestID);
	}
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::GetQuestAward QuestList = %s"), *Quest);
	FCMD_C2S_GetQuestAward Request;
	Request.QuestId.Append(QuestList);
	DETNetClient::Instance().Send(Request);
}

void UPWClientNet::SendServerAcquireTrophy(TArray<TrophyProcessItem>& TrophyProcessList)
{
	if (!_bInited)
		return;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::SendServerAcquireTrophy"));
	FCMD_C2S_PlayerCupProcessReq req;
	int Num = TrophyProcessList.Num();
	for (int32 i = 0; i < Num; i++)
	{
		TrophyProcessItem Item = TrophyProcessList[i];
		FCupInfo info;
		info.Id = Item.Get<0>();
		info.Type = Item.Get<1>();
		info.Process = Item.Get<2>();
		info.IsFinish = Item.Get<3>();
		req.CupList.Add(info);
	}
	//for (auto& Item : InTrophyInfoMap)
	//{
	//	FCupTypeInfo Info;
	//	Info.CupType = (int32)Item.Key;
	//	Info.Process = Item.Value;
	//	req.CupTypeList.Add(Info);
	//}
	//req.CupIds.Append(InActiveTrophyList);
	DETNetClient::Instance().Send(req);
}

///////////////////////////////EventHandler///////////////////////////////////////////
void UPWClientNet::OnHttpRequestDone()
{
	/*PW_LOG(LogPWNet, Log, TEXT("OnHttpRequestDone exec"));
	// -- wzt 数据打点开始---
	FString LogUrl = DETNetClient::Instance().GetLogAddress();
	UPWSaveGameManager* SaveGameManager = UPWSaveGameManager::Get(GWorld);
	if (SaveGameManager == nullptr)
	{
		PW_LOG(LogPWNet, Error, TEXT("OnHttpRequestDone error, failed to get SaveGameManager"));
		return;
	}
	FString AdverID = SaveGameManager->GetOtherData()->GetAdvertisingId();
	if (AdverID.IsEmpty()) {
		FGuid id;
		FPlatformMisc::CreateGuid(id);
		AdverID = id.ToString();
		// save adverID
		SaveGameManager->GetOtherData()->SetAdvertisingId(AdverID);
	}

	PWAnalytics* AnalyLog = PWAnalytics::Instance();
	AnalyLog->Init(LogUrl, AdverID);
	AnalyLog->StartSession();
	FString category = UPWProcedureBase::GetStringFromProcedure(ProcedureState::ProcedureState_Update);
	AnalyLog->LogEvent(category, EAnalyticsAction::AA_GameStart);
	// -- wzt ---

	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(GWorld);
	if (ProcedureManager == nullptr)
	{
		PW_LOG(LogPWNet, Error, TEXT("OnHttpRequestDone error, failed to get ProcedureManager"));
		return;
	}
#if !PLATFORM_PS4
	if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Update)
	{
		UPWProcedureUpdate* ProcedureUpdate = Cast<UPWProcedureUpdate>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Update));
		ProcedureUpdate->RequestUpdate();
	}
#endif
	UPWProcedurePolicy* ProcedurePolicy = Cast<UPWProcedurePolicy>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Policy));
	if(ProcedurePolicy)
	{
		ProcedurePolicy->InitHttpAddr(DETNetClient::Instance().UserAgreementUrl, DETNetClient::Instance().PrivacyPolicyUrl);
	}*/
}

void UPWClientNet::OnCharInfoModified(int32 Type, int64 Value)
{
	PW_LOG(LogPWNet, Log, TEXT("OnCharInfoModified Type = %d, Value = %lld"), Type, Value);

	UPWProcedureLobby* ProcedureLobby = Cast<UPWProcedureLobby>(UPWProcedureManager::GetInstance(GetWorld())->GetProcedure(ProcedureState::ProcedureState_Lobby));
	if (!ProcedureLobby)
	{
		return;
	}

	const PlayerData& Data = DETNetClient::Instance().Data;
	// 分类详见 DETNetMsgDeal.cpp 中的 DealSynCharInfoModify()
	switch (Type)
	{
	case VNT_CharName:
		break;
	case VNT_Level:
		break;
	case VNT_Exp:
		break;
	case VNT_Sex:
	{
		ProcedureLobby->UpdatePlayerSex(Data);
		break;
	}
	case VNT_Race:
		break;
	case VNT_Money:
	case VNT_MoneyBound:
	case VNT_Gold:
	case VNT_GoldBound:
	case VNT_LegendGold:
		UPWDelegateManager::Get(this)->OnMoneyModified.Broadcast();
		break;
	case VNT_Flagint32:
	{
		// 此处的 Value ≈ BodyPart，若为 30 即狼人皮肤
		ProcedureLobby->ModifyFlagInt32(Value, Data.Flags[Value]);
#if USE_NEW_BATTLEPASSLOBBYDATA
		if (UPWInventoryNetManagerV2::Get(this))
		{
			UPWInventoryNetManagerV2::Get(this)->UpdateInventorytDetails();
		}
#else
		if (UPWStorageManager::Get(this))
		{
			UPWStorageManager::Get(this)->UpdateStorageDetails();
		}
		UPWDelegateManager::Get(this)->OnStorageItemModified.Broadcast();
#endif
		break;
	}
	case VNT_ItemList:
	{
#if USE_NEW_BATTLEPASSLOBBYDATA
		if (UPWInventoryNetManagerV2::Get(this))
		{
			UPWInventoryNetManagerV2::Get(this)->UpdateInventorytDetails();
		}
#else
		if (UPWStorageManager::Get(this))
		{
			UPWStorageManager::Get(this)->UpdateStorageDetails();
		}
		UPWDelegateManager::Get(this)->OnStorageItemModified.Broadcast();
#endif
		break;
	}
	case VNT_AchList:
	{
		break;
	}
	case VNT_CDList:
	{
		break;
	}
	case VNT_SkillList:
	{
		break;
	}
	case VNT_StateList:
	{
		UPWDelegateManager::Get(this)->OnPlayerStateListModified.Broadcast();
		break;
	}
	case VNT_QuestList:
	{
		OnQuestUpdated(Value);
		break;
	}
	case VNT_APList:
	{
		OnAdditionPurchaseUpdated(Value);
		break;
	}
	case VNT_UAVList:
	{
		OnCompanionUpdated(Value);
		break;
	}
	}
}

void UPWClientNet::OnCharInfoDeleted(int32 Type, int64 Value)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnCharInfoDeleted Type = %d, Value = %lld"), Type, Value);

	switch (Type)
	{
	case VNT_CharName:
	case VNT_Level:
	case VNT_Exp:
	case VNT_Sex:
	case VNT_Race:
	case VNT_Money:
	case VNT_MoneyBound:
	case VNT_Gold:
	case VNT_GoldBound:
	case VNT_LegendGold:
	case VNT_Flagint32:
	case VNT_ItemList:
#if USE_NEW_BATTLEPASSLOBBYDATA
	{
		if (UPWInventoryNetManagerV2::Get(this))
		{
			UPWInventoryNetManagerV2::Get(this)->UpdateInventorytDetails();
		}
	}
	break;
#endif // USE_NEW_BATTLEPASSLOBBYDATA
	case VNT_AchList:
	case VNT_CDList:
	case VNT_SkillList:
	case VNT_StateList:
		break;
	case VNT_QuestList:
	{
		UPWQuestManager::GetInstance(this)->DeleteQuestData(Value);
		break;
	}
	case VNT_APList:
	{
		UPWLobbyDataManager::Get(this)->RemoveAdditionPurchaseData(Value);
		break;
	}
	}
}

void UPWClientNet::OnDropped(int32 Reason)
{
#if PLATFORM_PS4
	UDHOnlinePS4::GetInstance(this)->Store_HidePsStoreIcon();
#endif
	PW_LOG(LogTemp, Log, TEXT("UPWClientNet::OnDropped Reason = %d"), Reason);
	UPWLobbyTeamManager::GetInstance(this)->OnDropped();
#if PLATFORM_PS4
	FString ErrorMessage;
	if (Reason == DropType_Normal || Reason == DropType_GetHttpFailed)
	{
		ErrorMessage = DH_LOCTABLE_STR("GL_MSG_NET_UNAVAILABLE");
		UPWUIManager::Get(GetWorld())->SetOfflineReason(TEXT("GL_MSG_DISCONNECT"), Reason);
	}
	else
	{
		ErrorMessage = DH_LOCTABLE_STR("GL_MSG_FLOGIN");
		TArray<FStringFormatArg> ArgList;
		ArgList.Add(Reason);
		ErrorMessage = FString::Format(*ErrorMessage, ArgList);
	}
	UDHOnlinePS4::ShowErrorMessageDialog(ErrorMessage,
		UDHOnlinePS4::FShowMessageDialogDelegate::CreateLambda([this](EDHAppReturnType ReturnType) {
			UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
			if (ProcedureManager)
			{
				if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Battle)
				{
					UPWProcedureExitBattle* ExitBattle = Cast<UPWProcedureExitBattle>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_ExitBattle));
					if (ExitBattle)
					{
						ExitBattle->SetNextState(ProcedureState::ProcedureState_Login);
					}
					ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_ExitBattle);
				}
				else if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_ExitBattle)
				{
					UPWProcedureExitBattle* ExitBattle = Cast<UPWProcedureExitBattle>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_ExitBattle));
					if (ExitBattle)
					{
						ExitBattle->SetNextState(ProcedureState::ProcedureState_Login);
					}
				}
				else
				{
					ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_Login);
				}
			}

			}));
#else
	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
	if (ProcedureManager)
	{
		if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Battle)
		{
			UPWProcedureExitBattle* ExitBattle = Cast<UPWProcedureExitBattle>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_ExitBattle));
			if (ExitBattle)
			{
				ExitBattle->SetNextState(ProcedureState::ProcedureState_Login);
			}
			ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_ExitBattle);
		}
		else if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_ExitBattle)
		{
			UPWProcedureExitBattle* ExitBattle = Cast<UPWProcedureExitBattle>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_ExitBattle));
			if (ExitBattle)
			{
				ExitBattle->SetNextState(ProcedureState::ProcedureState_Login);
			}
		}
		else
		{
			ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_Login);
		}
	}
#endif
}

void UPWClientNet::OnStopServiceRefreshed()
{
	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
	if (!ProcedureManager)
	{
		return;
	}

	if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Lobby)
	{
		UPWProcedureLobby* ProcLobby = Cast<UPWProcedureLobby>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Lobby));
		if (ProcLobby)
		{
			ProcLobby->CheckStopServiceMsg();
		}
	}
	else if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Battle)
	{
		UPWProcedureBattle* ProcBattle = Cast<UPWProcedureBattle>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Battle));
		if (ProcBattle)
		{
			ProcBattle->CheckStopServiceMsg();
		}
	}
}

void UPWClientNet::OnBattlePassBulletinRefreshed()
{
	UPWBattlePassNetManagerV2* NetManager = UPWBattlePassNetManagerV2::GetInstance(this);
	if (NetManager)
	{
		NetManager->OnBattlePassBulletinRefreshed();
	}
}

void UPWClientNet::OnBulletinRefreshed()
{
	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
	if (!ProcedureManager)
	{
		return;
	}

	if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Login)
	{
		UPWProcedureLoginBase* ProcLogin = Cast<UPWProcedureLoginBase>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Login));
		if (ProcLogin)
		{
			ProcLogin->OnBulletinRefreshed();
		}
	}
}

void UPWClientNet::OnAdvanceNoticeRefreshed()
{
	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
	if (!ProcedureManager)
	{
		return;
	}

	if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Lobby)
	{
		UPWProcedureLobby* ProcLogin = Cast<UPWProcedureLobby>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Lobby));
		if (ProcLogin)
		{
			ProcLogin->CheckAdvanceNotice();
		}
	}
}

void UPWClientNet::_RegistEventHandler()
{
	DETNetMsgEvent::RegHttpEvent([this] {
		OnHttpRequestDone();
		});
	DETNetMsgEvent::RegModifyEvent([this](int32 Type, int64 Value) {
		OnCharInfoModified(Type, Value);
		});
	DETNetMsgEvent::RegDelEvent([this](int32 Type, int64 Value) {
		OnCharInfoDeleted(Type, Value);
		});

	DETNetMsgEvent::RegDropEvent([this](int32 Reason) {
		OnDropped(Reason);
		});
	DETNetMsgEvent::RegBulletinEvent([this] {
		OnBulletinRefreshed();
		});
	DETNetMsgEvent::RegAdvanceNoticeEvent([this] {
		OnAdvanceNoticeRefreshed();
		});
	DETNetMsgEvent::RegStopServiceEvent([this] {
		OnStopServiceRefreshed();
		});
	DETNetMsgEvent::RegBattlePassBulletinBuEvent([this] {
		OnBattlePassBulletinRefreshed();
		});
}


///////////////////////////////MsgHandler///////////////////////////////////////////
void UPWClientNet::OnSynWaitInfo(const FString& Data)
{
	FCMD_S2C_SynWaitInfo rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnSynWaitInfo wait = %d"), rsp.Wait);
	int32 Index = rsp.Wait;

	TWeakObjectPtr<UPWUIManager> UIManager = UPWUIManager::Get(GetWorld());

	if (UPWProcedureManager::GetInstance(GetWorld())->GetCurState() != ProcedureState::ProcedureState_Login)
	{
		UIManager->HideUI(EUMGID::EUMGID_Queue);
		return;
	}

	UPWQueueWidget* QueueWidget = Cast<UPWQueueWidget>(UIManager->GetWidget(EUMGID::EUMGID_Queue));
	if (QueueWidget)
	{
		QueueWidget->UpdateIndex(Index);
	}
	else
	{
		FPWUIOpenDelegate Del = FPWUIOpenDelegate::CreateLambda([UIManager, Index]()
			{
				if (!UIManager.IsValid() || !UPWProcedureManager::GetInstance(UIManager.Get()))
				{
					return;
				}

				if (UPWProcedureManager::GetInstance(UIManager.Get())->GetCurState() == ProcedureState::ProcedureState_Login)
				{
					UPWQueueWidget* widget = Cast<UPWQueueWidget>(UIManager->GetWidget(EUMGID::EUMGID_Queue));
					if (widget)
					{
						widget->UpdateIndex(Index);
					}
				}
				else
				{
					UIManager->HideUI(EUMGID::EUMGID_Queue);
				}
			});
		UIManager->ShowUI(EUMGID::EUMGID_Queue, Del);
	}
}

void UPWClientNet::OnLoginRsp(int Rst)
{
	PW_LOG(LogPWNet, Log, TEXT("OnLoginRsp Rst = %d"), Rst);

	auto ProcedureMgr = UPWProcedureManager::GetInstance(GetWorld());

	if (Rst < (int32)Login_Rst::LoginRst_Ok)
	{
		/*UPWOfflineNoticeWidget::SetOfflineReason(EOffLineReason::EOffLineReason_LoginFailed);
		UPWOfflineNoticeWidget::SetReasonCode(Rst);
		UPWUIManager::Get(GetWorld())->ShowUI(EUMGID::EUMGID_Offline);*/

		//if (Rst == -1 || Rst == -22)//这个两个提示无服务器可用（返回码）
		//{
		//	UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_NoServer"));
		//}
		//else if (Rst == -11) // 版本错误
		//{
		//	UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_VERSION"));
		//}
		//else if (Rst == -2)//被禁止登录
		//{
		//	UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_LoginFailed"),Rst);
		//}
		//else if (Rst == -21) // 角色已在线
		//{
		//	UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_ReplacedByOthers"));
		//}
		//else
		//{
		//	UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_LoginFailed"), Rst);
		//}
		if (Rst == LoginRst_Ver)
		{
			UPWUIManager::Get(GetWorld())->ShowOfflineReason(TEXT("GL_MSG_ERROR"));
		}
		else
		{
			UPWUIManager::Get(GetWorld())->ShowOfflineReason(TEXT("GL_MSG_FLOGIN"), Rst);
		}
		return;
	}

	UPWProcedurePolicy* ProcedurePolicy = Cast<UPWProcedurePolicy>(ProcedureMgr->GetProcedure(ProcedureState::ProcedureState_Policy));
	if (ProcedurePolicy == nullptr)
	{
		PW_LOG(LogPWNet, Error, TEXT("OnLoginRsp Error Failed to Get ProcedurePolicy"));
		return;
	}

	UPWProcedurePostLogin* ProcedurePostLogin = Cast<UPWProcedurePostLogin>(UPWProcedureManager::GetInstance(GetWorld())->GetProcedure(ProcedureState::ProcedureState_PostLogin));
	if (ProcedurePostLogin == nullptr)
	{
		PW_LOG(LogPWNet, Error, TEXT("OnLoginRsp Error Failed to Get ProcedurePostLogin"));
		return;
	}

	if (Rst == LoginRst_NoChar)
	{
#if WITH_EDITOR
		ProcedurePostLogin->CanSkipVideo = true;
#else
		ProcedurePostLogin->CanSkipVideo = false;
#endif
		ProcedurePolicy->SetNextState(ProcedureState::ProcedureState_CreateChar);
		ProcedurePostLogin->SetNextState(ProcedureState::ProcedureState_Policy);
		UPWProcedureManager::GetInstance(GetWorld())->ChangeCurState(ProcedureState::ProcedureState_PostLogin);
	}
	else if (Rst == LoginRst_Ok)
	{
		ProcedurePostLogin->CanSkipVideo = true;
		UPWProcedureLobby* ProcedureLobby = Cast<UPWProcedureLobby>(UPWProcedureManager::GetInstance(GetWorld())->GetProcedure(ProcedureState::ProcedureState_Lobby));
		if (ProcedureLobby)
		{
			ProcedureLobby->InitSelfPlayerInfo(DETNetClient::Instance().Data);
		}
		ProcedureState CurState = UPWProcedureManager::GetInstance(GetWorld())->GetCurState();
		if (CurState == ProcedureState::ProcedureState_Login)
		{
			UPWProcedureLobby* ProcedureLobby = Cast<UPWProcedureLobby>(UPWProcedureManager::GetInstance(GetWorld())->GetProcedure(ProcedureState::ProcedureState_Lobby));
			if (ProcedureLobby)
			{
				//PWAnalytics::Instance()->SetUniqueID(FString::Printf(TEXT("%llu"), DETNetClient::Instance().Data.CharId));
			}
			ProcedurePolicy->SetNextState(ProcedureState::ProcedureState_Lobby);
			if (DETNetClient::Instance().Data.Flags.IsValidIndex(player_flag32_user_agreement_version))
			{
				ProcedurePolicy->SetPolicyVersionFromServer(player_flag32_user_agreement_version, DETNetClient::Instance().Data.Flags[player_flag32_user_agreement_version]);
			}
			if (DETNetClient::Instance().Data.Flags.IsValidIndex(player_flag32_privacy_version))
			{
				ProcedurePolicy->SetPolicyVersionFromServer(player_flag32_privacy_version, DETNetClient::Instance().Data.Flags[player_flag32_privacy_version]);
			}
			ProcedurePostLogin->SetNextState(ProcedureState::ProcedureState_Policy);
			UPWProcedureManager::GetInstance(GetWorld())->ChangeCurState(ProcedureState::ProcedureState_PostLogin);
			UPWTrophyMananger::GetInstance(GetWorld())->InitData(DETNetClient::Instance().Data);
		}
		else
		{
			ProcedurePostLogin->SetNextState(ProcedureState::ProcedureState_Lobby);
			UPWProcedureManager::GetInstance(GetWorld())->ChangeCurState(ProcedureState::ProcedureState_PostLogin);
		}
	}
}

void UPWClientNet::OnCreateCharRsp(const FString& Data)
{
	FCMD_S2C_CreateCharRsp rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnCreateCharRsp Rst = %d"), rsp.Result);
	if (rsp.Result != 0)
	{
		UPWPromptBase* widget = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (widget)
		{
			FString Content = FString::Printf(TEXT("Create Char ErrorCode = %d"), rsp.Result);
			widget->SetPromptContent(FText::FromString(Content));
		}
	}
}

void UPWClientNet::OnKickNotify(const FString& Data)
{
	FCMD_S2C_KickNotify rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnKickNotify Reason = %s"), *rsp.Reason);
}

void UPWClientNet::OnApplyResultNotify(const FString& Data)
{
	FCMD_S2C_ApplyResultNotify rsp(Data);
	int64 TimeStamp = rsp.CurTime;
	FDateTime DataTime = FDateTime::FromUnixTimestamp(TimeStamp);
	PW_LOG(LogPWNet, Log, TEXT("OnApplyResultNotify Rst = %d MatchType = %d BattleId = %llu MapId = %d Addr = %s Token = %s, ShowOnlineId = %s, ShowDistance = %d, Time = %lld( %s)"),
		rsp.Result, rsp.MatchType, rsp.BattleId, rsp.MapId, *rsp.Addr, *rsp.Key, rsp.ShowOnlineId ? TEXT("true") : TEXT("false"), rsp.ShowDistance, TimeStamp, *DataTime.ToString());

	if (rsp.Result == 0)
	{
		UPWGameInstance* GameInstance = UPWGameInstance::Get(GetWorld());
		if (GameInstance)
		{
			GameInstance->SetMapID(rsp.MapId);
			GameInstance->SetBattleStartTime(rsp.CurTime);
		}
		UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
		UPWProcedureBattle* BattleProcedure = Cast<UPWProcedureBattle>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Battle));
		BattleProcedure->SetDSAddr(rsp.Addr);
		BattleProcedure->SetToken(rsp.Key);
		BattleProcedure->SetBattleId(rsp.BattleId);
		BattleProcedure->SetShowEnemyInfo(rsp.ShowOnlineId);
		BattleProcedure->SetEnemyInfoVisibleDistance((float)rsp.ShowDistance);
		ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_Battle);

		FTimerHandle TimerHandle;
		UPWLobbyTeamManager* TeamManager = UPWLobbyTeamManager::GetInstance(this);
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([TeamManager]() {
			return_if_null(TeamManager);
			TeamManager->ClearReadyStateOfAllMember();
			});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
	}
	else
	{
		//UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_Server"),rsp.Result);
		UPWPromptBase* widget = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (widget)
		{
			FString Content = FString::Printf(TEXT("ErrorCode = %d"), rsp.Result);
			widget->SetPromptContent(FText::FromString(Content));
		}
	}
}

void UPWClientNet::OnApplyRsp(const FString& Data)
{
	FCMD_S2C_ApplyRsp rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnApplyRsp Rst = %d MatchType = %d"), rsp.Result, rsp.MatchType);

	if (rsp.Result == 0)
	{
		UPWLobbyTeamManager::GetInstance(this)->SetSelfReady(true);

		//UPWProcedureLobby* Lobby = Cast<UPWProcedureLobby>(UPWProcedureManager::Procedure(this, ProcedureState::ProcedureState_Lobby));
		//if (Lobby)
		//{
			//Lobby->SetTeamMatchType(rsp.MatchType);
			//Lobby->ChangeMatchingState(true);
		//}
	}
	else
	{
		/*	UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_Server"), rsp.Result); */
		UPWPromptBase* widget = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (widget)
		{
			FString Content = FString::Printf(TEXT("ErrorCode = %d"), rsp.Result);
			widget->SetPromptContent(FText::FromString(Content));
		}
	}
}

void UPWClientNet::OnApplyWolfRsp(const FString& Data)
{
	FCMD_S2C_ApplyWolfRsp rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("FCMD_S2C_ApplyWolfRsp Rst = %d, MatchType = %d RoleType = %d"), rsp.Result, rsp.MatchType, rsp.RoleType);

	UPWLobbyTeamManager* TeamManager = UPWLobbyTeamManager::GetInstance(this);

	UPWProcedureLobby* Lobby = Cast<UPWProcedureLobby>(UPWProcedureManager::Procedure(this, ProcedureState::ProcedureState_Lobby));

	if (rsp.Result == 0) // 正常匹配
	{
		return_if_null(TeamManager);
		TeamManager->SetSelfReady(true);
	}
	else
	{
		UPWPromptBase* widget = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (widget)
		{
			FString Content = FString::Printf(TEXT("ErrorCode = %d"), rsp.Result);
			widget->SetPromptContent(FText::FromString(Content));
		}
	}
}

void UPWClientNet::OnTeamMemberNotify(const FString& Data)
{
	FCMD_S2C_TeamMemberNotify rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("FCMD_S2C_TeamMemberNotify Rst"));

	UPWLobbyTeamManager* TeamManager = UPWLobbyTeamManager::GetInstance(this);
	/*return_if_null(TeamManager);

	const TArray<FPWLobbyTeamMember>& MemberList = TeamManager->GetTeamMemberList();

	for (const FPWLobbyTeamMember& Member : MemberList)
	{
		bool bInTeam = rsp.Members.ContainsByPredicate([Member](const FTeamMember& Item)
		{
			return Member.PlayerInfo.CharID == Item.CharId;
		});
		if (!bInTeam)
		{
			TeamManager->OnTeamLeave(Member.PlayerInfo.CharID);
		}
	}*/
	int32 nFrindCount = rsp.Members.Num();
	TeamManager->OnInitTeamInfo(rsp.TeamId, rsp.LeaderId);

	TArray<FPWLobbyTeamMember> NewTeam;
	for (int32 i = 0; i < nFrindCount; i++)
	{
		FTeamMember TeamMember = rsp.Members[i];
		FPWLobbyTeamMember Member;
		Member.PlayerInfo.CharID = TeamMember.CharId;
		Member.bIsReady = TeamMember.Ready;
		Member.PlayerInfo.Name = TeamMember.Name;
		PW_LOG(LogTemp, Log, TEXT("UPWClientNet::OnTeamMemberNotify CharID = %llu Name = %s IsReady = %s"), TeamMember.CharId, *TeamMember.Name, TeamMember.Ready ? TEXT("true") : TEXT("false"));
		NewTeam.Add(Member);
	}
	TeamManager->OnTeamMemberChanged(NewTeam);
	//UPWLobbyWidgetV2::ForceJumpToHomeByTeamChange(this);
}

void UPWClientNet::OnMatchBattleNotify(const FString& Data)
{
	// Team match state:
	// 0 -> at least one of your teamate is not ready
	// 1 -> waiting for other teamates
	// 2 -> matching
	// 3 -> matching for werewolf
	FCMD_S2C_MatchBattleNotify rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnMatchBattleNotify Rst, rsp.Status = %d"), rsp.Status);
	UPWLobbyTeamManager* TeamManager = UPWLobbyTeamManager::GetInstance(this);

	if (TeamManager)
	{
		switch (rsp.Status)
		{
		case 0:
			TeamManager->OnChangeMatchState(LobbyMatchStateType::Lobby_TeamCancel);
			break;
		case 1:
			TeamManager->OnChangeMatchState(LobbyMatchStateType::Lobby_TeamStart);
			break;
		case 2:
		case 3:
			TeamManager->OnChangeMatchState(LobbyMatchStateType::Lobby_PlayStart);
			break;
		default:
			break;
		}
	}
}


void UPWClientNet::OnTeamDissolveNotify(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("OnTeamDissolveNotify Data = %s"), *Data);
	FCMD_S2C_TeamDissolveNotify rsp(Data);
	UPWLobbyTeamManager::GetInstance(this)->OnTeamDissolveNotify();
}

void UPWClientNet::OnUnApplyWolfRsp(const FString& Data)
{
	FCMD_S2C_UnApplyWolfRsp rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnUnApplyWolfRsp Rst = %d"), rsp.Result);

	if (rsp.Result == 0 || rsp.Result == -3)
	{
		UPWLobbyTeamManager* TeamManager = UPWLobbyTeamManager::GetInstance(this);
		if (TeamManager)
		{
			TeamManager->SetSelfReady(false);
			if (TeamManager->GetTeamMemberCount() > 1)
			{
				//Lobby->OnTeamDissolve(false);
			}
			TeamManager->OnChangeMatchState(LobbyMatchStateType::Lobby_TeamCancel);
		}
	}
	else
	{
		UPWPromptBase* widget = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (widget)
		{
			FString Content = FString::Printf(TEXT("ErrorCode = %d"), rsp.Result);
			widget->SetPromptContent(FText::FromString(Content));
		}
	}
}

void UPWClientNet::OnPlayerInfoRsp(const FString& Data)
{
	FCMD_S2C_PlayerInfoGetRsp Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnPlayerInfoRsp, succeed CharID = %llu, name = %s"), Rsp.CharId, *Rsp.Name);

	UPWLobbyTeamManager* TeamManager = UPWLobbyTeamManager::GetInstance(this);
	if (TeamManager)
	{
		TeamManager->OnGetPlayerInfo(Rsp);
	}
}
void UPWClientNet::OnSelectMoudleRsp(const FString& Data)
{
	FCMD_S2C_SelectModuleRsp rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnSelectMoudleRsp Rst = %d Module = %d"), rsp.Result, rsp.Module);

	if (rsp.Result == 0)
	{
		UPWProcedureLobby* LobbyProcedure = Cast<UPWProcedureLobby>(UPWProcedureManager::Procedure(this, ProcedureState::ProcedureState_Lobby));
		if (LobbyProcedure)
		{
			LobbyProcedure->SelectGameMoudles(rsp.Module);
		}
	}
	else
	{
		//UPWUIManager::Get(GetWorld())->ShowErrorUI(TEXT("ERROR_Server"), rsp.Result);
		UPWPromptBase* widget = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (widget)
		{
			FString Content = FString::Printf(TEXT("ErrorCode = %d"), rsp.Result);
			widget->SetPromptContent(FText::FromString(Content));
		}
	}
}

void UPWClientNet::OnWeaponSkinNotify(const FString& Data)
{
	FCMD_S2C_SetGunPaintingRsp rsp(Data);
#ifdef USE_NEW_BATTLEPASSLOBBYDATA
	if (UPWInventoryNetManagerV2::Get(this))
	{
		TArray<FLobbyWeaponSkinInfoStruct> GunPaintingDataInfo;
		for (FGunPatingUseInfo GunPUI : rsp.Info)
		{
			FLobbyWeaponSkinInfoStruct TmpNewInfor;
			TmpNewInfor.Id = GunPUI.Id;
			TmpNewInfor.ItemID = GunPUI.ExcelId;
			TmpNewInfor.Code = GunPUI.Code;
			GunPaintingDataInfo.Add(TmpNewInfor);
			PW_LOG(LogPWNet, Log, TEXT("OnWeaponSkinNotify, Rst guid= %lld, ItemID %d, result code %d"), GunPUI.Id, GunPUI.ExcelId, GunPUI.Code);
		}
		UPWInventoryNetManagerV2::Get(this)->InventoryWeaponSkinChanged(GunPaintingDataInfo);
	}
#endif
	//PW_LOG(LogPWNet, Log, TEXT("OnWeaponSkinNotify, Rst = %d"), rsp.Rst);

}

void UPWClientNet::OnDecomposeItemRsp(const FString& Data)
{
	FCMD_S2C_DecomposeItemRsp Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnWeaponSkinNotify, Rst = %d"), Rsp.Rst);
	// TODO: 弹窗
}

void UPWClientNet::OnGetShopInfoRsp(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("OnGetShopInfoRsp, Data = %s"), *Data);

	FCMD_S2C_SupermarketGetRsp rsp(Data);
	//PW_LOG(LogPWNet, Log, TEXT("OnGetShopInfoRsp, GoodsNum = %d"), rsp.Goods.Num());
#if USE_NEW_BATTLEPASSLOBBYDATA
	UPWShopNetManagerV2* ShopNetManager = UPWShopNetManagerV2::Get(this);
	if (ShopNetManager)
	{
		FLobbyShopDataStruct LobbyShopData;
		for (FGoodsUnit& GoodsUnit : rsp.Goods)
		{
			FPWLobbyShopInfo ShopInfor = PWLOBBYDATASTRUCT_HELPER::GoodsUnitToLobbyShopInfor(GoodsUnit);
			LobbyShopData.ShopDataList.Add(ShopInfor);
		}
		LobbyShopData.ShopType = (EPWLobbyShopType)rsp.Type;
		// 显示Widget Style Index.
		int32 WidgetStyleIndex = FCString::Atoi(*rsp.Show);
		LobbyShopData.WidgetStyleIndex = WidgetStyleIndex;
		ShopNetManager->InitShop(LobbyShopData);
	}
#else
	UPWShopManager* ShopManager = UPWShopManager::Get(this);
	if (ShopManager)
	{
		TArray<FPWShopCommodity> CommodityList;
		for (FGoodsUnit& GoodsUnit : rsp.Goods)
		{
			FPWShopCommodity Commodity;
			Commodity.Id = GoodsUnit.Id;
			Commodity.ItemID = GoodsUnit.ItemId;
			Commodity.Price = GoodsUnit.Price[0];
			Commodity.PriceType = GoodsUnit.PriceType;
			Commodity.bIsNew = GoodsUnit.IsNew == 1 ? true : false;
			Commodity.bIsHot = GoodsUnit.Hot == 1 ? true : false;
			Commodity.Discount = GoodsUnit.Discount;
			Commodity.bIsLimitQuantity = GoodsUnit.LimitCnt == 1 ? true : false;
			Commodity.QuantityLeft = GoodsUnit.LeftCnt;
			Commodity.EndTime = GoodsUnit.LimitTime;
			Commodity.bCanBeGift = GoodsUnit.Gift == 1 ? true : false;
			CommodityList.Add(Commodity);
		}

		ShopManager->InitShop(CommodityList);
	}
#endif
}

void UPWClientNet::OnBuyInShopRsp(const FString& Data)
{
	FCMD_S2C_SupermarketBuyRsp rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnBuyInShopRsp, Result = %d"), rsp.Result);
#if USE_NEW_BATTLEPASSLOBBYDATA
	UPWShopNetManagerV2* ShopNetManager = UPWShopNetManagerV2::Get(this);
	if (ShopNetManager)
	{
		ShopNetManager->ResponseDataBuyInShop(rsp.Result, rsp.Items, rsp.ExcelId, (EPWLobbyShopType)rsp.Type);
	}
#else
#endif
}

void UPWClientNet::ShopActivityStartRsp(const FString& Data)
{
	FCMD_S2C_AcBeginNotify Rsp(Data);
	UPWLobbyDataManager* LobbyDataManager = UPWLobbyDataManager::Get(this);
	return_if_null(LobbyDataManager);
	LobbyDataManager->StartDiscountActivity(Rsp.Id, EPWLobbyActivityType(Rsp.TypeId), Rsp.Param);
	LobbyDataManager->OnShopActivityStart.Broadcast(Rsp.Id, EPWLobbyActivityType(Rsp.TypeId));
}

void UPWClientNet::ShopActivityEndRsp(const FString& Data)
{
	FCMD_S2C_AcEndNotify Rsp(Data);
	UPWLobbyDataManager* LobbyDataManager = UPWLobbyDataManager::Get(this);
	return_if_null(LobbyDataManager);
	LobbyDataManager->EndDiscountActivity(Rsp.Id);
	LobbyDataManager->OnShopActivityEnd.Broadcast(Rsp.Id);
}

void UPWClientNet::OnOpenChestRsp(const FString& Data)
{
	FCMD_S2C_OpenChestRsp Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnOpenChestRsp Rst = %d,Num = %d"), Rsp.Rst, Rsp.Items.Num());

	UPWLobbyDataManager* LobbyDataManager = UPWLobbyDataManager::Get(this);
	return_if_null(LobbyDataManager);

	FLobbyLotteryResultInfo Info(Rsp.Rst, Rsp.Items);
	LobbyDataManager->OnDrawLottery.Broadcast(Info);
}

void UPWClientNet::OnGetChestInfoRsp(const FString& Data)
{
	FCMD_S2C_ChestInfoRsp Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnGetChestInfoRsp Rst = %d"), Rsp.Rst);

	UPWLobbyDataManager* LobbyDataManager = UPWLobbyDataManager::Get(this);
	return_if_null(LobbyDataManager);

	FLobbyLotteryChestItemInfo Info(Rsp.Rst, Rsp.Type, Rsp.Weight, Rsp.ItemMin, Rsp.Item);
	LobbyDataManager->OnGetLotteryChestItemsList.Broadcast(Info);

}

void UPWClientNet::OnBattleRewardNotify(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("OnBattleRewardNotify Data = %s"), *Data);

	FCMD_S2C_BattleAwardNotify rsp(Data);
	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
	if (ProcedureManager->GetCurState() == ProcedureState::ProcedureState_Battle)
	{
		UPWProcedureBattle* BattleProcedure = Cast<UPWProcedureBattle>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Battle));
		BattleProcedure->OnPlayerRewardNotify(rsp);
	}
}

void UPWClientNet::OnLobbyChargeInifoNotify(const FString& Data)
{
	FCMD_S2C_NotifyPs4SupermarketInfo rsp(Data);
#if USE_NEW_BATTLEPASSLOBBYDATA
	UPWShopNetManagerV2* ShopNetManager = UPWShopNetManagerV2::Get(this);
	if (ShopNetManager)
	{
		ShopNetManager->SetSupermarketInfoList(rsp.Infos);
	}
#else
	UPWShopManager* ShopManager = UPWShopManager::Get(this);
	if (ShopManager)
	{
		ShopManager->SetSupermarketInfoList(rsp.Infos);
	}
#endif
}

void UPWClientNet::OnLobbyChargeResult(const FString& Data)
{
	FCMD_S2C_PS4BuyRsp rsp(Data);

	UPWShopNetManagerV2* ShopNetManager = UPWShopNetManagerV2::Get(this);
	if (rsp.Result == 0)
	{
		PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnLobbyChargeResult Label = %s"), *rsp.Label);
		int32 MoneyNum = rsp.Items.FindRef(FString::FromInt(ITEM_MONEY_ID));
		if (IsValid(ShopNetManager))
		{
			ShopNetManager->ResponseDataPS4BuySucceed(rsp.Label, MoneyNum);
		}
	}
	else
	{
		PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnLobbyChargeResult Label = %s,error id = %d"), *rsp.Label, rsp.Result);
		if (IsValid(ShopNetManager))
		{
			ShopNetManager->ResponseDataPS4BuyFailed(rsp.Result, rsp.Label);
		}
	}
}


void UPWClientNet::OnSyncMails(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnSyncMails Data = %s"), *Data);
	FCMD_S2C_SyncMails rsp(Data);
	TArray<FNetMail> sendmails;
	int32 Count = rsp.Mails.Num();
	for (auto& Item : rsp.Mails)
	{
		FNetMail Mail;
		Mail.mailguid = Item.OnlyId;
		Mail.mailID = Item.MailId;
		Mail.title = Item.Title;
		Mail.content = Item.Content;
		Mail.mailsender = Item.Sender;
		Mail.filepath = Item.FilePath;
		Mail.mailType = Item.UiType;
		Mail.sendtime = Item.CreateTime;
		Mail.effectiveTime = Item.ExpireTime;
		Mail.HaveRead = Item.Status;
		for (auto& attchItem : Item.AttachList)
		{
			FMailAttach MailAttachItem;
			MailAttachItem.item_id = attchItem.Id;
			MailAttachItem.item_Type = attchItem.Type;
			MailAttachItem.item_num = attchItem.Num;
			Mail.attacharr.Add(MailAttachItem);
		}
		sendmails.Add(Mail);
	}
	UPWMailManager* MManager = UPWMailManager::GetInstance(this);
	if (MManager)
	{
		MManager->UpdateMailContentFromServer(sendmails);
		PW_LOG(LogPWNet, Log, TEXT("Mail Sended Success!"));
	}
}

void UPWClientNet::OnDelMail(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnDelMail Data = %s"), *Data);
	FCMD_S2C_PlayerDelMailRsp rsp(Data);
	UPWMailManager* MManager = UPWMailManager::GetInstance(this);
	if (MManager)
	{
		MManager->SetReturnResult(MManager->ServerValidation(0, rsp.OnlyId, rsp.Rst));
	}


}

void UPWClientNet::OnPlayerReadMail(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnPlayerReadMail Data = %s"), *Data);
	FCMD_S2C_PlayerReadMailRsp rsp(Data);
	if (rsp.Rst == 0)
	{
		UPWMailManager* MManager = UPWMailManager::GetInstance(this);
		if (MManager)
		{
			MManager->SetReturnResult(MManager->ServerValidation(2, rsp.OnlyId, rsp.Rst));
		}
	}
}

void UPWClientNet::OnPlayerGetMailAttach(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnPlayerGetMailAttach Data = %s"), *Data);
	FCMD_S2C_PlayerGetMailAttachRsp rsp(Data);
	if (rsp.Rst == 0)
	{
		UPWMailManager* MManager = UPWMailManager::GetInstance(this);
		if (MManager)
		{
			MManager->SetReturnResult(MManager->ServerValidation(1, rsp.OnlyId, rsp.Rst));
		}
	}
}

void UPWClientNet::OnPlayerUseAwardResult(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnPlayerUseAwardResult Data = %s"), *Data);
	FCMD_S2C_PlayerUseItemRsp  rsp(Data);
	if (rsp.Rst == 0)
	{
		TArray<FPWRewardInfo> Rewards;
		for (auto& pair : rsp.Items)
		{
			FPWRewardInfo info;
			info.Reward_id = FCString::Atoi(*pair.Key);
			info.Reward_num = pair.Value;
			Rewards.Emplace(info);
		}

		if (UPWPromptManager::Get(this))
		{
			UPWPromptManager::Get(this)->ShowRewardWindowWithAssociatedItemID(rsp.ExcelId, Rewards);
		}

		if (UPWDelegateManager::Get(this))
		{
			UPWDelegateManager::Get(this)->OnRecievedPlayerUseAwardResult.Broadcast(rsp.ExcelId, Rewards);
		}
	}
}

void UPWClientNet::OnSetItemNotNewResult(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnSetItemNotNewResult Data = %s"), *Data);
	FCMD_S2C_SetItemNotNewRsp  rsp(Data);
	if (rsp.Rst == 0)
	{
		UPWDelegateManager::Get(this)->OnItemNewStatusModified.Broadcast(FString::FromInt(rsp.ItemId));
#ifdef USE_NEW_BATTLEPASSLOBBYDATA
		if (UPWInventoryNetManagerV2::Get(this))
		{
			UPWInventoryNetManagerV2::Get(this)->UpdateInventorytDetails();
		}
#else
		UPWStorageManager::Get(this)->UpdateStorageDetails();
#endif
	}

}

void UPWClientNet::OnShowConsumedResult(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnShowConsumedResult Data = %s"), *Data);
	FCMD_S2C_SynCharInfoDel  rsp(Data);
	if (rsp.Id != 0)
	{
		UPWDelegateManager::Get(this)->OnStorageItemModified.Broadcast();
	}
}



void UPWClientNet::OnWolfWaitingRsp(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnWolfWaitingRsp Data = %s"), *Data);
	FCMD_S2C_WolfWaitingRsp rsp(Data);
	UPWProcedureLobby* LobbyProcedure = Cast<UPWProcedureLobby>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_Lobby));
	if (LobbyProcedure)
	{
		LobbyProcedure->AvgWaitTime = FTimespan(0, 0, rsp.Waiting);
	}
}

void UPWClientNet::CreateTeamRsp(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::CreateTeamRsp Data = %s"), *Data);

	FCMD_S2C_CreateGroupRsp  rsp(Data);
	if (rsp.Rst == 0)
	{
		UPWLobbyTeamManager::GetInstance(this)->OnTeamCreated(rsp.GroupId);
	}
	else
	{
		UPWPromptBase* widget = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (widget)
		{
			widget->SetPromptContent(LOCTABLE("Lang", "GL_MSG_CANTINVIT"));
		}
	}
}

void UPWClientNet::JoinTeamRsp(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::JoinTeamRsp Data = %s"), *Data);
	FCMD_S2C_JoinGroupRsp rsp(Data);
	UPWLobbyTeamManager::GetInstance(this)->JoinTeamRsp(rsp.Rst, rsp.GroupId);
}

void UPWClientNet::LeaveTeamRsp(const FString& Data)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::LeaveTeamRsp Data = %s"), *Data);
	FCMD_S2C_LeaveGroupRsp rsp(Data);
	UPWLobbyTeamManager::GetInstance(this)->LeaveTeamRsp(rsp.Rst);
}

void UPWClientNet::OnQuestUpdated(int64 QuestID)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnQuestUpdated QuestID = %lld"), QuestID);
	if (DETNetClient::Instance().Data.QuestList.Find(QuestID) != nullptr)
	{
		UPWQuestManager::GetInstance(this)->UpdateQuest(DETNetClient::Instance().Data.QuestList.FindRef(QuestID));
	}
}

void UPWClientNet::OnServerRspTips(const FString& Data)
{
	FCMD_S2C_TextUINotify Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("OnServerRspTips RstID = %d"), Rsp.Id);
	// 	UPWLobbyBaseManager* LobbyBaseManager = UPWLobbyBaseManager::Get(this);
	// 	return_if_null(LobbyBaseManager);
	// 	LobbyBaseManager->ShowServerResultPrompt(Rsp.Id, Rsp.Arr);
}

void UPWClientNet::OnCurrentQuestStatusUpdated(const FString& Data)
{
	FCMD_S2C_CurrentQuestStatus Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnCurrentQuestStatusUpdated Week = %d"), Rsp.WeekQuest_Week);
	UPWQuestManager::GetQuestMgr<UPWWeekQuestMgr>(this)->SetCurWeek(Rsp.WeekQuest_Week);
}

void UPWClientNet::OnCfgNotifyUpdated(const FString& Data)
{
	FCMD_S2C_SynCfgNotify rsp(Data);
	UPWLobbyDataManager* LobbyDataManager = UPWLobbyDataManager::Get(this);
	return_if_null(LobbyDataManager);
	LobbyDataManager->RefreshOnlineData();
}

void UPWClientNet::OnBPExp2GoldNotify(const FString& Data)
{
	FCMD_S2C_BPExp2GoldNotify rsp(Data);

	UPWPromptBase* Prompt = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
	if (Prompt)
	{
		FText text = FText::Format(FText::FromStringTable("Lang", "GL_MSG_EXPCHANGE"), rsp.Gold);
		Prompt->SetPromptContent(text);
	}
}

void UPWClientNet::OnBattlePassEndAfterSecReq()
{
	if (!_bInited)
	{
		return;
	}

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnBattlePassEndAfterSecReq"));
	FCMD_C2S_BPEndAfterSecReq Request;

	DETNetClient::Instance().Send(Request);
}

void UPWClientNet::OnBattlePassEndAfterSecRsp(const FString& Data)
{
	FCMD_S2C_BPEndAfterSecRsp rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnBattlePassEndAfterSecRsp BattlePass EndAfter = %d"), rsp.EndAfter);
	if (IsValid(UPWBattlePassManager::GetInstance(this)))
	{
		UPWBattlePassManager::GetInstance(this)->SetBattlePassEndAfterSecTime(rsp.EndAfter);
	}
}

void UPWClientNet::OnActivityStartRsp(const FString& Data)
{
	FCMD_S2C_AcBeginNotify Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnActivityStartRsp ActivityID = %d"), Rsp.Id);

	UPWActivityQuestMgr* ActivityQuestMgr = UPWQuestManager::GetQuestMgr<UPWActivityQuestMgr>(this);
	if (ActivityQuestMgr)
	{
		ActivityQuestMgr->OpenActivity(Rsp.Id);
	}
}

void UPWClientNet::OnActivityEndRsp(const FString& Data)
{
	FCMD_S2C_AcBeginNotify Rsp(Data);
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnActivityEndRsp ActivityID = %d"), Rsp.Id);
	UPWActivityQuestMgr* ActivityQuestMgr = UPWQuestManager::GetQuestMgr<UPWActivityQuestMgr>(this);
	if (ActivityQuestMgr)
	{
		ActivityQuestMgr->CloseActivity(Rsp.Id);
	}
}

void UPWClientNet::OnAdditionPurchaseUpdated(int64 AdditionPurchaseID)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnAdditionPurchaseUpdated AdditionPurchaseID = %lld"), AdditionPurchaseID);
	if (DETNetClient::Instance().Data.APList.Find(AdditionPurchaseID) != nullptr)
	{
		UPWLobbyDataManager::Get(this)->UpdateAdditionPurchaseData(DETNetClient::Instance().Data.APList.FindRef(AdditionPurchaseID));
	}
}

void UPWClientNet::OnCompanionUpdated(int64 CompanionID)
{
	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnCompanionUpdated CompanionID = %lld"), CompanionID);
	if (DETNetClient::Instance().Data.UAVList.Find(CompanionID) != nullptr)
	{
		UPWProcedureLobby* LobbyProcedure = Cast<UPWProcedureLobby>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_Lobby));
		if (LobbyProcedure)
		{
			LobbyProcedure->UpdateCompanionData(DETNetClient::Instance().Data.UAVList.FindRef(CompanionID));
		}
	}
}

void UPWClientNet::OnSetUAVFlagRsp(const FString& InData)
{
	FCMD_S2C_SetUAVFlagRsp Rsp(InData);
	int64 CompanionID = Rsp.UAVId;

	PW_LOG(LogPWNet, Log, TEXT("UPWClientNet::OnSetUAVFlagRsp CompanionID = %lld"), CompanionID);

	OnCompanionUpdated(CompanionID);
}

void UPWClientNet::_RegistMsgHandler()
{
	DETNetMsgEvent::Init();

	DETNetMsgEvent::Reg(S2C_SynWaitInfo, [this](const FString& Data) {
		OnSynWaitInfo(Data);
		});
	DETNetMsgEvent::RegLoginEvent([this](int Rst) {
		OnLoginRsp(Rst);
		});
	DETNetMsgEvent::Reg(S2C_CreateCharRsp, [this](const FString& Data) {
		OnCreateCharRsp(Data);
		});
	//DETNetMsgEvent::Reg(S2C_SynCharInfo, OnSynCharInfo);
	DETNetMsgEvent::Reg(S2C_KickNotify, [this](const FString& Data) {
		OnKickNotify(Data);
		});
	//DETNetMsgEvent::Reg(S2C_SynCharInfoModify, OnSynCharInfoModify);	//这个走event
	//DETNetMsgEvent::Reg(S2C_SynCharInfoDel, OnSynCharInfoDel);		//这个走event
	DETNetMsgEvent::Reg(S2C_ApplyResultNotify, [this](const FString& Data) {
		OnApplyResultNotify(Data);
		});
	DETNetMsgEvent::Reg(S2C_ApplyRsp, [this](const FString& Data) {
		OnApplyRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_ApplyWolfRsp, [this](const FString& Data) {
		OnApplyWolfRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_UnApplyWolfRsp, [this](const FString& Data) {
		OnUnApplyWolfRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_SelectModuleRsp, [this](const FString& Data) {
		OnSelectMoudleRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_TeamMemberNotify, [this](const FString& Data) {
		OnTeamMemberNotify(Data);
		});
	DETNetMsgEvent::Reg(S2C_PlayerInfoGetRsp, [this](const FString& Data) {
		OnPlayerInfoRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_MatchBattleNotify, [this](const FString& Data) {
		OnMatchBattleNotify(Data);
		});
	// 武器涂装。
	DETNetMsgEvent::Reg(S2C_SetGunPaintingRsp, [this](const FString& Data) {
		OnWeaponSkinNotify(Data);
		});
	// 捐献（军需兑换）
	DETNetMsgEvent::Reg(S2C_DecomposeItemRsp, [this](const FString& Data) {
		OnDecomposeItemRsp(Data);
		});

	// 商城
	DETNetMsgEvent::Reg(S2C_SupermarketGetRsp, [this](const FString& Data) {
		OnGetShopInfoRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_SupermarketBuyRsp, [this](const FString& Data) {
		OnBuyInShopRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_OpenChestRsp, [this](const FString& Data) {
		OnOpenChestRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_ChestInfoRsp, [this](const FString& Data) {
		OnGetChestInfoRsp(Data);
		});



	// 结算
	DETNetMsgEvent::Reg(S2C_BattleAwardNotify, [this](const FString& Data) {
		OnBattleRewardNotify(Data);
		});
	DETNetMsgEvent::Reg(S2C_NotifyPs4SupermarketInfo, [this](const FString& Data) {
		OnLobbyChargeInifoNotify(Data);
		});

	DETNetMsgEvent::Reg(S2C_TeamDissolveNotify, [this](const FString& Data) {
		OnTeamDissolveNotify(Data);
		});


	DETNetMsgEvent::Reg(S2C_PS4BuyRsp, [this](const FString& Data) {
		OnLobbyChargeResult(Data);
		});
	DETNetMsgEvent::Reg(S2C_SyncMails, [this](const FString& Data)
		{
			OnSyncMails(Data);
		});

	DETNetMsgEvent::Reg(S2C_PlayerDelMailRsp, [this](const FString& Data)
		{
			OnDelMail(Data);
		});

	DETNetMsgEvent::Reg(S2C_PlayerReadMailRsp, [this](const FString& Data)
		{
			OnPlayerReadMail(Data);
		});

	DETNetMsgEvent::Reg(S2C_PlayerGetMailAttachRsp, [this](const FString& Data)
		{
			OnPlayerGetMailAttach(Data);
		});
	//库存
	DETNetMsgEvent::Reg(S2C_PlayerUseItemRsp, [this](const FString& Data)
		{
			OnPlayerUseAwardResult(Data);
		});

	DETNetMsgEvent::Reg(S2C_SetItemNotNewRsp, [this](const FString& Data)
		{
			OnSetItemNotNewResult(Data);
		});
	DETNetMsgEvent::Reg(S2C_SynCharInfoDel, [this](const FString& Data)
		{
			OnShowConsumedResult(Data);
		});
	DETNetMsgEvent::Reg(S2C_WolfWaitingRsp, [this](const FString& Data)
		{
			OnWolfWaitingRsp(Data);
		});

	//Team
	DETNetMsgEvent::Reg(S2C_CreateGroupRsp, [this](const FString& Data)
		{
			CreateTeamRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_JoinGroupRsp, [this](const FString& Data)
		{
			JoinTeamRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_LeaveGroupRsp, [this](const FString& Data)
		{
			LeaveTeamRsp(Data);
		});
	// 商店或任务活动开始更新或发生了变化。
	DETNetMsgEvent::Reg(S2C_AcBeginNotify, [this](const FString& Data)
		{
			ShopActivityStartRsp(Data);
			// 任务活动开始响应
			OnActivityStartRsp(Data);
		});
	DETNetMsgEvent::Reg(S2C_AcEndNotify, [this](const FString& Data)
		{
			ShopActivityEndRsp(Data);
			// 任务活动结束响应
			OnActivityEndRsp(Data);
		});

	//  服务区信息返回 提示UI  TextUINotify
	DETNetMsgEvent::Reg(S2C_TextUINotify, [this](const FString& Data)
		{
			OnServerRspTips(Data);
		});

	DETNetMsgEvent::Reg(S2C_CurrentQuestStatus, [this](const FString& Data)
		{
			OnCurrentQuestStatusUpdated(Data);
		});
	/**Cfg config updated.*/
	DETNetMsgEvent::Reg(S2C_SynCfgNotify, [this](const FString& Data)
		{
			OnCfgNotifyUpdated(Data);
		});

	//battlepass满级后经验转金币
	DETNetMsgEvent::Reg(S2C_BPExp2GoldNotify, [this](const FString& Data)
		{
			OnBPExp2GoldNotify(Data);
		});

	// 跟随机器人的数据发生了改变，可能是跟随机器人本身也可能是其关联的其他数据，比如皮肤，设置在对应的Flag中。
	DETNetMsgEvent::Reg(S2C_SetUAVFlagRsp, [this](const FString& Data)
		{
			OnSetUAVFlagRsp(Data);
		});

	//BattlePass剩余时间变化
	DETNetMsgEvent::Reg(S2C_BPEndAfterSecRsp, [this](const FString& Data)
		{
			OnBattlePassEndAfterSecRsp(Data);
		});

}

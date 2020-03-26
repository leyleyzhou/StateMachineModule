// Copyright 2004-2018 Perfect World Co.,Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DHCommon.h"
#include "PWGameInstance.h"
#include "System/Trophy/PWTrophyDataTableStruct.h"
#include "DETNet/DETNetClient.h"
#include "PWLobbyDataDefine.h"
#include "PWClientNet.generated.h"

/**
 *
 */
UCLASS()
class PWGAME_API UPWClientNet : public UObject, public DHGameInstance<UPWClientNet>
{
	GENERATED_BODY()
public:
	UPWClientNet() {}

	void Init();

	void Close();

	void LoginReq(const FString& Account, const FString& Token, bool PlusAccess, const FString& Lang);
	void CreateCharReq(const FString& Name, int32 Sex, int32 Race, const TArray<int32>& BodyPartIds);
	void ModifyPlayerFlagValue(int32 Flag, int32 NewValue);
	void ApplyReq(int32 MatchType, int32 Single);
	void ApplyWolfReq(int32 CharacterType);
	void UnapplyWolfReq();
	void SelectModuleReq(int32 Module);
	void LeaveBattleUnExcepted(int64 InBattleID);
	void GetPlayerInfoReq(int64 InCharID);
	void ChangeClothsReq(TMap<FString, int>& Flags);
	/**更新武器涂装。<int64, int>-- 《GUID,0/1是否装备》*/
	void ChangeWeaponSkinReq(TMap<int64, int32>& InItems);
	void PS4ChargeSuccess(const FString& code, const FString& label);
	//void ChangeLanguageReq(const FString& Lang);
	/**
	* TrophyId 奖杯id
	* InProgress 奖杯进度
	* bActive 是否完成
	*/
	void SendServerAcquireTrophy(TArray<TrophyProcessItem>& TrophyProcessList);
	// 商城
	void GetShopInfoReq(const EPWLobbyShopType InShopType);
	void BuyInShop(int32 ID);
	void OpenChestReq(int32 Type);
	void GetChestInfoReq(int32 Type);

	//库存
	void SendPlayerUseItem(int64 id);

	void SendSetItemNotNew(int64 id);

	// 捐献（军需兑换）
	void SendDonateReq(const TMap<FString, int>& Items);


	// 邮件系统
	/**
	* @item MailID
	*/
	void SendDelMailReq(TArray<int64>& MailIDs);

	/**
	*@item MailID
	*/
	void SendPlayerReadMailReq(int64 MailID);

	/**
	*@item MailID
	*/
	void SendPlayerGetMailAttachReq(int64 MailID);


	// GM
	void AddItemReq(int32 ItemID, int32 Num);
	void SetServerTimestamp(int64 inTimestamp);
	void GMUDReq(FString CmdID, int32 Param1, int32 Param2);

	/**
	*@item Send wait time
	*/
	void SendWaitingReq();

	//Team
	void CreateTeamReq();
	void JoinTeamReq(int64 TeamID);
	void LeaveTeamReq();

	//quest
	void AddQuestSlotNum(int64 QuestID, int32 SlotID, int32 Count);
	void SetQuestSlotNum(int64 QuestID, int32 SlotID, int32 Count);
	void GetQuestAward(const TArray<int64>& QuestList);

	//battlePass
	void BuyBattlePassLevel(int32 ID, int32 Num);

	void SendSetUAVFlagReq(int32 InCompanionID, int32 InSkinItemID);

	void OnBattlePassEndAfterSecReq();

private:
	void _RegistEventHandler();
	void _RegistMsgHandler();
	uint8  _bInited = false;
private:
	void OnHttpRequestDone();
	void OnCharInfoModified(int32 Type, int64 Value);
	void OnCharInfoDeleted(int32 Type, int64 Value);
	void OnDropped(int32 Reason);
	void OnBulletinRefreshed();
	void OnAdvanceNoticeRefreshed();
	void OnStopServiceRefreshed();
	void OnBattlePassBulletinRefreshed();
	void OnSynWaitInfo(const FString& Data);
	void OnLoginRsp(int Rst);
	void OnCreateCharRsp(const FString& Data);
	void OnKickNotify(const FString& Data);
	void OnApplyResultNotify(const FString& Data);
	void OnApplyRsp(const FString& Data);
	void OnApplyWolfRsp(const FString& Data);
	void OnTeamMemberNotify(const FString& Data);
	void OnMatchBattleNotify(const FString& Data);
	void OnUnApplyWolfRsp(const FString& Data);
	void OnPlayerInfoRsp(const FString& Data);
	void OnSelectMoudleRsp(const FString& Data);

	void OnWeaponSkinNotify(const FString& Data);

	// 捐献（军需兑换）
	void OnDecomposeItemRsp(const FString& Data);

	// 商城
	void OnGetShopInfoRsp(const FString& Data);
	void OnBuyInShopRsp(const FString& Data);
	/**商城或任务活动开启。*/
	void ShopActivityStartRsp(const FString& Data);
	/**商城或任务活动结束。*/
	void ShopActivityEndRsp(const FString& Data);
	/**获取开箱结构。*/
	void OnOpenChestRsp(const FString& Data);
	/**获取宝箱信息。*/
	void OnGetChestInfoRsp(const FString& Data);

	// 结算
	void OnBattleRewardNotify(const FString& Data);

	//充值
	void OnLobbyChargeInifoNotify(const FString& Data);

	//单人匹配解散队伍
	void OnTeamDissolveNotify(const FString& Data);

	//充值失败/成功
	void OnLobbyChargeResult(const FString& Data);
	/**
	*@作用： 同步邮件
	*/
	void OnSyncMails(const FString& Data);

	/**
	*@作用: 删除邮件返回结果
	*/
	void OnDelMail(const FString& Data);
	/**
	*@作用: 阅读邮件返回结果
	*/
	void OnPlayerReadMail(const FString& Data);
	/**
	*@作用: 接收邮件附件返回结果
	*/
	void OnPlayerGetMailAttach(const FString& Data);

	/**
	*@作用: 接收使用奖励返回结果
	*/
	void OnPlayerUseAwardResult(const FString& Data);

	/**
	*@作用: 接收设置浏览过返回结果
	*/
	void OnSetItemNotNewResult(const FString& Data);

	void OnShowConsumedResult(const FString& Data);
	/**
	*@作用: 每隔60秒获取预留时间
	*/
	void OnWolfWaitingRsp(const FString& Data);

	//Team
	void CreateTeamRsp(const FString& Data);
	void JoinTeamRsp(const FString& Data);
	void LeaveTeamRsp(const FString& Data);

	//Quest
	void OnQuestUpdated(int64 QuestID);

	// 任务活动
	void OnActivityStartRsp(const FString& Data);
	void OnActivityEndRsp(const FString& Data);

	bool _IsLoginSended = false;
	/**服务器返回的提示信息。*/
	void OnServerRspTips(const FString& Data);

	void OnCurrentQuestStatusUpdated(const FString& Data);
	/**cfg Net Resource changed.*/
	void OnCfgNotifyUpdated(const FString& Data);

	void OnBPExp2GoldNotify(const FString& Data);

	//AdditionPurchase
	void OnAdditionPurchaseUpdated(int64 AdditionPurchaseID);

	void OnSetUAVFlagRsp(const FString& InData);

	void OnCompanionUpdated(int64 CompanionID);

	void OnBattlePassEndAfterSecRsp(const FString& Data);
};

// Copyright 2016 P906, Perfect World, Inc.

#include "PWProcedureLobby.h"
#include "Kismet/GameplayStatics.h"
#include "PWUIManager.h"
#include "Widget/UIWidgetHead.h"
#include "PWInventoryDataStructs.h"
#include "PWCharacterManager.h"
#include "PlayerInfo.h"
#include "PWDelegateManager.h"
#include "PWPromptManager.h"
#include "PWProcedureManager.h"
#include "PWLibrary.h"
#include "PWGameInstance.h"
#include "PWFriendsManager.h"
#include "PWLevelMgr.h"
#include "PWDataTable.h"
#include "PWCharacterLvExpDataRow.h"
#include "PWArrayUtility.h"
#include "PWDebugMessageWidget.h"
#include "PWGameSettings.h"
#include "PWLobbyWidget.h"
#if PLATFORM_PS4
#include "Online/PS4/DHOnlinePS4.h"
#endif
#include "PWLoadConnectingWidget.h"
#include "Client/PWProcedurePolicy.h"
#include "PWClientNet.h"
#include "PWStreamingLevelManager.h"
#include "DETNet/DETNet/DETNetStruct.h"
#include "PWCVar/PWGameConsoleVariables.h"
#include "TimerManager.h"
#include "DETNet/DETNetClient.h"
#include "PWVivoxManager.h"
#include "Lobby/PWStopServiceWidget.h"
#include "Online/DHEndpointTracker.h"
#include "PWSaveGameManager.h"
#include "DHInternational.h"
#include "PWQuest/PWQuestManager.h"
#include "PWLobbyTeamManager.h"
#include "PWLobbyUIManager.h"
#include "Lobby/PWLobbyBulletinWidget.h"
#include "PWActivityManager.h"
#include "PWLobbyBaseManager.h"
#include "PWLobbyDataManager.h"
#include "PWInventoryManagerV2.h"
#include "PWBattlePassManager.h"

void UPWProcedureLobby::Enter()
{
	// --- wzt 打点 ---
	DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_InLobby);
	// --- wzt ---
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Connecting);
	//这个有初始化顺序的。
	{
		UPWLobbyBaseManager::Get(this)->Initial();
		UPWLobbyDataManager::Get(this)->Initial();
		UPWInventoryManagerV2::Get(this)->Initial();
	}
	// 防止跳伞突然掉线导致的脸部被切
	GNearClippingPlane = 10.f;

	if (m_bOffline)
	{
		m_bOffline = false;
		UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_Offline);
		return;
	}
	TeamManager = UPWLobbyTeamManager::GetInstance(this);
	bDisableRefreshCharacter = false;

	if (m_bFirstTimeIntoLobby == true)
	{
		UPWDelegateManager::Get(this)->OnPS4InvationDataChanged.AddUniqueDynamic(this, &ThisClass::CheckFriendInviteData);
		CheckFriendInviteData();
		m_bFirstTimeIntoLobby = false;
		PullAdvanceNotice(); //登陆进大厅时拉取大厅公告，策划要求
	}
	else
	{
		SelectGameMoudles(m_nMoudles);
	}
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (UIManager && UIManager->IsValidLowLevel())
	{
		UIManager->HideUI(EUMGID::EUMGID_WW_PINGWidget);
	}
	if (UPWBattlePassManager::GetInstance(this))
	{
		UPWBattlePassManager::GetInstance(this)->Init();
	}
	UPWUIManager::Get(this)->PreLoadUI(EUMGID::EUMGID_WW_WaitingLoading);
	UPWUIManager::Get(this)->PreLoadUI(EUMGID::EUMGID_LobbyBulletinWidget);
	UPWUIManager::Get(this)->PreLoadNewLoadingScreen();
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(PullStopServiceHandle, this, &ThisClass::PullStopService, 60.0f, true);
		PullStopService();
	}


#if PLATFORM_PS4
	UPWClientNet* Net = UPWClientNet::GetInstance(this);
	UDHOnlinePS4* OnlinePS4 = UDHOnlinePS4::GetInstance(this);
	OnlinePS4->QueryEntitlements(UDHOnlinePS4::FGetEntitlementsDelegate::CreateLambda([OnlinePS4, Net](const TArray<FEntitlementPS4>& Entitlements) {
		return_if_true(Entitlements.Num() == 0);
		FString Token = OnlinePS4->GetAuthToken();
		for (auto& Entitlement : Entitlements) {
			if (Entitlement.RemainingCount > 0) {
				Net->PS4ChargeSuccess(Token, Entitlement.Label);
			}
		}
		}));

	UPWGameInstance* GameInstance = UPWGameInstance::Get(GetWorld());
	if (GameInstance && GameInstance->GetVivoxManager()->IsNotLoggedIn())
	{
		// Login Vivox with CharId
		GameInstance->GetVivoxManager()->Login(FString::Printf(TEXT("%llu"), DETNetClient::Instance().Data.CharId));
	}

	if (UPWLobbyTeamManager::GetInstance(this)->IsInTeam())
	{
		OnlinePS4->SetPresence(EGameStatus::EGameStatus_InTeam);
	}
	else
	{
		OnlinePS4->SetPresence(EGameStatus::EGameStatus_Lobby);
	}
#else
	// 	UPWUIManager::Get(this)->PreLoadUI(EUMGID::EUMGID_TeamReady);
	// 	UPWUIManager::Get(this)->PreLoadUI(EUMGID::EUMGID_Matching);
	// 	UPWUIManager::Get(this)->PreLoadUI(EUMGID::EUMGID_ReceivedInvite);
#endif
}

void UPWProcedureLobby::Leave()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	UIManager->HideUI(EUMGID::EUMGID_WW_Lobby);
	//     UIManager->HideUI(EUMGID::EUMGID_ThreeArymLobby);
	UIManager->HideUI(EUMGID::EUMGID_LobbyShutDownBroadcastWidget);
	UIManager->HideUI(EUMGID::EUMGID_LobbyBulletinWidget);
	//这个有顺序的。
	{
		UPWInventoryManagerV2::Get(this)->Clear();
		UPWLobbyDataManager::Get(this)->Clear();
		UPWLobbyBaseManager::Get(this)->Clear();
	}
	UPWLobbyUIManager* LobbyUIManager = UPWLobbyUIManager::Get(this);
	if (LobbyUIManager)
	{
		LobbyUIManager->HideAllUI();
		LobbyUIManager->Clear();
	}
	if (UPWBattlePassManager::GetInstance(this))
	{
		UPWBattlePassManager::GetInstance(this)->Clear();
	}

	if (AutoApplyHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoApplyHandle);
	}

	if (PullStopServiceHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PullStopServiceHandle);
	}
	if (StopServicePromptHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(StopServicePromptHandle);
	}
}

void UPWProcedureLobby::InitSelfPlayerInfo(const PlayerData& data)
{
	PWPlayerInfoManager::InitPlayerInfoFormPlayerData(&data, &(FullInfo.BasicPlayerInfo));

	UPWLobbyTeamManager::GetInstance(this)->InitTeamWhenInitSelf(FullInfo.BasicPlayerInfo);
	UPWQuestManager::GetInstance(this)->InitData(data.QuestList);
	UPWLobbyDataManager::Get(this)->InitAdditionPurchaseData(data.APList);

	UPWMailManager::GetInstance(this)->ClearData();
}

void UPWProcedureLobby::UpdatePlayerSex(const PlayerData& data)
{
	FullInfo.BasicPlayerInfo.Gender = (EGender)data.Sex;
}

void UPWProcedureLobby::ModifyFlagInt32(uint8 FlagIndex, int32 value)
{
	int32 PreviousValue = PWPlayerInfoManager::ModifyFlagInt32(FlagIndex, value, FullInfo.BasicPlayerInfo);

	// Suggestion: @zhouminyi change represents a transition process include previous and current state. 
	UPWDelegateManager::Get(this)->OnSelfInt32FlagChange.Broadcast(FlagIndex, value);

	UPWDelegateManager::Get(this)->OnSelfInt32FlagChange_WithTransition.Broadcast(FlagIndex, PreviousValue, value);
}

void UPWProcedureLobby::UpdateCompanionData(const FSUAV& CompanionData)
{
	bool IsNewCompanion = true;
	for (FSUAV& Tmp : FullInfo.BasicPlayerInfo.CompanionList)
	{
		if (Tmp.ID == CompanionData.ID)
		{
			Tmp.ExpireType = CompanionData.ExpireType;
			Tmp.ExpireValue = CompanionData.ExpireValue;
			FPWCompanionStruct::SetCompanionSkinByItemID(Tmp, FPWCompanionStruct::GetCompanionSkinItemID(CompanionData));
			IsNewCompanion = false;
			break;;
		}
	}
	if (IsNewCompanion == true)
	{
		FullInfo.BasicPlayerInfo.CompanionList.Add(CompanionData);
	}

	if (UPWDelegateManager::Get(this)->OnCompanionDataChanged.IsBound())
	{
		UPWDelegateManager::Get(this)->OnCompanionDataChanged.Broadcast();
	}
}

int32 UPWProcedureLobby::GetMoney() const
{
	const PlayerData& Data = DETNetClient::Instance().Data;
	return Data.Money + Data.MoneyBound;
}

FPWFullPlayerInfo UPWProcedureLobby::GetFullPlayerInfo()
{
	return FullInfo;
}

void UPWProcedureLobby::GetLoginoutTimeString(bool& bShowTime, FString& LoginTimeStr, FString& LogoutTimeStr)
{
	bShowTime = FullInfo.LastLoginTime && FullInfo.LastLogoutTime;
	if (bShowTime)
	{
		FTimespan timeDiff = FDateTime::Now() - FDateTime::UtcNow();
		LoginTimeStr = (FDateTime::FromUnixTimestamp(FullInfo.LastLoginTime) + timeDiff).ToString(TEXT("%Y-%m-%d %H:%M:%S"));
		LogoutTimeStr = (FDateTime::FromUnixTimestamp(FullInfo.LastLogoutTime) + timeDiff).ToString(TEXT("%Y-%m-%d %H:%M:%S"));
	}
}

const bool UPWProcedureLobby::IsTeamMemberReady(int32 Index /*= 0*/) const
{
	if (TeamManager)
	{
		return TeamManager->IsTeamMemberReady(Index);
	}
	return false;
}

void UPWProcedureLobby::ShowInviteDialog()
{
}

void UPWProcedureLobby::CheckStopServiceMsg()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (!UIManager)
	{
		return;
	}

	int32 LastBulletinID = -1;
	int32 LastStopServiceID = -1;
	UPWSaveGameManager* SaveGameManager = UPWSaveGameManager::Get(this);
	if (SaveGameManager)
	{
		LastBulletinID = SaveGameManager->GetOtherData()->GetLastBulletinID();
		LastStopServiceID = SaveGameManager->GetOtherData()->GetLastStopServiceID();
	}

	TWeakObjectPtr<UPWProcedureLobby>  WeakThis(this);
	DETNetClient& Net = DETNetClient::Instance();
	int32 StopServiceIndexToShow = -1;
	TArray<FClientBulletinInfo>& StopServiceList = Net.StopServiceList.List;
	for (int32 StopServiceIndex = 0; StopServiceIndex < StopServiceList.Num(); StopServiceIndex++)
	{
		if (StopServiceIndexToShow >= 0 && StopServiceList[StopServiceIndex].Id <= StopServiceList[StopServiceIndexToShow].Id)
		{
			continue;
		}
		StopServiceIndexToShow = StopServiceIndex;
	}

	if (StopServiceIndexToShow >= 0)
	{
		FClientBulletinInfo& StopServiceInfo = StopServiceList[StopServiceIndexToShow];
		float RemindTime = StopServiceInfo.EndTime - Net.StopServiceList.ServerTime;
		if (UIManager->IsInViewport(EUMGID::EUMGID_LobbyShutDownBroadcastWidget))
		{
			UPWStopServiceWidget* StopServiceWidget = Cast<UPWStopServiceWidget>(UPWUIManager::Get(WeakThis.Get())->GetWidget(EUMGID::EUMGID_LobbyShutDownBroadcastWidget));
			if (StopServiceWidget)
			{
				StopServiceWidget->SetEndTime(RemindTime);
			}
		}
		else
		{
			FPWUIOpenDelegate OpenDelegate = FPWUIOpenDelegate::CreateLambda([WeakThis, StopServiceInfo, RemindTime]()
				{
					if (!WeakThis.IsValid())
					{
						return;
					}
					UPWStopServiceWidget* StopServiceWidget = Cast<UPWStopServiceWidget>(UPWUIManager::Get(WeakThis.Get())->GetWidget(EUMGID::EUMGID_LobbyShutDownBroadcastWidget));
					if (StopServiceWidget)
					{
						StopServiceWidget->SetEndTime(RemindTime);
					}
				});
			UIManager->ShowUI(EUMGID::EUMGID_LobbyShutDownBroadcastWidget, OpenDelegate);
		}

		if (StopServiceInfo.Id > LastStopServiceID)
		{
			FTimerDelegate PromptDelegate = FTimerDelegate::CreateLambda([WeakThis, StopServiceInfo]() {
				if (!WeakThis.IsValid())
				{
					return;
				}
				UPWPromptBase* Prompt = UPWPromptManager::Get(WeakThis.Get())->ShowPrompt(ePromptType::EPT_WithOK);
				if (Prompt)
				{
					Prompt->SetPromptContent(FText::FromString(StopServiceInfo.Text));
				}
				UPWSaveGameManager* SaveGameManager = UPWSaveGameManager::Get(WeakThis.Get());
				if (SaveGameManager)
				{
					SaveGameManager->GetOtherData()->SetLastStopServiceID(StopServiceInfo.Id);
					SaveGameManager->SaveOthers();
				}
				});
			float RemindTime = StopServiceInfo.EventTime - Net.StopServiceList.ServerTime;
			if (RemindTime > 0)
			{
				GetWorld()->GetTimerManager().SetTimer(StopServicePromptHandle, PromptDelegate, RemindTime, false);
			}
			else
			{
				PromptDelegate.Execute();
			}
		}
		StopServiceList.Empty();
	}
	else if (UIManager->IsInViewport(EUMGID::EUMGID_LobbyShutDownBroadcastWidget))
	{
		UIManager->HideUI(EUMGID::EUMGID_LobbyShutDownBroadcastWidget);
	}
}

bool UPWProcedureLobby::ToggleBeastOrHumanMode()
{
	bIfInBeastMode = !bIfInBeastMode;
	return bIfInBeastMode;
}

void UPWProcedureLobby::SetBeastType(EBeastType InBeastType)
{
	BeastType = InBeastType;
}

void UPWProcedureLobby::SetLoginoutTime(int LoginTime, int LogoutTime)
{
	FullInfo.LastLoginTime = LoginTime;
	FullInfo.LastLogoutTime = LogoutTime;
}

void UPWProcedureLobby::SelectGameMoudles(int32 nMoudles)
{
	m_nMoudles = nMoudles;
	if (m_nMoudles == 0)
	{
		UPWClientNet::GetInstance(this)->SelectModuleReq(3);
	}
	else if (m_nMoudles == 1)
	{
		// 		UPWUIManager::Get(this)->ForceShowUI(EUMGID::EUMGID_Lobby);
		// 		UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Entrance);
		UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Connecting);
	}
	else if (m_nMoudles == 2)
	{
		// 		UPWUIManager::Get(this)->ForceShowUI(EUMGID::EUMGID_ThreeArymLobby);
		// 		UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Entrance);
		UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Connecting);
	}
	else if (m_nMoudles == 3)
	{
		UPWUIManager::Get(this)->ForceShowUI(EUMGID::EUMGID_WW_Lobby);

		UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_Connecting);
		if (UPWLobbyTeamManager::GetInstance(this)->HavePlayTogetherEventToDeal())
		{
			UPWClientNet::GetInstance(this)->CreateTeamReq();
		}
		else if (UPWLobbyTeamManager::GetInstance(this)->IsWaittingForEnterTeam())
		{
			UPWLobbyTeamManager::GetInstance(this)->DealEnterTeam(true);
		}

		if (PWGameCVars::CVarDebugAutoApply.GetValueOnGameThread() > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(AutoApplyHandle, this, &UPWProcedureLobby::AutoApply, 5.0f, false);
		}
	}
}

FPWFullPlayerInfo::FPWFullPlayerInfo()
{
	BasicPlayerInfo = FPWPlayerInfo();

	CoinAmount = 0;
	CompetitionTicketAmount = 0;
	EliteTicketAmount = 0;
}

void UPWProcedureLobby::UpdatePlayerLevel(int32 Level) {
	TArray<FPWCharacterLvExpDataRow> lvExps;
	FPWDataTable::Get().GetDataByDataType(lvExps);
	int32 LargestLevel = FPWArrayUtility::Reduce(
		lvExps,
		[](int32 MaxLevel, const FPWCharacterLvExpDataRow& lvExp) {
			if (lvExp.Lv > MaxLevel)
				MaxLevel = lvExp.Lv;
			return MaxLevel;
		}, 0);
	struct FTemp {
		FTemp(int32 InCurLevel, int32 InLastLevelMax, int32 InThisLevelMax) {
			CurLevel = InCurLevel;
			LastLevelMax = InLastLevelMax;
			ThisLevelMax = InThisLevelMax;
		}
		int32 CurLevel, LastLevelMax, ThisLevelMax;
	};
	FTemp Temp = FPWArrayUtility::Reduce(lvExps, [](FTemp& Value, const FPWCharacterLvExpDataRow& lvExp) {
		if (lvExp.Lv == Value.CurLevel)
			return FTemp(Value.CurLevel, lvExp.NeedExp, lvExp.MaxExp);
		else
			return Value;
		}, FTemp(Level, 0, 0));

	FullInfo.BasicPlayerInfo.RequestExp = Temp.ThisLevelMax;
	FullInfo.BasicPlayerInfo.LastLevelExp = Temp.LastLevelMax;

	FullInfo.BasicPlayerInfo.CurrentLevel = Level;
	if (OnCharacterLvChangedDelegate.IsBound()) {
		OnCharacterLvChangedDelegate.Broadcast(Level);
	}
}

void UPWProcedureLobby::UpdatePlayerExp(int32 Exp) {
	FullInfo.BasicPlayerInfo.CurrentExp = Exp;
	if (OnCharacterExpChangedDelegate.IsBound())
	{
		OnCharacterExpChangedDelegate.Broadcast(Exp);
	}
}

void UPWProcedureLobby::UpdatePlayerScore(int32 Score)
{
	FullInfo.BasicPlayerInfo.Score = Score;
	// 	UPWDebugMessageWidget* DebugMessageWidget = Cast<UPWDebugMessageWidget>(UPWUIManager::Get(this)->GetWidget(EUMGID::EUMGID_DebugMessage));
	// 	if (DebugMessageWidget)
	// 	{
	// 		DebugMessageWidget->OnModifySelfScore();
	// 	}
}

void UPWProcedureLobby::CheckFriendInviteData()
{
	SelectGameMoudles(m_nMoudles);
}

void UPWProcedureLobby::UpdateTranscriptsFlag(int32 value)
{
	FullInfo.TranscriptsFlag = value;
}

void UPWProcedureLobby::PullStopService()
{
	const FString CurLang = DHInternational::GetInstance()->GetDisplayLanguage();
	DETNetClient::Instance().PullStopService(CurLang);
}

void UPWProcedureLobby::PullAdvanceNotice()
{
	const FString CurLang = DHInternational::GetInstance()->GetDisplayLanguage();
	DETNetClient::Instance().PullAdvanceNotice(CurLang);
}

void UPWProcedureLobby::CheckAdvanceNotice()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (!UIManager)
	{
		return;
	}
	PW_LOG(LogTemp, Log, TEXT("CheckAdvanceNotice"));

	DETNetClient& Net = DETNetClient::Instance();
	TArray<FClientBulletinInfo>& AdvanceNoticeList = Net.AdvanceNoticeList.List;
	if (AdvanceNoticeList.Num() == 0)
	{
		PW_LOG(LogTemp, Log, TEXT("AdvanceNoticeList is empty"));
		return;
	}
	TWeakObjectPtr<UPWProcedureLobby> WeakThis(this);
	TArray<FBulletinUrl> ImagesToShow = AdvanceNoticeList[0].Urls;
	FPWUIOpenDelegate UIOpenDelegate = FPWUIOpenDelegate::CreateLambda([WeakThis, ImagesToShow]() {
		if (WeakThis.IsValid())
		{
			UPWLobbyBulletinWidget* LobbyBulletin = Cast<UPWLobbyBulletinWidget>(UPWUIManager::Get(WeakThis.Get())->GetWidget(EUMGID::EUMGID_LobbyBulletinWidget));
			if (LobbyBulletin)
			{
				LobbyBulletin->SetUpBulletinImage(ImagesToShow);
			}
		}
		});
	UIManager->ShowUI(EUMGID::EUMGID_LobbyBulletinWidget, UIOpenDelegate);
	AdvanceNoticeList.Empty();
}

void UPWProcedureLobby::AutoApply()
{
	UPWClientNet::GetInstance(this)->ApplyWolfReq(PWGameCVars::CVarDebugAutoApply.GetValueOnGameThread());
}

void UPWProcedureLobby::ShowWaitingLoadingWidget()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (UIManager)
	{
		if (!UIManager->IsInViewport(EUMGID::EUMGID_WW_WaitingLoading))
		{
			UIManager->ShowUI(EUMGID::EUMGID_WW_WaitingLoading);
		}
	}
}

void UPWProcedureLobby::HideWaitingLoadingWidget()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (UIManager->IsInViewport(EUMGID::EUMGID_WW_WaitingLoading))
	{
		UIManager->HideUI(EUMGID::EUMGID_WW_WaitingLoading);
	}
}
// Copyright 2016 P906, Perfect World, Inc.

#include "Client/PWProcedureBattle.h"
#include "PWGameState.h"
#include "PWUIManager.h"
#include "UIWidgetHead.h"
#include "Client/PWProcedureLobby.h"
#include "PWProcedureManager.h"
#include "PWCharacter.h"
#include "PWLibrary.h"
#include "PWCharacterManager.h"
#include "PWDelegateManager.h"
#include "PWGameState.h"
#include "PWGameInstance.h"
#include "Engine.h"
#include "PlayerController/PWPlayerController.h"
#include "PWWidgetBase.h"
#include "PWGameSettings.h"
#include "UIWidgetHead.h"
#include "PWUIManager.h"
#include "PWAirDropManager.h"
#include "PWCharacterData_Runtime.h"
#include "PWEndingRewardWidget.h"
#include "PWLevelMgr.h"
#include "Classes/Engine/NetDriver.h"
#include "PWCameraActor.h"


#include "PWNetUtility.h"
#include "PWBeginningNotice.h"
#include "PWWeaponInfoWidget.h"
#include "PWUserSettings.h"
#include "PWMainWidget.h"
#include "PWActorTileManager.h"
#include "PWMapConfig.h"
#include "Components/DirectionalLightComponent.h"
#include "Classes/Engine/DirectionalLight.h"
#include "PWHornManager.h"
#include "PWDoubleCircleManager.h"
#include "PWSaveGameManager.h"
#include "PWPreloadCacheManager.h"
#include "PWLogManager.h"
#include "PWRouteManager.h"
#include "PWAirForteManager.h"
#include "PWEnvTempManager/PWEnvTempManager.h"
#include "Environment/SkyLight/PWSkyLightBlender.h"
#include "Environment/SkyLight/PWSkyLightManager.h"
#include "PWMarkerManager/PWMarkerManager.h"
#include "PWInteractRegisterManager.h"
#include "Werewolf/Battle/Common/PWBroadcastMessageWidget.h"
#include "PWPhysicalPowerManager/PWPhysicalPowerManager.h"
#include "PWDynamicActorManager/PWDynamicActorManager.h"
#include "PWMatchResultManager/PWMatchResultManager.h"
#include "Client/PWProcedureExitBattle.h"
#include "Werewolf/Battle/CharacterInfo/PWCharacterMainInfoWidget.h"
#include "Werewolf/Battle/CharacterInfo/PWShowEnemyInfo.h"
#include "PWClientNet.h"
#include "Lobby/PWStopServiceWidget.h"
#include "Lobby/PWStopServiceInGameWidget.h"
#include "PWEnemyHittedInfoManager.h"
#include "DHInternational.h"
#include "WWise/PWBroadCastManager.h"
#include "Werewolf/Battle/Weapon/PWCrosshairWidget.h"
#include "../../../SaveGame/PWSaveGameDefine.h"
#include "PWLibrary.h"
#if PLATFORM_PS4
#include "Online/PS4/DHOnlinePS4.h"
#endif
#include "PWHitTargetPromptManager/PWHitTargetPromptManager.h"
#include "PWInteractArrowManager.h"
#include "PWQuest/PWQuestManager.h"
#include "PWQuest/PWWeekQuestMgr.h"
#include "PWQuest/PWActivityQuestMgr.h"
#include "PWActivityConfig.h"
#include "PWInteractPromptBoardManager.h"
#include "PWSpecificationUIManager.h"

void UPWProcedureBattle::Enter()
{
	// --- wzt 打点 ---
	// --- wzt ---
	PWInteractRegisterManager::GetInstance()->InitialOnClient();

	if (GetWorld()->IsPlayInEditor())
	{
		UPWEnvTempManager::Get(this)->Init();
	}

	_TeamID = 0;
	_bIsRewardInited = false;
	_bIsEndingTipPlayed = false;

	if (GetWorld()->IsPlayInEditor() == false)
	{
		if (DSAddr.IsEmpty())
		{
			PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::Enter BattleServer Addr is wrong!"));
			UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_Lobby);
			return;
		}
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if (controller == nullptr)
		{
			PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::Enter BattleServer PlayerController is wrong!"));
			UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_Lobby);
			return;
		}
		FString travelString;
		if (Token.IsEmpty())
		{
			PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::Enter BattleServer Token is Empty!"));
			return;
		}
		if (UPWBlueprintLibrary::IsPlayWithGamePad() == false)
		{
			controller->ConsoleCommand(TEXT("r.Streaming.FullyLoadUsedTextures 0"), true);
		}

		//		UPWGameInstance::Get(this)->ShowLoadingScreen();
				//new loading
		UPWGameInstance::Get(this)->ShowNewLoadingScreen(EUMGLoadingSceneType::EUMGLoadingSceneType_BeforeBattle);
		TArray<FStringFormatArg> ArgList;
		ArgList.Add(FStringFormatArg(DSAddr));
		ArgList.Add(FStringFormatArg(Token));
		FString url = FString::Format(TEXT("{0}#{1}"), ArgList);
		controller->ClientTravel(url, TRAVEL_Absolute, true);
	}
	UPWDelegateManager::Get(this)->OnConnectToDSSucceeded.AddDynamic(this, &UPWProcedureBattle::OnEnterBattleSucceeded);
	GEngine->OnTravelFailure().AddUObject(this, &UPWProcedureBattle::OnTravelFailed);
	EnterBattleFailed = GEngine->OnNetworkFailure().AddUObject(this, &UPWProcedureBattle::OnNetworkFailed);
	UPWDelegateManager::Get(this)->OnLocalRuntimeDataInited.AddUniqueDynamic(this, &UPWProcedureBattle::OnLocalRuntimeDataInited);
	UPWDelegateManager::Get(this)->OnCharacterStateChanged.AddUniqueDynamic(this, &UPWProcedureBattle::OnCharacterStateChange);
	UPWDelegateManager::Get(this)->OnDayTimeChanged.AddUniqueDynamic(this, &UPWProcedureBattle::OnDayTimeChanged);
	UPWDelegateManager::Get(this)->OnQuestComplete.RemoveAll(this);
	UPWDelegateManager::Get(this)->OnQuestComplete.AddUObject(this, &UPWProcedureBattle::OnQuestComplete);
	UPWSkyLightManager::Get(this)->AllLevelLoaded.AddUniqueDynamic(this, &UPWProcedureBattle::OnAllSkyLightDataLevelLoaded);
	_CalculateMaxVolume();
	UPWLimousineDataManager::Get(this)->Init();
	UPWPhysicalPowerManager::GetInstance(this)->Init();
	UPWBroadCastManager::GetInstance(this)->Init();
	UPWActorTileManager::Get(this)->Init();
	UPWActorTileManager::Get(this)->MakeTiles();
	UPWMatchResultManager::GetInstance(this)->Init();

	UPWProcedureExitBattle* ExitBattle = Cast<UPWProcedureExitBattle>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_ExitBattle));
	if (ExitBattle)
	{
		ExitBattle->SetNextState(ProcedureState::ProcedureState_Lobby);
	}
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (UIManager)
	{
		UIManager->PreLoadUI(EUMGID::EUMGID_WaittingToStartFight);
		UIManager->InitInBattle();
	}
#if PLATFORM_PS4
	UDHOnlinePS4::GetInstance(this)->SetPresence(EGameStatus::EGameStatus_Fighting);
#endif
}

void UPWProcedureBattle::Leave()
{
	// --- wzt 打点 ---
	// --- wzt ---
	UPWBroadCastManager::GetInstance(this)->Release();
	GEngine->OnNetworkFailure().RemoveAll(this);
	GEngine->OnTravelFailure().RemoveAll(this);
	UPWDelegateManager::Get(this)->OnConnectToDSSucceeded.RemoveAll(this);
	SkyLightBlender = nullptr;
	UPWSkyLightManager::Get(this)->AllLevelLoaded.RemoveDynamic(this, &UPWProcedureBattle::OnAllSkyLightDataLevelLoaded);
	UPWDelegateManager::Get(this)->OnDayTimeChanged.RemoveDynamic(this, &UPWProcedureBattle::OnDayTimeChanged);
	UPWDelegateManager::Get(this)->OnLocalRuntimeDataInited.RemoveDynamic(this, &UPWProcedureBattle::OnLocalRuntimeDataInited);
	UPWDelegateManager::Get(this)->OnCharacterStateChanged.RemoveDynamic(this, &UPWProcedureBattle::OnCharacterStateChange);
	UPWDelegateManager::Get(this)->OnQuestComplete.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	UPWHitTargetPromptManager::GetInstance(this)->Release();
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_DamageInstigator);
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_InGameShutDownBroadcastWidget);
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_InGameTaskWidget);
	TaskInfos.Empty();
	DSAddr.Empty();
	Token.Empty();
	UPWEnemyHittedInfoManager::GetInstance(this)->Clear();
	UPWPhysicalPowerManager::GetInstance(this)->Release();
	UPWUIManager::Get(this)->ClearInBattle();
	_LeaveVivoxChannels();
	UPWInteractArrowManager::GetInstance(this)->OnExitFight();
	UPWInteractPromptBoardManager::GetInstance(this)->OnExitFight();
	UPWPreloadCacheManager::Get(this)->Release();
	UPWActorTileManager::Get(this)->Clear();
}

void UPWProcedureBattle::SetTeamID(uint64 inTeamID)
{
	_TeamID = inTeamID;
	UPWCharacterMainInfoWidget* CharacterMainInfoWidget = Cast<UPWCharacterMainInfoWidget>(UPWUIManager::Get(this)->GetWidget(EUMGID::EUMGID_CharacterInfo));
	if (CharacterMainInfoWidget && CharacterMainInfoWidget->ShowEnemyInfoWidget)
	{
		CharacterMainInfoWidget->ShowEnemyInfoWidget->SetTeamID(_TeamID);
	}
}

//----------------------------BattleUI--------------------------------------
void UPWProcedureBattle::UpdateBattleUI()
{
	EMapType MapType = UPWBlueprintLibrary::GetMapType(this);
	APWCharacterData_Runtime* RT = UPWBlueprintLibrary::GetLocalRuntimeData(this);
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (RT == nullptr || UIManager == nullptr)
		return;

	if (MapType == EMapType::EMapType_WereWolf)
	{
		switch (RT->GetCharacterState())
		{
		case ECharacterState::ECS_WaittingForStart:
			_ShowWaittingForStartUI_Werewolf(UIManager, RT);
			break;
		case ECharacterState::ECS_OnThePlane:
			_ShowPrepareForFightUI_Werewolf(UIManager, RT);
			break;
		case ECharacterState::ECS_Fighting:
			_ShowFightingUI_Werewolf(UIManager, RT);
			break;
		case ECharacterState::ECS_GameOver:
		case ECharacterState::ECS_WaittingForReward:
			_ShowGameOverUI_Werewolf(UIManager, RT);
			break;
		}
	}
	else
	{
		switch (RT->GetCharacterState())
		{
		case ECharacterState::ECS_WaittingForStart:
			_ShowWaittingForStartUI(UIManager);
			break;
		case ECharacterState::ECS_OnThePlane:
			_ShowParachuteUI(UIManager);
			break;
		case ECharacterState::ECS_Fighting:
		{
			APWCharacter* PWCH = Cast<APWCharacter>(RT->GetCharacter());
			if (PWCH && PWCH->IsParachuting())
			{
				_ShowParachuteUI(UIManager);
			}
			else
			{
				_ShowFightingUI(UIManager);
			}
		}
		break;
		case ECharacterState::ECS_GameOver:
		case ECharacterState::ECS_WaittingForReward:
			_ShowGameOverUI(UIManager);
			break;
		}
	}
	UIManager->ShowUI(EUMGID::EUMGID_OperationNotice);
	CheckStopServiceMsg();
}

void UPWProcedureBattle::_ShowWaittingForStartUI(class UPWUIManager* UIManager)
{
	HideTotalLoadings();
	// 	UIManager->ShowUI(EUMGID::EUMGID_BeginningNotice);
	UIManager->ShowUI(EUMGID::EUMGID_WW_BroadcastMessage);
	UIManager->ShowUI(EUMGID::EUMGID_TeamMessage);
	UIManager->ShowUI(EUMGID::EUMGID_MainWidget);
	// 	if (UIManager->GetWidget(EUMGID::EUMGID_Inventory) == nullptr)
	// 	{
	UIManager->ShowUI(EUMGID::EUMGID_CharacterInfo);
	UIManager->ShowUI(EUMGID::EUMGID_TeamInfo);
	// 		APWCharacter* chr = Cast<APWCharacter>(UPWBlueprintLibrary::GetLocalPWCharacter(this));
	// 		if (chr && chr->IsDriving())
	// 		{
	// 			UIManager->ShowUI(EUMGID::EUMGID_Vehicle);
	// 		}
	// 	}
}

void UPWProcedureBattle::_ShowParachuteUI(class UPWUIManager* UIManager)
{
	HideTotalLoadings();
	// 	UIManager->HideUI(EUMGID::EUMGID_BeginningNotice);
	UIManager->ShowUI(EUMGID::EUMGID_MainWidget);
	// 	if (UIManager->GetWidget(EUMGID::EUMGID_Inventory) == nullptr)
	// 	{
	UIManager->ShowUI(EUMGID::EUMGID_CharacterInfo);
	UIManager->ShowUI(EUMGID::EUMGID_TeamInfo);
	if (UIManager->GetWidget(EUMGID::EUMGID_Map) == nullptr)
	{
		UIManager->ShowUI(EUMGID::EUMGID_MiniMap);
	}
	// 	}
}

void UPWProcedureBattle::_ShowFightingUI(class UPWUIManager* UIManager)
{
	HideTotalLoadings();
	// 	UIManager->HideUI(EUMGID::EUMGID_BeginningNotice);
	UIManager->ShowUI(EUMGID::EUMGID_MainWidget);
	// 	if (UIManager->GetWidget(EUMGID::EUMGID_Inventory) == nullptr)
	// 	{
	UIManager->ShowUI(EUMGID::EUMGID_CharacterInfo);
	UIManager->ShowUI(EUMGID::EUMGID_TeamInfo);

	if (UIManager->GetWidget(EUMGID::EUMGID_Map) == nullptr)
	{
		UIManager->ShowUI(EUMGID::EUMGID_MiniMap);
		// 			APWGameState* PWGS = Cast<APWGameState>(GetWorld()->GetGameState());
		// 			if (PWGS)
		// 			{
		// 				APWSafeCircle* SafeCircleActor = PWGS->GetSafeCircleActor();
		// 				if (SafeCircleActor && SafeCircleActor->GetCircleState() == eSafeCircleState::eSafeCircleState_WaittingForShrinking)
		// 				{
		// 					UIManager->ShowUI(EUMGID::EUMGID_SafeAreaInfo);
		// 				}
		// 				else
		// 				{
		// 					UIManager->HideUI(EUMGID::EUMGID_SafeAreaInfo);
		// 				}
		// 			}
	}
	// 		APWCharacter* chr = Cast<APWCharacter>(UPWBlueprintLibrary::GetLocalPWCharacter(this));
	// 		if (chr)
	// 		{
	// 			if (chr->IsDriving())
	// 			{
	// 				UIManager->ShowUI(EUMGID::EUMGID_Vehicle);
	// 			}
				/*if (chr->CurrentOxygen < chr->CharacterData->OxygenMaxmun)
				{
					UIManager->ShowUI(EUMGID::EUMGID_Lung);
				}*/
				//UIManager->ShowUI(EUMGID::EUMGID_SafeAreaInfo);
	// 		}

	TWeakObjectPtr<UPWUIManager> WeakThis(UPWUIManager::Get(this));
	FPWUIOpenDelegate UIOpenDelegate = FTimerDelegate::CreateLambda([WeakThis]() {
		if (WeakThis.IsValid())
		{
			UPWCrosshairWidget* CrosshairWidget = Cast<UPWCrosshairWidget>(UPWUIManager::Get(WeakThis.Get())->GetWidget(EUMGID::EUMGID_WeaponInfo));
			if (CrosshairWidget)
			{
				CrosshairWidget->UpdateAllWeapons();
				/*UPWSaveGameContent* SGContent = UPWSaveGameManager::Get(WeakThis.Get())->GetPWSGContent();
				if (SGContent)
				{
					bool IsWeaponSlotHUD = SGContent->GetBoolValueByType(EPWSGType::EPWSGType_WeaponSlotHUD);
					WeaponInfoWidget->SetWeaponPanelVisibilty(IsWeaponSlotHUD);
				}*/
			}
		}
		});
	UIManager->ShowUI(EUMGID::EUMGID_WeaponInfo, UIOpenDelegate);
	// 	}
}

void UPWProcedureBattle::_ShowGameOverUI(class UPWUIManager* UIManager)
{
	HideTotalLoadings();
	UIManager->HideUI(EUMGID::EUMGID_Map);
	UIManager->HideUI(EUMGID::EUMGID_MiniMap);
	UIManager->HideUI(EUMGID::EUMGID_TeamInfo);
	UIManager->HideUI(EUMGID::EUMGID_Settings);
	UIManager->HideUI(EUMGID::EUMGID_ESCMatch);
	UIManager->HideUI(EUMGID::EUMGID_WeaponInfo);
	UIManager->ShowUI(EUMGID::EUMGID_MainWidget);
	UIManager->HideUI(EUMGID::EUMGID_AutoPickup);
	UIManager->HideUI(EUMGID::EUMGID_Interaction);
	// 	UIManager->HideUI(EUMGID::EUMGID_SafeAreaInfo);
	UIManager->HideUI(EUMGID::EUMGID_CharacterInfo);
	UIManager->HideUI(EUMGID::EUMGID_OperationNotice);
	UIManager->HideUI(EUMGID::EUMGID_WW_BornOperation);
	// 	UIManager->HideUI(EUMGID::EUMGID_ExplodeCountdown);
	UIManager->HideUI(EUMGID::EUMGID_WW_BattleHumanItem);
	UIManager->HideUI(EUMGID::EUMGID_WW_GamePadOperation);
	UIManager->HideUI(EUMGID::EUMGID_WW_BroadcastMessage);

	//UIManager->HideUI(EUMGID::EUMGID_DrugPreview);


}

void UPWProcedureBattle::OnPreLoadMap(const FString& MapName)
{

}

void UPWProcedureBattle::OnPostLoadMap(UWorld* LoadedWorld)
{
	PW_LOG(LogTemp, Warning, TEXT("UPWProcedureBattle::OnPostLoadMap"));
	// Read static marker info
	UPWMarkerManager::Get(this)->InitForCurMap();
	PW_LOG(LogTemp, Error, TEXT("PWMarkerManager CarrionHeapLocations Num : %d"), UPWMarkerManager::Get(this)->CarrionHeapLocations.Num());

	UPWUIManager::Get(this)->PreLoadUI(EUMGID::EUMGID_ESCMatch);

	//UPWLevelMgr::Get(this)->HandlePostLoadMap(LoadedWorld);

	_PreLoadUI();


	//UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_WaittingToStartFight);
	UPWEnemyHittedInfoManager::GetInstance(this)->Init();
	UPWHitTargetPromptManager::GetInstance(this)->Init();
}

void UPWProcedureBattle::OnPostLoadAllMaps()
{
	// TODO 在编辑器模式下，开启多人模式，这里会触发N-1次，需要修复

	EMapType MapType = UPWBlueprintLibrary::GetMapType(this);
	if (MapType == EMapType::EMapType_Corps)
	{
		UPWDoubleCircleManager::Get(this)->Init();
	}
	else if (MapType == EMapType::EMapType_Normal || MapType == EMapType::EMapType_MiniBattle || MapType == EMapType::EMapType_WereWolf)
	{
		UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_OperationNotice);
		// 		UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_SafeAreaNotice);

		UPWSafeCircleManager::Get(this)->Init();
	}
	UPWHornManager::Get(this)->Init();
	UPWPreloadCacheManager::Get(this)->Init();
	UPWRouteManager::Get(this)->Init();
	UPWAirForteManager::Get(this)->Init();
	UPWEnvTempManager::Get(this)->Init();
	UPWWWiseManager::Get(this)->Init();

	APWPlayerControllerBase* PWPCBase = Cast<APWPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
	if (PWPCBase)
	{
		PWPCBase->ServerPlayerLoadAllMap();
	}
	//Apply Specific console variables
	_ApplyConsoleConfig();

	bAllSkyDataLevelLoaded = false;
	FindSkyLightBlender();
#if WITH_EDITOR
	// for every game instance , we need a unique copy of sky light data level loaded from disk. This is conflicted
	// with PIE level load mechanism. So we only load sky light data levels for first game instance.
	if (GetWorld()->IsPlayInEditor())
	{
		if (GetWorld()->GetOutermost()->PIEInstanceID >= 3)
		{
			return;
		}
	}
#endif
	UPWSkyLightManager::Get(this)->LoadDataLevels();

	GetWorld()->GetTimerManager().SetTimer(PullStopServiceTimerHandle, this, &ThisClass::PullStopService, 60, true);
	PullStopService(); //确定能显示UI的时候再拉停服广播

	UPWInteractArrowManager::GetInstance(this)->OnEnterFight();
	UPWInteractPromptBoardManager::GetInstance(this)->OnEnterFight();
}

void UPWProcedureBattle::FindSkyLightBlender()
{
	for (TActorIterator<APWSkyLightBlender> It(GetWorld()); It; ++It)
	{
		SkyLightBlender = *It;
		if (SkyLightBlender->bCreateFromBlueprint)
		{
			PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::FindSkyLightBlender bCreateFromBlueprint of SkyLightBlender must be false"));
		}
		return;
	}
	PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::FindSkyLightBlender cant find SkyLightBlender"));
}

void UPWProcedureBattle::OnAllSkyLightDataLevelLoaded()
{
	PW_LOG(LogTemp, Warning, TEXT("OnAllSkyLightDataLevelLoaded"));
	bAllSkyDataLevelLoaded = true;
}

void UPWProcedureBattle::OnEnterBattleSucceeded()
{
	UPWDelegateManager::Get(this)->OnConnectToDSSucceeded.RemoveAll(this);
	PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::OnEnterBattleSucceeded"));

	_SetVivoxAreaChannelName();
}

void UPWProcedureBattle::OnNetworkFailed(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::OnEnterbattleFailed, ErrorString = %s"), *ErrorString);
	UPWClientNet::GetInstance(this)->LeaveBattleUnExcepted(BattleId);
	UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_ExitBattle);
}

void UPWProcedureBattle::OnTravelFailed(UWorld* World, ETravelFailure::Type NetDriver, const FString& ErrorString)
{
	PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::OnTravelFailed, ErrorString = %s"), *ErrorString);
	UPWClientNet::GetInstance(this)->LeaveBattleUnExcepted(BattleId);
	UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_ExitBattle);
}

void UPWProcedureBattle::_PreLoadUI()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	UIManager->PreLoadUI(EUMGID::EUMGID_WaittingToStartFight);
	UIManager->PreLoadUI(EUMGID::EUMGID_Map);
	// 	UIManager->PreLoadUI(EUMGID::EUMGID_Inventory);
		//UIManager->PreLoadUI(EUMGID::EUMGID_ParachuteInfo);
	UIManager->PreLoadUI(EUMGID::EUMGID_MainWidget);
	UIManager->PreLoadUI(EUMGID::EUMGID_WW_BornOperation);
	UIManager->PreLoadUI(EUMGID::EUMGID_AutoPickup);
	if (UPWBlueprintLibrary::IsPlayWithGamePad())
	{
		UIManager->PreLoadUI(EUMGID::EUMGID_Interaction);
	}
}

void UPWProcedureBattle::OnCharacterStateChange(uint8 state)
{
	UpdateBattleUI();
}

void UPWProcedureBattle::OnDayTimeChanged(EPWDayTimeType From, EPWDayTimeType To)
{
	if (!SkyLightBlender)
	{
		// Skylight blender is streaming loaded, so may be nullptr at PostLoadAllMaps, search it again.
		FindSkyLightBlender();
	}
	if (!SkyLightBlender)
	{
		PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::OnDayTimeChanged SkyLightBlender missing!"));
		return;
	}
	if (!bAllSkyDataLevelLoaded)
	{
		PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::OnDayTimeChanged not all data level loaded!"));
		return;
	}
	float BlenderSpeed = UPWSkyLightManager::Get(this)->GetBlenderSpeed(To);
	SkyLightBlender->StartToChange((uint8)From, (uint8)To, BlenderSpeed, false);
}

void UPWProcedureBattle::OnQuestComplete(const FPWTUnionStruct& QuestStruct)
{
	int64 QuestID = QuestStruct.GetParam<int64>(0);
	UPWWeekQuestMgr* WeekQuestMgr = UPWQuestManager::GetQuestMgr<UPWWeekQuestMgr>(this);
	UPWActivityQuestMgr* ActivityQuestMgr = UPWQuestManager::GetQuestMgr<UPWActivityQuestMgr>(this);
	if (WeekQuestMgr && WeekQuestMgr->IsDailyQuest(QuestID))
	{
		FText TaskDesc = FText::FromString(QuestStruct.GetParam<FString>(1));
		int32 TargetNum = QuestStruct.GetParam<int32>(2);
		FText TaskTarget = FText::FromString(FString::Printf(TEXT("%d/%d"), TargetNum, TargetNum));
		AddHUDTaskInfo(TaskDesc, TaskTarget, FText::FromStringTable("Lang", "MISSION_DAILY"));
		return;
	}
	if (ActivityQuestMgr && ActivityQuestMgr->ShouldShowHUDTips(QuestID))
	{
		int32 ActivityID = ActivityQuestMgr->GetActivityIDByQuestID(QuestID);
		const FPWActivityConfig* Cfg = FPWActivityConfigDataTable::GetInstance()->GetRowByKey(ActivityID);
		return_if_null(Cfg);
		FText TaskDesc = FText::FromString(QuestStruct.GetParam<FString>(1));
		int32 TargetNum = QuestStruct.GetParam<int32>(2);
		FText TaskTarget = FText::FromString(FString::Printf(TEXT("%d/%d"), TargetNum, TargetNum));
		AddHUDTaskInfo(TaskDesc, TaskTarget, Cfg->Name);
		return;
	}

}

void UPWProcedureBattle::OnLocalRuntimeDataInited()
{
	UpdateBattleUI();
}

void UPWProcedureBattle::PullStopService()
{
	const FString CurLang = DHInternational::GetInstance()->GetDisplayLanguage();
	DETNetClient::Instance().PullStopService(CurLang);
}

void UPWProcedureBattle::CheckStopServiceMsg()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (!UIManager)
	{
		return;
	}
	DETNetClient& Net = DETNetClient::Instance();
	TWeakObjectPtr<UPWProcedureBattle>  WeakThis(this);
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
		if (UIManager->IsInViewport(EUMGID::EUMGID_InGameShutDownBroadcastWidget))
		{
			UPWStopServiceWidget* StopServiceWidget = Cast<UPWStopServiceWidget>(UPWUIManager::Get(WeakThis.Get())->GetWidget(EUMGID::EUMGID_InGameShutDownBroadcastWidget));
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
					UPWStopServiceWidget* StopServiceWidget = Cast<UPWStopServiceWidget>(UPWUIManager::Get(WeakThis.Get())->GetWidget(EUMGID::EUMGID_InGameShutDownBroadcastWidget));
					if (StopServiceWidget)
					{
						StopServiceWidget->SetEndTime(RemindTime);
					}
				});
			UIManager->ShowUI(EUMGID::EUMGID_InGameShutDownBroadcastWidget, OpenDelegate);
		}
	}
	else if (UIManager->IsInViewport(EUMGID::EUMGID_InGameShutDownBroadcastWidget))
	{
		UIManager->HideUI(EUMGID::EUMGID_InGameShutDownBroadcastWidget);
	}
}

void UPWProcedureBattle::OnPlayerRewardNotify(FCMD_S2C_BattleAwardNotify inReward)
{
	_bIsRewardInited = true;
	SelfRewardV2 = inReward;
	_ShowEndUI();
}

void UPWProcedureBattle::BackToLobby()
{
	if (_bIsEndingTipPlayed == false)
	{
		APWPlayerControllerBase* PWPC = Cast<APWPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
		if (PWPC)
		{
			PWPC->Server_EndMatch();
		}
	}
	else
	{
		_ShowEndUI();
	}
}

void UPWProcedureBattle::OnDeathTipClosed(bool bMatchOver)
{
	APWPlayerController* PWPC = Cast<APWPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PWPC)
	{
		PWPC->ServerChangeToViewState();
		// 		UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_Observe);
	}

#if !PLATFORM_LINUX
	UPWVivoxManager::Get(this)->SetAreaModuleLogicState(false); // 进入观战模式，停止区域语音
#endif
}

void UPWProcedureBattle::OnItemRewardWindowClosed()
{
	UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_Ending);
}

void UPWProcedureBattle::_ShowEndUI()
{
	/*if (_bIsEndingTipPlayed == false)
		return;*/

	if (_bIsRewardInited == false)
		return;

	// 	bool bSkipItemRewardUI = true;
		//if (SelfReward.ItemID > 0)
		//{
		//	bSkipItemRewardUI = false;
		//}
		//else
		//{
		//	for (auto tmp : TeammateRewardList)
		//	{
		//		if (tmp.ItemID > 0)
		//		{
		//			bSkipItemRewardUI = false;
		//			break;
		//		}
		//	}
		//}

#if PLATFORM_PS4
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_AutoPickup);
#endif

	// 	if (bSkipItemRewardUI == false)
	// 	{
	// 		UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_EndingReward);
	// 	}
	// 	else
	// 	{
	UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_Ending);
	// 	}
}

void UPWProcedureBattle::HideTotalLoadings()
{
	UPWGameInstance::Get(this)->HideLoadingScreen();
	UPWGameInstance::Get(this)->HideNewLoadingScreen();
}

void UPWProcedureBattle::_ShowWaittingForStartUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData)
{
	HideTotalLoadings();
	UIManager->ForceShowUI(EUMGID::EUMGID_WaittingToStartFight);

	UIManager->ShowUI(EUMGID::EUMGID_MainWidget);
	UIManager->ShowUI(EUMGID::EUMGID_CharacterInfo);
	UIManager->ShowUI(EUMGID::EUMGID_WW_BroadcastMessage);
	UIManager->ShowUI(EUMGID::EUMGID_TeamMessage);
	UIManager->ShowUI(EUMGID::EUMGID_MiniMap);
	UIManager->ShowUI(EUMGID::EUMGID_TeamInfo);
	UIManager->ShowUI(EUMGID::EUMGID_DamageInstigator);
	UIManager->ShowUI(EUMGID::EUMGID_WW_SurroundingGrenadeInfo);
	RefreshPingWidgetBySettingParam();
}

void UPWProcedureBattle::_ShowPrepareForFightUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData)
{
	HideTotalLoadings();
	UIManager->HideUI(EUMGID::EUMGID_WaittingToStartFight);
}

void UPWProcedureBattle::_ShowParachuteUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData)
{
	_ShowParachuteUI(UIManager);
}

void UPWProcedureBattle::_ShowFightingUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData)
{
	HideTotalLoadings();
	UIManager->HideUI(EUMGID::EUMGID_WaittingToStartFight);
	UIManager->ShowUI(EUMGID::EUMGID_DamageInstigator);
	UIManager->ShowUI(EUMGID::EUMGID_WW_BroadcastMessage);
	UIManager->ShowUI(EUMGID::EUMGID_TeamMessage);
	UIManager->ShowUI(EUMGID::EUMGID_WW_SurroundingGrenadeInfo);
	RefreshPingWidgetBySettingParam();
	if (UPWBlueprintLibrary::IsPlayWithGamePad())
	{
		UPWSaveGameManager* SaveGameManager = UPWBlueprintLibrary::GetSaveGameManager(this);
		if (SaveGameManager && SaveGameManager->GetPWSGTutorial())
		{
			UPWSaveGameTutorial* SGTutorial = SaveGameManager->GetPWSGTutorial();
			bool bIfShow = SGTutorial->GetBoolValueByType(EPWSGType::EPWSGType_TutorialVis);

			APWCharacterBase* CurCharacter = UPWBlueprintLibrary::GetLocalPWCharacter(this);
			if (bIfShow && CurCharacter && (CurCharacter->GetCharacterType() == ECharacterType::CT_Werewolf || CurCharacter->GetCharacterType() == ECharacterType::CT_Leopard) && !IsInitial)
			{
				IsInitial = true;
				UPWSpecificationUIManager::GetInstance(this)->OnShowSpecificationInBattleWidget(EPWSpecificationInBattleType::HintTeachingInfo);
			}
		}
	}

	if (RuntimeData->GetCharacterType() == ECharacterType::CT_Human)
	{
		APWCharacter* PWCH = Cast<APWCharacter>(RuntimeData->GetCharacter());
		if (PWCH && PWCH->IsParachuting())
		{
			_ShowParachuteUI_Werewolf(UIManager, RuntimeData);
		}
		else
		{
			_ShowFightingUI(UIManager);
		}
	}
	else if (RuntimeData->GetCharacterType() == ECharacterType::CT_Werewolf || RuntimeData->GetCharacterType() == ECharacterType::CT_Leopard)
	{
		_ShowFightingUI(UIManager);
	}
}

void UPWProcedureBattle::_ShowGameOverUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData)
{
	HideTotalLoadings();
	_ShowGameOverUI(UIManager);
}

void UPWProcedureBattle::_ShowEndUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData)
{
	_ShowEndUI();
}

void UPWProcedureBattle::OnEndTipClosed()
{
	_bIsEndingTipPlayed = true;
	_ShowEndUI();
}

void UPWProcedureBattle::ConnectionLost()
{
	UPWClientNet::GetInstance(this)->LeaveBattleUnExcepted(BattleId);
	UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_ExitBattle);
}

FBox UPWProcedureBattle::GetMaxVolumeOfAll() const
{
	return _MaxVolumeOfLevels;
}

void UPWProcedureBattle::_CalculateMaxVolume()
{
	int32 BornislandCapacity = 1;
	FPWMapConfig MapConfig;
	if (UPWBlueprintLibrary::GetMapConfig(this, MapConfig))
	{
		FVector WorldCenter = MapConfig.WorldCenter;
		FVector WorldSize = FVector(MapConfig.WorldSize, WorldCenter.Z);
		_MaxVolumeOfLevels = FBox(-WorldSize / 2, WorldSize / 2);
	}
	return;
}

void UPWProcedureBattle::_SetVivoxAreaChannelName()
{
#if !PLATFORM_LINUX
	FString AreaChannelName = FString::Printf(TEXT("%s%llu"), *DSAddr.Replace(TEXT("."), TEXT("")).Replace(TEXT(":"), TEXT("")), BattleId);
	UPWVivoxManager::Get(this)->SetAreaChannelName(AreaChannelName);
#endif
}

void UPWProcedureBattle::_LeaveVivoxChannels()
{
#if !PLATFORM_LINUX
	UPWGameInstance* GameInstance = UPWGameInstance::Get(GetWorld());
	if (GameInstance)
	{
		GameInstance->GetVivoxManager()->SetTeamModuleLogicState(false);
		GameInstance->GetVivoxManager()->SetAreaModuleLogicState(false);
	}
#endif
}

void UPWProcedureBattle::_ApplyConsoleConfig()
{
	int32 BornislandCapacity = 1;
	FPWMapConfig MapConfig;
	if (UPWBlueprintLibrary::GetMapConfig(this, MapConfig))
	{
		if (MapConfig.ConsoleConfigOverwrite.bNeededOverwrite_DynamicShadowDistanceMovableLight)
		{
			for (TActorIterator<ADirectionalLight> ActorIter(GetWorld()); ActorIter; ++ActorIter)
			{
				ADirectionalLight* DLight = *ActorIter;
				UDirectionalLightComponent* DLComp = Cast<UDirectionalLightComponent>(DLight->GetComponentByClass(UDirectionalLightComponent::StaticClass()));
				if (DLComp)
				{
					DLComp->SetDynamicShadowDistanceMovableLight(MapConfig.ConsoleConfigOverwrite.DynamicShadowDistanceMovableLight);
				}

			}
		}

	}
	else
	{
		PW_LOG(LogTemp, Error, TEXT("UPWProcedureBattle::ApplyConsoleConfig  MapId failed!"));
	}
}

void UPWProcedureBattle::RefreshPingWidgetBySettingParam()
{
	ShowPingWidgetLogic(UPWBlueprintLibrary::IsShowWidgetByType(this, EPWSaveTabPageType::PSTPT_Display, EPWSGType::EPWSGType_PING));
}

void UPWProcedureBattle::ShowPingWidgetLogic(bool IsShow)
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (UIManager->IsValidLowLevel())
	{
		if (IsShow)
		{
			UIManager->ShowUI(EUMGID::EUMGID_WW_PINGWidget);
		}
		else
		{
			UIManager->HideUI(EUMGID::EUMGID_WW_PINGWidget);
		}
	}

}

void UPWProcedureBattle::AddHUDTaskInfo(FText TaskContent, FText TaskTarget, FText QuestType)
{
	if (UPWProcedureManager::GetInstance(this)->GetCurState() != ProcedureState::ProcedureState_Battle)
	{
		return;
	}
	FHUDTaskInfo NewInfo;
	NewInfo.Content = TaskContent;
	NewInfo.Target = TaskTarget;
	NewInfo.QuestType = QuestType;
	TaskInfos.Add(NewInfo);
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (!IsValid(UIManager))
	{
		return;
	}
	if (UIManager->IsInViewport(EUMGID::EUMGID_InGameTaskWidget))
	{
		return;
	}
	UIManager->ShowUI(EUMGID::EUMGID_InGameTaskWidget);
}

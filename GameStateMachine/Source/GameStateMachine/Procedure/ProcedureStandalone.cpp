// Copyright 2004-2018 Perfect World Co.,Ltd. All Rights Reserved.

#include "Client/PWProcedureStandaloneCopy.h"
#include "Engine/Engine.h"
#include "PWProcedureManager.h"
#include "Client/PWProcedureLobby.h"
#include "Kismet/GameplayStatics.h"
#include "PWUIManager.h"
#include "PWGameInstance.h"
#include "PWLibrary.h"
#include "PWSaveGameManager.h"
#include "PWCharacter.h"
#include "Standalone/Level/PWStandaloneLeveScriptActor.h"
#include "PWWeaponInfoWidget.h"
#include "Werewolf/Battle/Weapon/PWCrosshairWidget.h"
#include "PWNetDefine.h"


static FName L_MapName = TEXT("/Game/StandalonePackage/Maps/SpecialLevel/SpecialLevel_Main");
static int32 L_StandaloneMapID = 8100101;

ProcedureState UPWProcedureStandaloneCopy::GetState()
{
	return ProcedureState::ProcedureState_StandaloneCopy;
}

void UPWProcedureStandaloneCopy::Enter()
{
	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(this);
	FString MapName = L_MapName.ToString();
	if (GEngine && GEngine->MakeSureMapNameIsValid(MapName) == false)
	{
		//this means we havent get the standalone patch
		ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_Lobby);
		return;
	}
	UPWProcedureLobby* LobbyProcedure = Cast<UPWProcedureLobby>(ProcedureManager->GetProcedure(ProcedureState::ProcedureState_Lobby));
	if ((LobbyProcedure->GetFullPlayerInfo().TranscriptsFlag & (1 << TRANSCRIPT_1)) > 0)
	{
		//this means we have completed the standalone mission
		ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_Lobby);
		return;
	}

	UPWGameInstance::Get(this)->SetMapID(L_StandaloneMapID);

	if (GetWorld()->IsPlayInEditor() == false)
	{
		//begin standalone fight
		m_bIsStandaloneCopyStarted = true;
		UGameplayStatics::OpenLevel(this, L_MapName);
		//UPWUIManager::Get(this)->ForceShowUI(EUMGID::EUMGID_Connecting);
	}
}

void UPWProcedureStandaloneCopy::Leave()
{
	if (m_bIsStandaloneCopyStarted == true)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		check(PC);
		FString cmd = TEXT("open ");
		cmd += UPWGameInstance::Get(this)->GetLobbyMapName();
		FString OutMsg = PC->ConsoleCommand(cmd);
		m_bIsStandaloneCopyStarted = false;
	}
}

void UPWProcedureStandaloneCopy::OnPostLoadMap(UWorld* LoadedWorld)
{
	//UPWUIManager::Get(this)->ForceShowUI(EUMGID::EUMGID_Connecting);
}

void UPWProcedureStandaloneCopy::OnPostLoadAllMaps()
{
	APWStandaloneLeveScriptActor* LevelScriptActor = Cast<APWStandaloneLeveScriptActor>(GetWorld()->GetLevelScriptActor());
	if (LevelScriptActor)
	{
		LevelScriptActor->OnPostLoadAllMap();
	}
	// 	UPWUIManager::Get(this)->PreLoadUI(EUMGID::EUMGID_Inventory);
}

void UPWProcedureStandaloneCopy::UpdateUI()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	UIManager->ForceShowUI(EUMGID::EUMGID_MainWidget);
	UIManager->ShowUI(EUMGID::EUMGID_StandaloneNotice);
	// 	if (UIManager->GetWidget(EUMGID::EUMGID_Inventory) == nullptr)
	// 	{
	if (UIManager->GetWidget(EUMGID::EUMGID_Map) == nullptr)
	{
		UIManager->ShowUI(EUMGID::EUMGID_MiniMap);
	}
	// 		APWCharacter* chr = Cast<APWCharacter>(UPWBlueprintLibrary::GetLocalPWCharacter(this));
	// 		if (chr && chr->IsDriving())
	// 		{
	// 			UIManager->ShowUI(EUMGID::EUMGID_Vehicle);
	// 		}
	UIManager->ShowUI(EUMGID::EUMGID_CharacterInfo);
	UIManager->ShowUI(EUMGID::EUMGID_TeamInfo);

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

// Copyright 2016 P906, Perfect World, Inc.

#include "PWProcedureUpdate.h"
#include "PWGameInstance.h"
#include "PWProcedureManager.h"
#include "PWLibrary.h"
#include "PWLoadingScreen.h"
#include "Online/DHEndpointTracker.h"
#include "GameFramework/PlayerController.h"
#include "PWSaveGameManager.h"
#include "PWClientNet.h"
#include "GameMapsSettings.h"
#include "PWLevelMgr.h"
#include "DHPlatform.h"
#include "PWAssetManager.h"


//static void OnLoadBankCompletedPS4(AkUInt32 in_bankID, const void *in_pInMemoryBankPtr, AKRESULT in_eLoadResult, AkMemPoolId in_memPoolId, void *in_pCookie);

void UPWProcedureUpdate::Enter()
{
	if (GetWorld() && GetWorld()->IsPlayInEditor())
	{
		if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
		{
			UPWSaveGameManager::Get(this)->Init();
			UPWSaveGameManager::Get(this)->InitUserData();
		}
		UPWAssetManager::Get(this)->Init();
		UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_Battle);
		return;
	}

	Initilize();
	if (FPWLoadingScreenModule::Get().IsStartMoviePlaying()) {
		FPWLoadingScreenModule::Get().OnStartMovieFinishedDelegate.AddUObject(this, &UPWProcedureUpdate::OnStartMovieFinished);
	}
	else
	{
		OnStartMovieFinished();
	}
	{
		//Load Lobby Map
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		check(PC);

		FString Command = FString::Printf(TEXT("open %s"), *UPWGameInstance::Get(this)->GetLobbyMapName());
		FString OutMsg = PC->ConsoleCommand(Command);
		PW_LOG(LogTemp, Log, TEXT("OnStartMovieFinished : %s"), *OutMsg);
	}
}

void UPWProcedureUpdate::Initilize()
{
	StartTracker();
	UPWClientNet::GetInstance(this)->Init();
	//LoadAllWWiseBank();
}

void UPWProcedureUpdate::Leave()
{
	FPWLoadingScreenModule::Get().OnStartMovieFinishedDelegate.RemoveAll(this);
	//UPWGameInstance::Get(this)->HideLoadingScreen();
}

void UPWProcedureUpdate::OnPreLoadMap(const FString& MapName)
{
	UPWGameInstance::Get(this)->ShowLoadingScreen(false);
}

void UPWProcedureUpdate::OnPostLoadMap(UWorld* LoadedWorld)
{
	UPWGameInstance::Get(this)->ShowLoadingScreen(false);
	FString LobbyName = UPWGameInstance::Get(this)->GetLobbyMapNameWithExtension();
	UGameMapsSettings::SetGameDefaultMap(LobbyName);
	PW_LOG(LogTemp, Log, TEXT("SetGameDefaultMap to %s"), *LobbyName);

	ChangeUpdateStatus(EUpdateStatus::US_LoadMapDown);
	UPWLevelMgr::Get(this)->BeginLoadSubLevels();
}

void UPWProcedureUpdate::StartTracker()
{
	UPWSaveGameManager* SaveGameMgr = UPWSaveGameManager::Get(this);
	SaveGameMgr->InitUserData();

	{ // 开启日志记录点
		UPWOtherSaveData* OtherSaveData = SaveGameMgr->GetOtherData();
		FString AdverID = OtherSaveData->GetAdvertisingId();
		bool IsNewUser = false;
		if (AdverID.IsEmpty()) {
			IsNewUser = true;
			FGuid id;
			FPlatformMisc::CreateGuid(id);
			AdverID = id.ToString();
			OtherSaveData->SetAdvertisingId(AdverID);
			SaveGameMgr->SaveOthers();
		}
		auto Tracker = DHEndpointTracker::GetInstance();
		Tracker->Init(TEXT("https://log_ps4.det-i.com/v1/tracker"), AdverID);
		Tracker->StartSession(IsNewUser, DHPlatform::GetDeviceName());
	}
}

void UPWProcedureUpdate::OnStartMovieFinished()
{
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureUpdate::OnStartMovieFinished exec"));
	UPWGameInstance::Get(this)->ShowLoadingScreen(false);
	DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_EndVideo);
	ChangeUpdateStatus(EUpdateStatus::US_StartMovieDown);
}

void UPWProcedureUpdate::ChangeUpdateStatus(EUpdateStatus Status)
{
	UpdateStatus = (EUpdateStatus)((uint8)UpdateStatus | (uint8)Status);
	if (UpdateStatus == EUpdateStatus::US_AllDown)
	{
		UPWSaveGameManager::Get(this)->Init();
		UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_Login);
	}
}

//void UPWProcedureUpdatePS4::LoadAllWWiseBank()
//{
//    auto Settings = UPWGameSettings::Get();
//    return_if_true(!Settings || !Settings->bUseWWise);
//
//    FPWWiseBank Table;
//    return_if_true(!FPWDataTable::Get().GetItemById<FPWWiseBank>(123456, Table));
//    for (int i = 0; i < Table.Banks.Num(); i++) {
//        if (Table.Banks.IsValidIndex(i) && !Table.Banks[i].IsNull()) {
//            UAkAudioBank* Bank = PWLIB::SynchronouslyLoadAsset<UAkAudioBank>(Table.Banks[i].ToSoftObjectPath());
//            continue_if_true(!Bank);
//            if (UPWGameSettings::Get()->bSyncLoadWiseBank) {
//                Bank->Load();
//            } else {
//                if (!Bank->LoadAsync((void*)OnLoadBankCompletedPS4, this)) {
//                    GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("Bank : %s Load Failed !"), *Bank->GetName()));
//                }
//            }
//            PW_LOG(LogTemp, Log, TEXT("Bank %s have Loaded!"), *Table.Banks[i].Get()->GetName());
//        }
//    }
//}

//static void OnLoadBankCompletedPS4(AkUInt32 in_bankID, const void *in_pInMemoryBankPtr, AKRESULT in_eLoadResult, AkMemPoolId in_memPoolId, void *in_pCookie)
//{
//    //UPWProcedureUpdatePS4 *Procedure = static_cast<UPWProcedureUpdatePS4*>(in_pCookie);
//    UE_LOG(LogTemp, Log, TEXT("AkAudioBankLoadLog : PWProcedureUpdate AsyncLoad result = %d"), (int)in_eLoadResult);
//    //GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Bank LoadAsync Completed !"));
//}
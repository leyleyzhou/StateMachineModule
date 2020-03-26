//  Copyright 2004-2017 Perfect World Co.,Ltd. All Rights Reserved.

#include "PWGameInstance.h"
#include "DHCommon.h"
#include "Runtime/Core/Public/Misc/NetworkVersion.h"
#include "Runtime/Core/Public/Misc/CommandLine.h"
#include "Runtime/Core/Public/Misc/Parse.h"
#include "PWFXDataStructs.h"

// Replay
#include "Runtime/NetworkReplayStreaming/NullNetworkReplayStreaming/Public/NullNetworkReplayStreaming.h"
#include "NetworkVersion.h"
#include "Engine/DemoNetDriver.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "PWDataTable.h"
#include "PWLocalPlayer.h"
#include "PWKillcamPlayback.h"
// end Replay
#include "DS/PWProcedureBattleDS.h"
#include "PWGMEMgr.h"
#include "PWLevelMgr.h"
#include "PWUserSettings.h"
#include "PWExtraEffectComponent.h"
#include "PWVoicePlatform.h"
#include "PWGameSettings.h"
#include "PWReplicationGraph.h"
#if PLATFORM_PS4
#include "Online/PS4/DHOnlinePS4.h"
#endif
#include "PWOfflineNoticeWidget.h"
#include "GenericPlatformMisc.h"
#include "PWBulletPool.h"
#include "PWNetUtility.h"
#include "ReplicationGraphTypes.h"
#include "PWLibrary.h"
#include "AssetRegistryModule.h"
#include "PWTrophyMananger.h"
#include "Werewolf/Common/PWNewLoadingWidget.h"
#include "PWLoadingWidget.h"
#include "Client/PWProcedureExitBattle.h"
#include "PWDSNet.h"
//
#include "PWGameInstance_InitialManager.inl"
#include "PWQuest/PWQuestManager.h"
#include "DateTime.h"


DEFINE_LOG_CATEGORY(LogPWGameInstance);

UPWGameInstance::UPWGameInstance()
	: ControllerMessagePrompt(nullptr)
{
	auto GetNetVersionOverride = []()
	{
		return 111;
	};
	FNetworkVersion::GetLocalNetworkVersionOverride.BindLambda(GetNetVersionOverride);
}

UPWGameInstance* UPWGameInstance::Get(const UObject* ContextObject)
{
	return_null_if_null(ContextObject);

	UWorld* World = ContextObject->GetWorld();
	return_null_if_null(World);

	return World->GetGameInstance<UPWGameInstance>();
}

void UPWGameInstance::PostInitProperties()
{
	Super::PostInitProperties();
	// init log
	GetLogManager()->SetGameInstance(this);
	GetLogManager()->Init();
}

void UPWGameInstance::StartGameInstance()
{
	Super::StartGameInstance();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	FParse::Value(FCommandLine::Get(), TEXT("MapID="), BattleMapID);

	if (UPWGameSettings::Get()->MatchSetting.bPlayWithLobby == false || GetWorld()->IsPlayInEditor())
	{
		BattleStartTimeStamp = FDateTime::UtcNow().GetTicks();
	}

	UPWProcedureManager::GetInstance(this)->Init();
#if PLATFORM_PS4
	UDHOnlinePS4::GetInstance(this)->Init();
#endif
}

#if WITH_EDITOR
FGameInstancePIEResult UPWGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	BattleMapID = UPWGameSettings::Get()->StandAloneMatchSetting.MapID;
	FGameInstancePIEResult ret = Super::StartPlayInEditorGameInstance(LocalPlayer, Params);

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	if (UPWGameSettings::Get()->MatchSetting.bPlayWithLobby == false || GetWorld()->IsPlayInEditor())
	{
		BattleStartTimeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	}

	UPWProcedureManager::GetInstance(this)->Init();
	SetBattleStartTime(FDateTime::UtcNow().ToUnixTimestamp());
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		if (UPWGameSettings::Get()->StandAloneMatchSetting.TestQuest)
		{
			UPWQuestManager::GetInstance(this)->Init();
			UPWQuestManager::GetInstance(this)->AddPieTetst();
		}
	}

	return ret;
}
#endif

void UPWGameInstance::OnGameRestart()
{
	UPWProcedureBattleDS* ProcedureDS = Cast<UPWProcedureBattleDS>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_BattleDS));
	if (ProcedureDS)
	{
		ProcedureDS->StartRestart();
		//清空一下子弹List
		if (UPWBulletPool::Get())
		{
			UPWBulletPool::Get()->EmptyBulletList();
		}
	}
}

UPWReplicationGraph* UPWGameInstance::GetReplicationGraph()
{
	return RepGraphInst;
}

void UPWGameInstance::InitReplicationGraph()
{

	TWeakObjectPtr<UPWGameInstance> WeakThis(this);

	UReplicationDriver::CreateReplicationDriverDelegate().BindLambda([WeakThis](UNetDriver* ForNetDriver, const FURL& URL, UWorld* World) -> UReplicationDriver*
		{
			if (WeakThis.IsValid())
			{
				WeakThis->RepGraphInst = NewObject<UPWReplicationGraph>(GetTransientPackage());
			}
			return WeakThis->RepGraphInst;
		});

}

void UPWGameInstance::FlushNetDormantActor(class AActor* actor) const
{
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		return;
	}
	if (RepGraphInst == nullptr ||
		actor == nullptr ||
		actor->Role < ENetRole::ROLE_Authority
		)
	{
		UE_CLOG(RepGraphInst == nullptr, LogPWGameInstance, Warning, TEXT("UPWGameInstance::FlushDormantActor return,ReplicationGraph Instance is null"));
		UE_CLOG(actor == nullptr, LogPWGameInstance, Warning, TEXT("UPWGameInstance::FlushDormantActor,Actor is null"));
		if (actor)
		{
			UE_CLOG(actor->Role < ENetRole::ROLE_Authority, LogPWGameInstance, Warning, TEXT("UPWGameInstance::FlushDormantActor,Actor Role is %s,Actor is %s"), *FPWNetUtility::NetRoleToString(actor->Role), *actor->GetName());
		}
		return;
	}

	RepGraphInst->FlushNetDormancy(actor, false);

}

void UPWGameInstance::ForceNetUpdateActor(class AActor* actor) const
{
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		return;
	}
	if (RepGraphInst == nullptr ||
		actor == nullptr ||
		actor->Role < ENetRole::ROLE_Authority
		)
	{
		UE_CLOG(RepGraphInst == nullptr, LogPWGameInstance, Warning, TEXT("UPWGameInstance::FlushDormantActor return,ReplicationGraph Instance is null"));
		UE_CLOG(actor == nullptr, LogPWGameInstance, Warning, TEXT("UPWGameInstance::FlushDormantActor,Actor is null"));
		if (actor)
		{
			UE_CLOG(actor->Role < ENetRole::ROLE_Authority, LogPWGameInstance, Warning, TEXT("UPWGameInstance::FlushDormantActor,Actor Role is %s,Actor is %s"), *FPWNetUtility::NetRoleToString(actor->Role), *actor->GetName());
		}
		return;
	}

	RepGraphInst->ForceNetUpdate(actor);
}

void UPWGameInstance::AddDependentActor(class AActor* OwnerActor, class AActor* DependentActor)
{
	if (RepGraphInst == nullptr ||
		OwnerActor == nullptr ||
		OwnerActor->Role < ENetRole::ROLE_Authority
		)
	{
		UE_CLOG(RepGraphInst == nullptr, LogPWGameInstance, Warning, TEXT("UPWGameInstance::AddDependentActor return,ReplicationGraph Instance is null"));
		UE_CLOG(OwnerActor == nullptr, LogPWGameInstance, Warning, TEXT("UPWGameInstance::AddDependentActor,Actor is null"));
		if (OwnerActor)
		{
			UE_CLOG(OwnerActor->Role < ENetRole::ROLE_Authority, LogPWGameInstance, Warning, TEXT("UPWGameInstance::AddDependentActor,Actor Role is %s,Actor is %s"), *FPWNetUtility::NetRoleToString(OwnerActor->Role), *OwnerActor->GetName());
		}
		return;
	}

	const TSharedPtr<FReplicationGraphGlobalData>& GraphGlobals = RepGraphInst->GetGraphGlobals();

	FReplicationGraphGlobalData* RGGD = GraphGlobals.Get();
	if (GraphGlobals.Get() && GraphGlobals->GlobalActorReplicationInfoMap)
	{
		FGlobalActorReplicationInfo& ActorInfo = RGGD->GlobalActorReplicationInfoMap->Get(OwnerActor);
		ActorInfo.DependentActorList.PrepareForWrite();
		ActorInfo.DependentActorList.Add(DependentActor);

	}
	UE_CLOG(GraphGlobals->GlobalActorReplicationInfoMap == nullptr, LogPWGameInstance, Warning, TEXT("UPWGameInstance::AddDependentActor,GlobalActorReplicationInfoMap is null"));
}

void UPWGameInstance::Init()
{
	Super::Init();

	// create a ReplayStreamer for FindReplays() and DeleteReplay(..)
	NullStreamsPtr = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();

	FString InMemoryStreamingFactoryName = TEXT("InMemoryNetworkReplayStreaming");
	InMemoryStreamerPtr = FNetworkReplayStreaming::Get().GetFactory(*InMemoryStreamingFactoryName).CreateReplayStreamer();

	// Link FindReplays() delegate to function
	OnEnumerateStreamsCompleteDelegate = FEnumerateStreamsCallback::CreateUObject(this, &UPWGameInstance::OnEnumerateStreamsComplete);
	// Link DeleteReplay() delegate to function
	OnDeleteFinishedStreamCompleteDelegate = FDeleteFinishedStreamCallback::CreateUObject(this, &UPWGameInstance::OnDeleteFinishedStreamComplete);

	InitReplicationGraph();


	FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UPWGameInstance::OnControllerConnectionChanged);
}

void UPWGameInstance::Shutdown()
{
	FPWDataTable::Release();
	FPWActorAsyncLoadAssetsManager::Release();
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		GetSaveGameManager()->OnGameShutDown();
	}
	else
	{
		UPWDSNet::GetInstance(this)->Exit();
	}
#if !PLATFORM_LINUX
	GetVivoxManager()->Logout();
#endif
	Super::Shutdown();
}

void UPWGameInstance::PreloadContentForURL(FURL InURL)
{
}

bool UPWGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	if (GetCheatManager()->ProcessConsoleExec(Cmd, Ar, Executor))
		return true;

	if (GetSkillManager()->ProcessConsoleExec(Cmd, Ar, Executor))
		return true;

	//if (GetTrophyMananger()->ProcessConsoleExec(Cmd, Ar, Executor))
	//	return true;

	if (GetPreloadCacheManager()->ProcessConsoleExec(Cmd, Ar, Executor))
		return true;

	if (GetLogManager()->ProcessConsoleExec(Cmd, Ar, Executor))
		return true;

	return Super::ProcessConsoleExec(Cmd, Ar, Executor);
}


void UPWGameInstance::BP_StartRecordingReplay(FString ReplayName, FString FriendlyName, ENetReplayStreamingModule StreamerModule /*= EDemoNetDriverStreamerModule::SM_NULL*/, float TimeBufferHintSeconds /*= -1*/)
{
	TArray<FString> Options;
	switch (StreamerModule)
	{
	case ENetReplayStreamingModule::SM_Memory:
	{
		if (TimeBufferHintSeconds > 0)
		{
			InMemoryStreamerPtr->SetTimeBufferHintSeconds(TimeBufferHintSeconds);
		}

		Options.Add("ReplayStreamerOverride=InMemoryNetworkReplayStreaming");
		break;
	}
	}

	StartRecordingReplay(ReplayName, FriendlyName, Options);
}

void UPWGameInstance::BP_StopRecordingReplay()
{
	StopRecordingReplay();
}

void UPWGameInstance::BP_PlayReplay(FString ReplayName, ENetReplayStreamingModule StreamerModule /*= EDemoNetDriverStreamerModule::SM_NULL*/)
{
	TArray<FString> Options;
	switch (StreamerModule)
	{
	case ENetReplayStreamingModule::SM_Memory:
		Options.Add("ReplayStreamerOverride=InMemoryNetworkReplayStreaming");
		break;
	}
	PlayReplay(ReplayName, nullptr, Options);
}

void UPWGameInstance::FindReplays()
{
	if (NullStreamsPtr.Get())
	{
		NullStreamsPtr.Get()->EnumerateStreams(FNetworkReplayVersion(), FString(), FString(), OnEnumerateStreamsCompleteDelegate);
	}
}

void UPWGameInstance::RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName)
{
	// Get File Info
	FNullReplayInfo Info;

	const FString DemoPath = FPaths::Combine(*FPaths::ProjectSavedDir(), TEXT("Demos/"));
	const FString StreamDirectory = FPaths::Combine(*DemoPath, *ReplayName);
	const FString StreamFullBaseFilename = FPaths::Combine(*StreamDirectory, *ReplayName);
	const FString InfoFilename = StreamFullBaseFilename + TEXT(".replayinfo");

	TUniquePtr<FArchive> InfoFileArchive(IFileManager::Get().CreateFileReader(*InfoFilename));

	if (InfoFileArchive.IsValid() && InfoFileArchive->TotalSize() != 0)
	{
		FString JsonString;
		*InfoFileArchive << JsonString;

		Info.FromJson(JsonString);
		Info.bIsValid = true;

		InfoFileArchive->Close();
	}

	// Set FriendlyName
	Info.FriendlyName = NewFriendlyReplayName;

	// Write File Info
	TUniquePtr<FArchive> ReplayInfoFileAr(IFileManager::Get().CreateFileWriter(*InfoFilename));

	if (ReplayInfoFileAr.IsValid())
	{
		FString JsonString = Info.ToJson();
		*ReplayInfoFileAr << JsonString;

		ReplayInfoFileAr->Close();
	}
}

void UPWGameInstance::DeleteReplay(const FString& ReplayName)
{
	if (NullStreamsPtr.Get())
	{
		NullStreamsPtr.Get()->DeleteFinishedStream(ReplayName, OnDeleteFinishedStreamCompleteDelegate);
	}
}

bool UPWGameInstance::PlayReplay(const FString& InName, UWorld* WorldOverride /* = nullptr */, const TArray<FString>& AdditionalOptions /* = TArray<FString>() */)
{
	UWorld* CurrentWorld = WorldOverride != nullptr ? WorldOverride : GetWorld();

	if (CurrentWorld == nullptr)
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("UGameInstance::PlayReplay: GetWorld() is null"));
		return false;
	}

	if (CurrentWorld->WorldType == EWorldType::PIE)
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("UGameInstance::PlayReplay: Function called while running a PIE instance, this is disabled."));
		return false;
	}

	CurrentWorld->DestroyDemoNetDriver();

	FURL DemoURL;
	PW_LOG(LogPWGameInstance, Log, TEXT("PlayReplay: Attempting to play demo %s"), *InName);

	DemoURL.Map = InName;

	for (const FString& Option : AdditionalOptions)
	{
		DemoURL.AddOption(*Option);
	}

	const FName NAME_DemoNetDriver(TEXT("DemoNetDriver"));

	if (!GEngine->CreateNamedNetDriver(CurrentWorld, NAME_DemoNetDriver, NAME_DemoNetDriver))
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("PlayReplay: failed to create demo net driver!"));
		return false;
	}

	CurrentWorld->DemoNetDriver = Cast< UDemoNetDriver >(GEngine->FindNamedNetDriver(CurrentWorld, NAME_DemoNetDriver));

	check(CurrentWorld->DemoNetDriver != NULL);

	CurrentWorld->DemoNetDriver->SetWorld(CurrentWorld);

	if (DemoURL.Map == TEXT("_DeathCam"))
	{
		CurrentWorld->DemoNetDriver->bIsLocalReplay = true;
	}
	FString Error;

	if (!CurrentWorld->DemoNetDriver->InitConnect(CurrentWorld, DemoURL, Error))
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("Demo playback failed: %s"), *Error);
		CurrentWorld->DestroyDemoNetDriver();
	}
	else
	{
		FCoreUObjectDelegates::PostDemoPlay.Broadcast();
	}


	return true;
	// destroy DemoNetDriver before load map.
	//HandlePreLoadMapDelegate = FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UPWGameInstance::HandlePreLoadMap);
	//PostLoadMapDelegateHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UPWGameInstance::HandlePostLoadMap);
}

void UPWGameInstance::StartRecordingReplay(const FString& Name, const FString& FriendlyName, const TArray<FString>& AdditionalOptions)
{
	if (FParse::Param(FCommandLine::Get(), TEXT("NOREPLAYS")))
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("UGameInstance::StartRecordingReplay: Rejected due to -noreplays option"));
		return;
	}

	UWorld* CurrentWorld = GetWorld();

	if (CurrentWorld == nullptr)
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("UGameInstance::StartRecordingReplay: GetWorld() is null"));
		return;
	}

	if (CurrentWorld->WorldType == EWorldType::PIE)
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("UGameInstance::StartRecordingReplay: Function called while running a PIE instance, this is disabled."));
		return;
	}

	if (CurrentWorld->DemoNetDriver && CurrentWorld->DemoNetDriver->IsPlaying())
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("UGameInstance::StartRecordingReplay: A replay is already playing, cannot begin recording another one."));
		return;
	}

	FURL DemoURL;
	FString DemoName = Name;

	DemoName.ReplaceInline(TEXT("%m"), *CurrentWorld->GetMapName());

	// replace the current URL's map with a demo extension
	DemoURL.Map = DemoName;
	DemoURL.AddOption(*FString::Printf(TEXT("DemoFriendlyName=%s"), *FriendlyName));
	DemoURL.AddOption(*FString::Printf(TEXT("Remote")));

	for (const FString& Option : AdditionalOptions)
	{
		DemoURL.AddOption(*Option);
	}

	bool bDestroyedDemoNetDriver = false;
	if (!CurrentWorld->DemoNetDriver || !CurrentWorld->DemoNetDriver->bRecordMapChanges || !CurrentWorld->DemoNetDriver->IsRecordingPaused())
	{
		CurrentWorld->DestroyDemoNetDriver();
		bDestroyedDemoNetDriver = true;

		const FName NAME_DemoNetDriver(TEXT("DemoNetDriver"));

		if (!GEngine->CreateNamedNetDriver(CurrentWorld, NAME_DemoNetDriver, NAME_DemoNetDriver))
		{
			PW_LOG(LogPWGameInstance, Warning, TEXT("RecordReplay: failed to create demo net driver!"));
			return;
		}

		CurrentWorld->DemoNetDriver = Cast< UDemoNetDriver >(GEngine->FindNamedNetDriver(CurrentWorld, NAME_DemoNetDriver));
	}

	check(CurrentWorld->DemoNetDriver != nullptr);

	CurrentWorld->DemoNetDriver->SetWorld(CurrentWorld);

	// Set the new demo driver as the current collection's driver
	FLevelCollection* CurrentLevelCollection = CurrentWorld->FindCollectionByType(ELevelCollectionType::DynamicSourceLevels);
	if (CurrentLevelCollection)
	{
		CurrentLevelCollection->SetDemoNetDriver(CurrentWorld->DemoNetDriver);
	}

	FString Error;

	if (bDestroyedDemoNetDriver)
	{
		if (!CurrentWorld->DemoNetDriver->InitListen(CurrentWorld, DemoURL, false, Error))
		{
			PW_LOG(LogPWGameInstance, Warning, TEXT("Demo recording - InitListen failed: %s"), *Error);
			CurrentWorld->DemoNetDriver = NULL;
			return;
		}
	}
	else if (!CurrentWorld->DemoNetDriver->ContinueListen(DemoURL))
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("Demo recording - ContinueListen failed"));
		CurrentWorld->DemoNetDriver = NULL;
		return;
	}

	//PW_LOG(LogPWGameInstance, Log, TEXT("Num Network Actors: %i"), CurrentWorld->DemoNetDriver->GetNetworkObjectList().GetActiveObjects().Num());
}

void UPWGameInstance::HandlePreLoadMap(const FString& MapName)
{
	PW_LOG(LogTemp, Warning, TEXT("UPWGameInstance::HandlePreLoadMap %s"), *MapName);
	UPWProcedureManager::GetInstance(this)->OnPreLoadMap(MapName);
	//GetWorld()->DemoNetDriver = nullptr; // The solution lies in this line
}

void UPWGameInstance::HandlePostLoadMap(UWorld* World)
{
	PW_LOG(LogTemp, Warning, TEXT("UPWGameInstance::HandlePostLoadMap world name is %s"), World != nullptr ? *World->GetMapName() : *FString(TEXT("null")));
	UPWProcedureManager::GetInstance(this)->OnPostLoadMap(World);
}

void UPWGameInstance::HandlePostLoadAllMap()
{
	PW_LOG(LogTemp, Warning, TEXT("UPWGameInstance::HandlePostLoadAllMap "));
	UPWProcedureManager::GetInstance(this)->OnPostLoadAllMaps();
}

void UPWGameInstance::OnEnumerateStreamsComplete(const FEnumerateStreamsResult& EnumerateStreamsResult)
{
	TArray<FPWReplayInfo> AllReplays;

	for (FNetworkReplayStreamInfo StreamInfo : EnumerateStreamsResult.FoundStreams)
	{
		if (!StreamInfo.bIsLive)
		{
			AllReplays.Add(FPWReplayInfo(StreamInfo.Name, StreamInfo.FriendlyName, StreamInfo.Timestamp, StreamInfo.LengthInMS));
		}
	}

	BP_OnFindReplaysComplete(AllReplays);
}

void UPWGameInstance::OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& DeleteFinishedStreamResult)
{
	FindReplays();
}

void UPWGameInstance::OnControllerConnectionChanged(bool bIsConnected, int32 UserID, int32 ControllerId)
{
	const FString ConnectionMessage = bIsConnected ? "Connected" : "Disconnected";
	PW_LOG(LogPWGameInstance, Warning, TEXT("OnControllerConnectionChanged: User %i (ControlledId %i) %s"), UserID, ControllerId, *ConnectionMessage);

#if PLATFORM_PS4
	UDHOnlinePS4* PS4 = UDHOnlinePS4::GetInstance(this);
	if (ControllerId != 0) {
		if (bIsConnected && !PS4->DialogIsVisible()) {
			PS4->ShowModalMessageDialog(DH_LOCTABLE_STR("GL_MSG_PLAYER_CHANGED"));
		}
		else if (!bIsConnected && PS4->DialogIsVisible()) {
			PS4->CloseDialog();
		}
	}
#endif

	// If a controlled is disconnected, check if it belongs to a local player
	if (!bIsConnected)
	{
		const ULocalPlayer* LocalPlayer = FindLocalPlayerFromControllerId(ControllerId);
		if (LocalPlayer)
		{
			PW_LOG(LogPWGameInstance, Warning, TEXT("OnControllerConnectionChanged: Disconnected controller matched to player %s!"), *LocalPlayer->GetNickname());
			/*

						// Show message to notify player their controller has been disconnected
						if (!ControllerMessagePrompt)
						{
							ControllerMessagePrompt = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_WithOK);
						}
						if (ControllerMessagePrompt && ControllerMessagePrompt->IsValidLowLevel())
						{
							ControllerMessagePrompt->OnPromptClosedDelegate.AddUniqueDynamic(this, &UPWGameInstance::OnControllerPromptClosed);
						}

			#if PLATFORM_PS4
						const FText PromptMessage = NSLOCTEXT("PromptMessages", "ControllerDisconnectPS4", "DUALSHOCK®4 wireless controller disconnected\nPlease reconnect wireless controller to continue");
			#else
						const FText PromptMessage = NSLOCTEXT("PromptMessages", "ControllerDisconnect", "Wireless controller disconnected\nPlease reconnect wireless controller to continue");
			#endif
						if (ControllerMessagePrompt && ControllerMessagePrompt->IsValidLowLevel())
						{
							ControllerMessagePrompt->SetPromptContent(PromptMessage);
						}*/
		}
	}
	else
	{
		/*
				const ULocalPlayer* LocalPlayer = FindLocalPlayerFromControllerId(ControllerId);
				if (!LocalPlayer)
				{
		#if PLATFORM_PS4
					UDHOnlinePS4::ShowMessageDialog({}, TEXT("The current user is not the user that started the application\nPlease switch back to the initial user or restart the application to continue"), {});
					ExecuteNextFrame(FSimpleDelegate::CreateLambda([this] {
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
							else
							{
								ProcedureManager->ChangeCurState(ProcedureState::ProcedureState_Login);
							}
						}
					}));
		#else
					if (ControllerMessagePrompt && ControllerMessagePrompt->IsValidLowLevel())
					{
						const FText PromptMessage = NSLOCTEXT("PromptMessages", "ControllerUserChange", "The current user is not the user that started the application\nPlease switch back to the initial user or restart the application to continue");
						ControllerMessagePrompt->SetPromptContent(PromptMessage);
					}
		#endif
				}*/
	}
}

void UPWGameInstance::OnControllerPromptClosed(bool bOkPressed)
{
	ControllerMessagePrompt = nullptr;
}

bool UPWGameInstance::IsKillcamWorld(UWorld* QueryWorld) const
{
	if (QueryWorld == nullptr || QueryWorld->DemoNetDriver == nullptr)
	{
		return false;
	}

	if (GetLocalPlayers().Num() == 0)
	{
		PW_LOG(LogPWGameInstance, Warning, TEXT("IsKillcamWorld : LocalPlayer num is 0!"));
		return false;
	}

	UPWLocalPlayer* Player = Cast<UPWLocalPlayer>(GetLocalPlayers()[0]);
	if (Player)
	{
		UPWKillcamPlayback* KillcamPB = Player->GetKillcamPlaybackManager();
		if (KillcamPB)
		{
			return KillcamPB->GetKillcamWorld() == QueryWorld;
		}
	}

	return false;
}

FString UPWGameInstance::GetLobbyMapName()
{
	return TEXT("/Game/Maps/Werewolf/Lobby/Lobby_Main");
}

FString UPWGameInstance::GetLobbyMapNameWithExtension()
{
	return TEXT("/Game/Maps/Werewolf/Lobby/Lobby_Main.Lobby_Main");
}

void UPWGameInstance::ShowLoadingScreen(bool IsContentVisible /* = true */)
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
		return;
	if (GetWorld()->IsPlayInEditor() == true)
		return;

	PW_LOG(LogTemp, Log, TEXT("UPWGameInstance::ShowLoadingScreen exec"));
	if (LoadingScreenWidget == nullptr)
	{
		if (LoadingScreen == nullptr)
		{
			PW_LOG(LogTemp, Error, TEXT("UPWGameInstance::InitLoadingScreenWidget, LoadingScreen is invalid"));
			return;
		}
		LoadingScreenWidget = CreateWidget<UPWLoadingWidget>(this, LoadingScreen);

	}
	if (LoadingScreenWidget)
	{
		if (LoadingScreenWidget->IsInViewport() == false)
		{
			int32 Order = UPWBlueprintLibrary::GetHighestWidgetOrder();
			LoadingScreenWidget->Priority = Order;
			LoadingScreenWidget->AddToViewport(Order);
		}
		LoadingScreenWidget->UpdateContentVisibility(IsContentVisible);
	}
}

void UPWGameInstance::UpdateLoadingScreenContentVisibility(bool inIsVisible)
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->UpdateContentVisibility(inIsVisible);
	}
}

void UPWGameInstance::HideLoadingScreen()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
		return;

	PW_LOG(LogTemp, Log, TEXT("UPWGameInstance::HideLoadingScreen exec"));
	if (LoadingScreenWidget && LoadingScreenWidget->IsInViewport())
	{
		LoadingScreenWidget->RemoveFromViewport();
		LoadingScreenWidget->OnHide();
	}
}

void UPWGameInstance::ShowNewLoadingScreen(EUMGLoadingSceneType type)
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
		return;
	/*if (GetWorld()->IsPlayInEditor() == true)
		return;*/

	PW_LOG(LogTemp, Log, TEXT("UPWGameInstance::ShowLoadingScreen exec"));
	if (GetUIManager())
	{
		GetUIManager()->ShowNewLoadingScreen(type);
	}
}
void UPWGameInstance::HideNewLoadingScreen()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
		return;

	PW_LOG(LogTemp, Log, TEXT("UPWGameInstance::HideNewLoadingScreen exec"));
	if (GetUIManager())
	{
		GetUIManager()->HideNewLoadingScreen();
	}
}

UClass* UPWGameInstance::GetPawnClassByCharacterType(ECharacterType CharacterType)
{
	for (auto CharacterTypeData : CharacterTypeDataList)
	{
		if (CharacterTypeData.CharacterType == CharacterType)
			return *CharacterTypeData.PawnClass;
	}
	return nullptr;
}

UClass* UPWGameInstance::GetControllerClassByCharacterType(ECharacterType CharacterType)
{
	for (auto CharacterTypeData : CharacterTypeDataList)
	{
		if (CharacterTypeData.CharacterType == CharacterType)
			return *CharacterTypeData.ControllerClass;
	}
	return nullptr;
}



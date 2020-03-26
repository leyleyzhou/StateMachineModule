// Copyright 2016 P906, Perfect World, Inc.

#include "Client/PWProcedurePostLogin.h"
#include "PWGameInstance.h"
#include "PWProcedureManager.h"
#include "PWLibrary.h"
#include "Client/PWProcedurePolicy.h"
#include "DETNet/DETNetPlayerData.h"
#include "Client/PWProcedureLobby.h"
#include "DETNet/DETNetClient.h"
#include "PWTrophyMananger.h"
#include "PWUIManager.h"
#include "Lobby/PWPostLoginWidget.h"
#include "Online/DHEndpointTracker.h"
#include "PWLobbyTeamManager.h"
#include "PWFriendsManager.h"
#include "DHInternational.h"
#include "PWClientNet.h"
#include "PWSaveGameManager.h"

UPWProcedurePostLogin::UPWProcedurePostLogin()
{
	CanSkipVideo = false;
	VideoPlayed = false;
}

void UPWProcedurePostLogin::Enter()
{
	UPWFriendsManager::GetInstance(this)->Init();
	UPWTrophyMananger::GetInstance(this)->Init();


	UPWSaveGameManager* SaveGameManager = UPWBlueprintLibrary::GetSaveGameManager(this);
	if (SaveGameManager && SaveGameManager->GetPWSGTutorial())
	{
		UPWSaveGameTutorial* SGTutorial = SaveGameManager->GetPWSGTutorial();

		auto Config = GetInstanceOfJson<FPWTutorialVersionConfig>();
		if (Config)
		{
			if (SGTutorial->GetStringValueByType(EPWSGType::EPWSGType_TutorialVersion) != Config->Version)
			{
				SGTutorial->SetBoolValueByType(EPWSGType::EPWSGType_TutorialVis, true);
			}
		}
		else
		{
			PW_LOG(LogTemp, Warning, TEXT("UPWProcedurePostLogin::Enter : SaveGameManager is null!"));
		}
	}

	//if (DHInternational::GetInstance())
	//{
	//	DHInternational::GetInstance()->GetOnDisplayLanguageUpdated().RemoveAll(this);
	//	DHInternational::GetInstance()->GetOnDisplayLanguageUpdated().AddUObject(this, &UPWProcedurePostLogin::OnCurLanguageChanged);

	//	const FString CurLang = DHInternational::GetInstance()->GetDisplayLanguage();
	//	UPWClientNet::GetInstance(this)->ChangeLanguageReq(CurLang);
	//}

	if (VideoPlayed || UPWLobbyTeamManager::GetInstance(this)->IsWaittingForEnterTeam() || UPWLobbyTeamManager::GetInstance(this)->HavePlayTogetherEventToDeal())
	{
		ChangeToNextState();
		return;
	}
	DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_StartLobbyVideo);
	TWeakObjectPtr<UPWUIManager> WeakUIManager = UPWUIManager::Get(this);
	bool CanSkip = CanSkipVideo;
	FPWUIOpenDelegate UIOpenDel = FPWUIOpenDelegate::CreateLambda([WeakUIManager, CanSkip]() {
		if (!WeakUIManager.IsValid())
		{
			return;
		}
		auto PostLoginWidget = Cast<UPWPostLoginWidget>(WeakUIManager->GetWidget(EUMGID::EUMGID_WW_PostLoginVideo));
		if (PostLoginWidget)
		{
			PostLoginWidget->StartMovie(CanSkip);
		}
		});
	WeakUIManager->ShowUI(EUMGID::EUMGID_WW_PostLoginVideo, UIOpenDel);
	VideoPlayed = true;
}


void UPWProcedurePostLogin::Leave()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	check(UIManager);
	if (!UIManager->IsInViewport(EUMGID::EUMGID_WW_PostLoginVideo))
	{
		return;
	}
	auto PostLoginWidget = Cast<UPWPostLoginWidget>(UIManager->GetWidget(EUMGID::EUMGID_WW_PostLoginVideo));
	if (PostLoginWidget)
	{
		PostLoginWidget->StopPlayMovie();
	}
}

void UPWProcedurePostLogin::SetNextState(ProcedureState inNextState)
{
	NextState = inNextState;
}

void UPWProcedurePostLogin::ChangeToNextState()
{
	UPWProcedureManager::GetInstance(GetWorld())->ChangeCurState(NextState);
}

//void UPWProcedurePostLogin::OnCurLanguageChanged(const FString& Language)
//{
//	UPWClientNet::GetInstance(this)->ChangeLanguageReq(Language);
//}


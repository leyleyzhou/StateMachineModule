// Copyright 2016 P906, Perfect World, Inc.

#include "Client/PWProcedureCreateChar.h"

#include "PWUIManager.h"
#include "UIWidgetHead.h"
#include "GameFramework/PlayerController.h"
#include "Classes/Engine/World.h"
#include "PWLibrary.h"
#include "PWStreamingLevelManager.h"
#include "Werewolf/Lobby/PWLobbyWidgetV2.h"
#include "Online/DHEndpointTracker.h"
#include "PWLobbyTeamManager.h"
#include "PWPromptManager.h"
#include "PWPromptBase.h"

void UPWProcedureCreateChar::Enter()
{
	// --- wzt 打点 ---
	DHEndpointTracker::GetInstance()->Track(EEndpointTrackEvent::TE_StartCreateCharacter);
	// --- wzt ---

	// 防止跳伞突然清库导致的脸部被切
	GNearClippingPlane = 10.f;

	// TODO： Mode Change
	TWeakObjectPtr<UPWProcedureCreateChar> WeakThis(this);

	LobbyLevel::SLM_Callback CallBackFunc = LobbyLevel::SLM_Callback::CreateLambda([WeakThis](ULevelStreaming* LevelStream)
		{
			if (WeakThis.Get())
			{
				UPWUIManager::Get(WeakThis.Get())->ShowUI(EUMGID::EUMGID_CreateChar);
				UPWUIManager::Get(WeakThis.Get())->HideUI(EUMGID::EUMGID_Connecting);
			}
		});
	LobbyLevel::RefreshSubLevelVisibility(this, ELobbySubLevelType::CreateCharacter, CallBackFunc);

	APlayerController* TargetPC = GetWorld()->GetFirstPlayerController();
	if (TargetPC)
	{
		if (UPWBlueprintLibrary::IsPlayWithPS4() == false)
		{
			TargetPC->ConsoleCommand(TEXT("r.Streaming.FullyLoadUsedTextures 1"), true);
		}
	}
	if (UPWLobbyTeamManager::GetInstance(this)->IsWaittingForEnterTeam() || UPWLobbyTeamManager::GetInstance(this)->HavePlayTogetherEventToDeal())
	{
		UPWPromptBase* Prompt = UPWPromptManager::Get(this)->ShowPrompt(ePromptType::EPT_Base);
		if (Prompt)
		{
			FText text = FText::FromStringTable("Lang", "GL_MSG_NOID");

			Prompt->SetPromptContent(text);
		}
		UPWLobbyTeamManager::GetInstance(this)->DealEnterTeam(false);
	}
}

void UPWProcedureCreateChar::Leave()
{
	// TODO： Mode Change
	UPWUIManager::Get(this)->HideUI(EUMGID::EUMGID_CreateChar);
}

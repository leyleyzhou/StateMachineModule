// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GameStateMachineGameMode.h"
#include "GameStateMachineCharacter.h"
#include "UObject/ConstructorHelpers.h"


#include "PWGameMode.h"
#include "PWHUD.h"
#include "PlayerController/PWPlayerController.h"
#include "PWPlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "PWGameState.h"
#include "PWGameSession.h"
#include "PWGameSettings.h"
#include "PWCharacterManager.h"
#include "PWRouteManager.h"
#include "PWAirForteManager.h"
#include "PWLibrary.h"
#include "PWDoubleCircleManager.h"
#include "PWAirDropManager.h"
#include "PWSafeCircleManager.h"
#include "PWDelegateManager.h"
#include "PWDSNet.h"
#include "DHCharacterDataStructs.h"

AGameStateMachineGameMode::AGameStateMachineGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

APWGameMode::APWGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HUDClass = APWHUD::StaticClass();
	PlayerControllerClass = APWPlayerController::StaticClass();
	PlayerStateClass = APWPlayerState::StaticClass();
	SpectatorClass = ASpectatorPawn::StaticClass();
	GameStateClass = APWGameState::StaticClass();
	GameSessionClass = APWGameSession::StaticClass();
	ReplaySpectatorPlayerControllerClass = ASpectatorPawn::StaticClass();
}

void APWGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FinishMatchSecond = UPWGameSettings::Get()->MatchSetting.FinishMatchSecond;
	APWGameState* PWGameState = Cast<APWGameState>(GameState);
	if (PWGameState && PWGameState->IsValidLowLevelFast())
	{
		PWGameState->SetWaittingForStartCD(-1);
	}

	UWorld* MyWorld = GetWorld();
	if (MyWorld)
	{
		MyWorld->GetWorldSettings()->bUseClientSideLevelStreamingVolumes = true;
		UE_LOG(LogTemp, Warning, TEXT("setClientSideLevelStreamingVolumes"));
	}
}

bool APWGameMode::ReadyToStartMatch_Implementation()
{
	return  FMath::IsNearlyZero(CountDownTimeSecond, 0.01f);
}

bool APWGameMode::ReadyToEndMatch_Implementation()
{
	return ReadEndMatch != 0;
}

void APWGameMode::HandleMatchIsWaitingToStart()
{
	ReadEndMatch = false;

	CountDownTimeSecond = UPWGameSettings::Get()->MatchSetting.CountDownTimeSecond;
	CountDownStarted = false;
	UPWDelegateManager::Get(this)->OnCharacterLoadAllMap.AddUniqueDynamic(this, &APWGameMode::OnCharacterLoadAllMap);

	switch (UPWBlueprintLibrary::GetMapType(this))
	{
	case EMapType::EMapType_Normal:
		if (UPWGameSettings::Get()->MatchSetting.bEnableParachute)
		{
			UPWAirForteManager::Get(this)->Init();
		}
		if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
		{
			UPWSafeCircleManager::Get(this)->ResetData();
			UPWSafeCircleManager::Get(this)->OnSafeCircleChanged.AddUniqueDynamic(this, &APWGameMode::OnSafeCircleChanged);
		}
		break;
	case EMapType::EMapType_Corps:
		if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
		{
			UPWDoubleCircleManager::Get(this)->ResetData();
		}
		break;
	case EMapType::EMapType_MiniBattle:
		if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
		{
			UPWSafeCircleManager::Get(this)->ResetData();
			UPWSafeCircleManager::Get(this)->OnSafeCircleChanged.AddUniqueDynamic(this, &APWGameMode::OnSafeCircleChanged);
		}
		break;
	case EMapType::EMapType_BattleRoyale:
		if (UPWGameSettings::Get()->MatchSetting.bEnableParachute)
		{
			UPWAirForteManager::Get(this)->Init();
		}
		if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
		{
			UPWSafeCircleManager::Get(this)->ResetData();
			UPWSafeCircleManager::Get(this)->OnSafeCircleChanged.AddUniqueDynamic(this, &APWGameMode::OnSafeCircleChanged);
		}
		break;
	}
	Super::HandleMatchIsWaitingToStart();
}

void APWGameMode::HandleMatchHasStarted()
{
	UPWDelegateManager::Get(this)->OnCharacterLoadAllMap.RemoveAll(this);

	UPWDSNet::GetInstance(this)->SendBeginFightNotify(60 * 40);
	APWGameState* PWGS = Cast<APWGameState>(GameState);
	if (PWGS)
	{
		PWGS->TotalPlayerAmount = PWGS->AlivePlayerAmount;
		PWGS->FightingStartTime = GetWorld()->GetTimeSeconds();
	}

	switch (UPWBlueprintLibrary::GetMapType(this))
	{
	case EMapType::EMapType_Normal:
		if (UPWGameSettings::Get()->MatchSetting.bEnableParachute)
		{
			UPWCharacterManager::Get(this)->AllCharactersGetOnPlane(ECharacterType::CT_Human);
			UPWAirForteManager::Get(this)->BeginFly();
			UPWDelegateManager::Get(this)->OnCharacterCanJumpFromAirforte.AddUniqueDynamic(this, &APWGameModeBase::OnEnterParachuteArea);
			UPWDelegateManager::Get(this)->OnCharacterForceJumpFromAirforte.AddUniqueDynamic(this, &APWGameModeBase::OnLeaveParachuteArea);
		}
		else
		{
			UPWCharacterManager::Get(this)->AllCharactersStartFight();
			if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
			{
				UPWSafeCircleManager::Get(this)->LaunchSafeCircle();
			}
			UPWAirDropManager::Get(this)->PrepareForSystemAirDrop();
		}
		break;
	case EMapType::EMapType_Corps:
		if (UPWGameSettings::Get()->MatchSetting.bEnableParachute)
		{
			UPWCharacterManager::Get(this)->AllCharactersTeleportToPlayerStart(ECharacterType::CT_Human, true);
		}
		else
		{
			UPWCharacterManager::Get(this)->AllCharactersStartFight();
		}
		if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
		{
			UPWDoubleCircleManager::Get(this)->LaunchCircle();
		}
		UPWAirDropManager::Get(this)->PrepareForSystemAirDrop();
		break;
	case EMapType::EMapType_MiniBattle:
		if (UPWGameSettings::Get()->MatchSetting.bEnableParachute)
		{
			UPWCharacterManager::Get(this)->AllCharactersTeleportToPlayerStart(ECharacterType::CT_Human, true);
		}
		else
		{
			UPWCharacterManager::Get(this)->AllCharactersStartFight();
		}
		if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
		{
			UPWSafeCircleManager::Get(this)->LaunchSafeCircle();
		}
		UPWAirDropManager::Get(this)->PrepareForSystemAirDrop();
		break;
	case EMapType::EMapType_BattleRoyale:
		if (UPWGameSettings::Get()->MatchSetting.bEnableParachute)
		{
			UPWCharacterManager::Get(this)->AllCharactersGetOnPlane(ECharacterType::CT_Human);
			UPWAirForteManager::Get(this)->BeginFly();
			UPWDelegateManager::Get(this)->OnCharacterCanJumpFromAirforte.AddUniqueDynamic(this, &APWGameMode::OnEnterParachuteArea);
			UPWDelegateManager::Get(this)->OnCharacterForceJumpFromAirforte.AddUniqueDynamic(this, &APWGameMode::OnLeaveParachuteArea);
		}
		else
		{
			UPWCharacterManager::Get(this)->AllCharactersStartFight();
			if (UPWGameSettings::Get()->MatchSetting.bEnableSafeCircle)
			{
				UPWSafeCircleManager::Get(this)->LaunchSafeCircle();
			}
			UPWAirDropManager::Get(this)->PrepareForSystemAirDrop();
		}
		break;
	}

	Super::HandleMatchHasStarted();
}

void APWGameMode::HandleMatchHasEnded()
{
	UPWCharacterManager::Get(this)->AllCharactersGetReweard();
	if (UPWBlueprintLibrary::GetMapType(this) == EMapType::EMapType_Corps)
	{
		UPWDoubleCircleManager::Get(this)->FinishCircle();
	}
	else
	{
		UPWSafeCircleManager::Get(this)->FinishSafeCircle();
	}

	Super::HandleMatchHasEnded();
}

void APWGameMode::Tick_WaittingForStart(float DeltaSeconds)
{
	if (CountDownStarted == true)
	{
		if (CountDownTimeSecond > 0.0f)
		{
			CountDownTimeSecond -= DeltaSeconds;
			if (CountDownTimeSecond <= 0.0f)
			{
				CountDownTimeSecond = 0.0f;
			}
			APWGameState* PWGameState = Cast<APWGameState>(GameState);
			if (PWGameState && PWGameState->IsValidLowLevelFast())
			{
				PWGameState->SetWaittingForStartCD(CountDownTimeSecond);
			}
		}
	}
}

void APWGameMode::Tick_InProgress(float DeltaSeconds)
{
	if (UPWCharacterManager::Get(this)->GetFightingTeamNum() <= 1)
	{
		_EndMatch();
	}
}

void APWGameMode::Tick_WaitingPostMatch(float DeltaSeconds)
{
	bool IsAnyoneInMatch = false;
	const TArray<APWCharacterData_Runtime*>& CharList = UPWCharacterManager::Get(this)->GetCharaterList();
	for (auto CharData : CharList)
	{
		if (CharData->GetCharacterState() < ECharacterState::ECS_Leaved)
		{
			IsAnyoneInMatch = true;
			break;
		}
	}
	if (IsAnyoneInMatch == false)
	{
		RestartGame();
	}
	else
	{
		FinishMatchSecond -= DeltaSeconds;
		if (FinishMatchSecond < 0)
		{
			RestartGame();
		}
	}
}

void APWGameMode::_EndMatch()
{
	if (UPWGameSettings::Get()->MatchSetting.bCanEndMatch)
		ReadEndMatch = true;
}

void APWGameMode::UpdateCountDownSeconds(int32 inSeconds)
{
	CountDownTimeSecond = inSeconds;
	APWGameState* GS = Cast<APWGameState>(GameState);
	if (GS)
	{
		GS->SetWaittingForStartCD(CountDownTimeSecond);
	}
}

void APWGameMode::UpdateFinishMatchSeconds(int32 inSeconds)
{
	FinishMatchSecond = inSeconds;
}

void APWGameMode::OnSafeCircleChanged()
{
	UE_LOG(LogTemp, Log, TEXT("++++++ UPWSafeCircleManager OnSafeCircleChanged"));
	UPWSafeCircleManager* SafeCircleManager = UPWSafeCircleManager::Get(this);
	APWGameState* PWGS = Cast<APWGameState>(GameState);
	if (SafeCircleManager && PWGS)
	{
		PWGS->SafeCircleInfo.State = SafeCircleManager->SafeCircleState;
		PWGS->SafeCircleInfo.StartTimeSeconds = SafeCircleManager->StartSeconds;
		PWGS->SafeCircleInfo.Index = SafeCircleManager->CurrentCircleIndex;
		PWGS->SafeCircleInfo.CurCenter = SafeCircleManager->GetCurrentCircleCenter();
		PWGS->SafeCircleInfo.CurRadius = SafeCircleManager->GetCurrentCircleRadius();
		PWGS->SafeCircleInfo.PreCenter = SafeCircleManager->GetPreviousCircleCenter();
		PWGS->SafeCircleInfo.PreRadius = SafeCircleManager->GetPreviousCircleRadius();
		PWGS->SafeCircleInfo.StartTimeSeconds = GetWorld()->GetTimeSeconds();
	}
}

void APWGameMode::OnDoubleSafeCircleChanged()
{

}

void APWGameMode::OnCharacterLoadAllMap()
{
	if (CountDownStarted == false)
	{
		CountDownStarted = true;
		UPWDelegateManager::Get(this)->OnCharacterLoadAllMap.RemoveAll(this);
	}
}


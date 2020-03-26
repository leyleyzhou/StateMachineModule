// Copyright 2004-2019 Perfect World Co.,Ltd. All Rights Reserved.

#include "Client/PWProcedureExitBattle.h"
#include "GameFramework/PlayerController.h"
#include "PWGameInstance.h"
#include "PWProcedureManager.h"
#include "PWLevelMgr.h"

void UPWProcedureExitBattle::Enter()
{
	//	UPWGameInstance::Get(this)->ShowLoadingScreen();
	UPWGameInstance::Get(this)->ShowNewLoadingScreen(EUMGLoadingSceneType::EUMGLoadingSceneType_AfterBattle);

	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		FString LobbyName = UPWGameInstance::Get(this)->GetLobbyMapName();
		FString cmd = TEXT("open ");
		cmd += LobbyName;
		PC->ConsoleCommand(cmd);
	}
}

void UPWProcedureExitBattle::Leave()
{
	NextState = ProcedureState::ProcedureState_Max;
	UPWGameInstance::Get(this)->HideLoadingScreen();
	UPWGameInstance::Get(this)->HideNewLoadingScreen();
}

void UPWProcedureExitBattle::OnPostLoadMap(UWorld* LoadedWorld)
{
	UPWLevelMgr::Get(this)->BeginLoadSubLevels();
}

void UPWProcedureExitBattle::OnPostLoadAllMaps()
{
	UPWProcedureManager::GetInstance(this)->ChangeCurState(NextState);
}

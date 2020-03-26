// Copyright 2016 P906, Perfect World, Inc.

#include "PWProcedureManager.h"
#include "PWGameInstance.h"
#include "Client/PWProcedureCreateChar.h"
#include "Client/PWProcedureLobby.h"
#include "Utility/PWLibrary.h"
#include "PWGameSettings.h"
#include "Client/PWProcedureBattle.h"
#include "Client/PWProcedureSelectLobby.h"
#include "Client/PWProcedureSelectArea.h"
#include "Client/PWProcedurePolicy.h"
#include "Client/PWProcedureStandaloneCopy.h"
#include "GameMode/PWStandaloneGameMode.h"
#include "Client/PWProcedureExitBattle.h"
#if PLATFORM_PS4
#   include "Client/PS4/PWProcedureLoginPS4.h"
#endif
#include "Client/PC/PWProcedureLogin.h"
#include "Client/PWProcedureUpdate.h"
#include "Client/PWProcedurePostLogin.h"
#include "DS/PWProcedureBattleDS.h"
#include "DS/PWProcedureUpdateDS.h"
#include "TimerManager.h"

void UPWProcedureManager::Init()
{
	CurState = ProcedureState::ProcedureState_Max;
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UPWProcedureUpdateDS* UpdateProcedure = NewObject<UPWProcedureUpdateDS>(this);
		ProcedureList.Add(UpdateProcedure);
		UPWProcedureBattleDS* BattleProcedure = NewObject<UPWProcedureBattleDS>(this);
		ProcedureList.Add(BattleProcedure);
	}
	else
	{
#if PLATFORM_PS4
		UPWProcedureLoginPS4* loginProcedure = NewObject<UPWProcedureLoginPS4>(this);
#else
		UPWProcedureLogin* loginProcedure = NewObject<UPWProcedureLogin>(this);
#endif
		UPWProcedureUpdate* updateProcedure = NewObject<UPWProcedureUpdate>(this);
		ProcedureList.Add(updateProcedure);
		ProcedureList.Add(loginProcedure);
		UPWProcedurePostLogin* PostLoginProcedure = NewObject<UPWProcedurePostLogin>(this);
		ProcedureList.Add(PostLoginProcedure);
		/*UPWProcedureSelectArea* SelectAreaProcedure = NewObject<UPWProcedureSelectArea>(this);
		ProcedureList.Add(SelectAreaProcedure);
		UPWProcedureSelectLobby* selectLobbyProcedure = NewObject<UPWProcedureSelectLobby>(this);
		ProcedureList.Add(selectLobbyProcedure);*/
		UPWProcedurePolicy* PolicyProcedure = NewObject<UPWProcedurePolicy>(this);
		ProcedureList.Add(PolicyProcedure);
		UPWProcedureCreateChar* CreateCharProcedure = NewObject<UPWProcedureCreateChar>(this);
		ProcedureList.Add(CreateCharProcedure);
		UPWProcedureStandaloneCopy* StandaloneCopyProcedure = NewObject<UPWProcedureStandaloneCopy>(this);
		ProcedureList.Add(StandaloneCopyProcedure);
		UPWProcedureLobby* LobbyProcedure = NewObject<UPWProcedureLobby>(this);
		ProcedureList.Add(LobbyProcedure);
		UPWProcedureBattle* battleProcedure = NewObject<UPWProcedureBattle>(this);
		ProcedureList.Add(battleProcedure);
		UPWProcedureExitBattle* exitBattleProcedure = NewObject<UPWProcedureExitBattle>(this);
		ProcedureList.Add(exitBattleProcedure);
	}

	ChangeCurState(ProcedureState::ProcedureState_Update);
}

void UPWProcedureManager::ChangeCurState(ProcedureState state)
{
	FString EnumName(TEXT("ProcedureState"));
	FString strCurType = PWLIB::GetEnumValueAsString(EnumName, (int32)CurState);
	FString strNextType = PWLIB::GetEnumValueAsString(EnumName, (int32)state);
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureManager::ChangeCurState, CurState = %s(%d), NextState = %s(%d)"), *strCurType, (uint8)CurState, *strNextType, (uint8)state);
	if (NextState == ProcedureState::ProcedureState_Max)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPWProcedureManager::ChangeStateInternal, 0.0001f, false);
	}
	NextState = state;
}

ProcedureState UPWProcedureManager::GetCurState()
{
	return CurState;
}

UPWProcedureBase* UPWProcedureManager::GetProcedure(ProcedureState state)
{
	if (this == nullptr)
		return nullptr;

	for (int32 index = 0; index < ProcedureList.Num(); ++index)
	{
		UPWProcedureBase* procedure = ProcedureList[index];
		if (procedure->GetState() == state)
			return procedure;
	}
	return nullptr;
}

UPWProcedureBase* UPWProcedureManager::Procedure(UObject* ContextObject, ProcedureState state)
{
	UPWProcedureManager* ProcedureManager = UPWProcedureManager::GetInstance(ContextObject);
	return ProcedureManager->GetProcedure(state);
}

void UPWProcedureManager::OnPreLoadMap(const FString& MapName)
{
	FString EnumName(TEXT("ProcedureState"));
	FString strType = PWLIB::GetEnumValueAsString(EnumName, (int32)CurState);
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureManager::OnPreLoadMap, CurState = %s(%d)"), *strType, (uint8)CurState);
	UPWProcedureBase* procedure = GetProcedure(CurState);
	if (procedure)
	{
		procedure->OnPreLoadMap(MapName);
	}
}

void UPWProcedureManager::OnPostLoadMap(UWorld* LoadedWorld)
{
	auto SelfWorld = GetWorld();
	if (SelfWorld && SelfWorld->IsPlayInEditor() && LoadedWorld)
	{
		//PIE模式下检查InstanceID是否匹配
		auto PIEInstanceID = SelfWorld->GetOutermost()->PIEInstanceID;
		if (PIEInstanceID != LoadedWorld->GetOutermost()->PIEInstanceID)
		{
			return;
		}
	}
	FString EnumName(TEXT("ProcedureState"));
	FString strType = PWLIB::GetEnumValueAsString(EnumName, (int32)CurState);
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureManager::OnPostLoadMap, CurState = %s(%d)"), *strType, (uint8)CurState);
	UPWProcedureBase* procedure = GetProcedure(CurState);
	if (procedure)
	{
		procedure->OnPostLoadMap(LoadedWorld);
	}
}

void UPWProcedureManager::OnPostLoadAllMaps()
{
	FString EnumName(TEXT("ProcedureState"));
	FString strType = PWLIB::GetEnumValueAsString(EnumName, (int32)CurState);
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureManager::OnPostLoadAllMaps, CurState = %s(%d)"), *strType, (uint8)CurState);
	UPWProcedureBase* procedure = GetProcedure(CurState);
	if (procedure)
	{
		procedure->OnPostLoadAllMaps();
	}
}

void UPWProcedureManager::ChangeStateInternal()
{
	FString EnumName(TEXT("ProcedureState"));
	FString strCurType = PWLIB::GetEnumValueAsString(EnumName, (int32)CurState);
	FString strNextType = PWLIB::GetEnumValueAsString(EnumName, (int32)NextState);
	PW_LOG(LogTemp, Log, TEXT("UPWProcedureManager::ChangeStateInternal, CurState = %s(%d), NextState = %s(%d)"), *strCurType, (uint8)CurState, *strNextType, (uint8)NextState);
	UPWProcedureBase* CurProcedure = GetProcedure(CurState);
	UPWProcedureBase* NextProcedure = GetProcedure(NextState);
	CurState = NextState;
	NextState = ProcedureState::ProcedureState_Max;
	if (CurProcedure)
	{
		CurProcedure->Leave();
	}
	if (NextProcedure)
	{
		NextProcedure->Enter();
	}
}

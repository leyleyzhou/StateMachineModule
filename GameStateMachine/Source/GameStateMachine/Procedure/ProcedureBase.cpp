// Copyright 2016 P906, Perfect World, Inc.

#include "PWProcedureBase.h"
#include "PWProcedureManager.h"




void UPWProcedureBase::Enter()
{
}

void UPWProcedureBase::Leave()
{
}

FString UPWProcedureBase::GetStringFromProcedure(const ProcedureState procedure)
{
	switch (procedure)
	{
	case ProcedureState::ProcedureState_Update:
		return FString(TEXT("Update"));
		break;
	case ProcedureState::ProcedureState_Login:
		return FString(TEXT("Login"));
		break;
	case ProcedureState::ProcedureState_SelectArea:
		return FString(TEXT("SelectArea"));
		break;
	case ProcedureState::ProcedureState_SelectLobby:
		return FString(TEXT("SelectLobby"));
		break;
	case ProcedureState::ProcedureState_Policy:
		return FString(TEXT("Policy"));
		break;
	case ProcedureState::ProcedureState_CreateChar:
		return FString(TEXT("CreateChar"));
		break;
	case ProcedureState::ProcedureState_StandaloneCopy:
		return FString(TEXT("StandaloneCopy"));
		break;
	case ProcedureState::ProcedureState_Lobby:
		return FString(TEXT("Lobby"));
		break;
	case ProcedureState::ProcedureState_Battle:
		return FString(TEXT("Battle"));
		break;
	case ProcedureState::ProcedureState_BattleDS:
	case ProcedureState::ProcedureState_Max:
	default:
		break;
	}
	return FString();

}

bool UPWProcedureBase::IsCurrentState()
{
	return UPWProcedureManager::GetInstance(this)->GetCurState() == GetState();
}

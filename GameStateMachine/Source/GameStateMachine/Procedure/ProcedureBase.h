// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PWProcedureBase.generated.h"


UENUM(BlueprintType)
enum class ProcedureState : uint8
{
	ProcedureState_Update = 0,
	//Client
	ProcedureState_Login,
	ProcedureState_PostLogin,
	ProcedureState_SelectArea,
	ProcedureState_SelectLobby,
	ProcedureState_Policy,
	ProcedureState_CreateChar,
	ProcedureState_StandaloneCopy,
	ProcedureState_Lobby,
	ProcedureState_Battle,
	ProcedureState_ExitBattle,
	//DS
	ProcedureState_BattleDS,		//Õ½¶··þ
	ProcedureState_Max,
};
/**
 *
 */
UCLASS()
class PWGAME_API UPWProcedureBase : public UObject
{
	GENERATED_BODY()

		friend class UPWProcedureManager;
public:
	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_Max; }
	virtual void Enter();
	virtual void Leave();

	static FString GetStringFromProcedure(const ProcedureState procedure);

	bool IsCurrentState();

protected:
	//load map interface
	virtual void OnPreLoadMap(const FString& MapName) {}
	virtual void OnPostLoadMap(UWorld* LoadedWorld) {}
	virtual void OnPostLoadAllMaps() {}
	//load map interface



};

// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "PWProcedureBase.h"
#include "PWGameInstance.h"
#include "UObject/NoExportTypes.h"
#include "PWProcedureManager.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class PWGAME_API UPWProcedureManager : public UObject, public DHGameInstance<UPWProcedureManager>
{
	GENERATED_BODY()
public:
	void Init();

	UFUNCTION(BlueprintCallable, Category = "Procedure")
		void ChangeCurState(ProcedureState state);

	UFUNCTION(BlueprintPure, Category = "Procedure")
		ProcedureState GetCurState();

	UFUNCTION(BlueprintPure, Category = "procedure")
		UPWProcedureBase* GetProcedure(ProcedureState state);

	UFUNCTION(BlueprintPure, Category = "procedure")
		static UPWProcedureBase* Procedure(UObject* ContextObject, ProcedureState state);
public:

	virtual void OnPreLoadMap(const FString& MapName);
	virtual void OnPostLoadMap(UWorld* LoadedWorld);
	virtual void OnPostLoadAllMaps();

	UFUNCTION()
		void ChangeStateInternal();

	bool GetIsChangingState() const { return NextState != ProcedureState::ProcedureState_Max; }

private:
	ProcedureState CurState = ProcedureState::ProcedureState_Max;
	ProcedureState NextState = ProcedureState::ProcedureState_Max;

	UPROPERTY()
		TArray<UPWProcedureBase*>		ProcedureList;
};

// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Procedure/PWProcedureBase.h"
#include "IHttpRequest.h"
#include "DETNet/DETNetClient.h"
#include "WWise/PWWWiseDataStructs.h"
#include "PWProcedureUpdate.generated.h"



UCLASS()
class PWGAME_API UPWProcedureUpdate : public UPWProcedureBase
{
	GENERATED_BODY()
public:
	enum class EUpdateStatus : uint8
	{
		US_None = 0,
		US_LoadMapDown = 1U << 0,
		US_StartMovieDown = 1U << 1,
		US_AllDown = US_LoadMapDown | US_StartMovieDown,
		US_HasError = 1U << 7
	};

	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_Update; }


	virtual void Enter();
	virtual void Leave();

	void OnStartMovieFinished();

	void ChangeUpdateStatus(EUpdateStatus Status);

protected:
	virtual void OnPreLoadMap(const FString& MapName);
	virtual void OnPostLoadMap(UWorld* LoadedWorld);

	void StartTracker();
	void Initilize();


	EUpdateStatus UpdateStatus = EUpdateStatus::US_None;
	//protected:
	//	void LoadAllWWiseBank();
};

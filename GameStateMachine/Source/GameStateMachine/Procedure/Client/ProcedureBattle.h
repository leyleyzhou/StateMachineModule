// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "Procedure/PWProcedureBase.h"
#include "Engine/EngineTypes.h"
#include "Classes/Engine/EngineBaseTypes.h"
#include "PWCharacterData_Runtime.h"
#include "DETNet/DETNetStruct.h"
#include "PWTUnionStruct.h"
#include "PWProcedureBattle.generated.h"

DECLARE_MULTICAST_DELEGATE(FPWOnPostLoadAllMaps);
class UNetDriver;
class APWSkyLightBlender;

USTRUCT()
struct FHUDTaskInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FText Target;
	UPROPERTY()
		FText Content;

	UPROPERTY()
		FText QuestType;
};

/**
 *
 */
UCLASS(BlueprintType)
class PWGAME_API UPWProcedureBattle : public UPWProcedureBase
{
	GENERATED_BODY()

public:
	virtual ProcedureState GetState() { return ProcedureState::ProcedureState_Battle; }
	virtual void Enter();
	virtual void Leave();

	//Before Battle 
	void SetToken(const FString& inToken) { Token = inToken; }
	void SetDSAddr(const FString& addr) { DSAddr = addr; }

	void SetBattleId(const int64& InBattleID) { BattleId = InBattleID; }
	int64 GetBattleId() const {
		return BattleId;
	}
	void SetShowEnemyInfo(bool inValue) { _bShowEnemyInfo = inValue; }
	bool IsEnemyInfoShow() { return _bShowEnemyInfo; }
	void SetEnemyInfoVisibleDistance(float inDist) { _EnemyInfoVisibleDistance = inDist; }
	float GetEnemyInfoVisibleDistance() { return _EnemyInfoVisibleDistance; }

	void SetTeamID(uint64 inTeamID);
	uint64 GetTeamID() { return _TeamID; }

	void UpdateBattleUI();

	FBox GetMaxVolumeOfAll() const;

	void ConnectionLost();

	const FCMD_S2C_BattleAwardNotify& GetRewardV2() { return SelfRewardV2; }

	void OnPlayerRewardNotify(FCMD_S2C_BattleAwardNotify inReward);

	UFUNCTION(BlueprintCallable, Category = "UI")
		void BackToLobby();

	UFUNCTION(BlueprintCallable, Category = "UI")
		void OnDeathTipClosed(bool bMatchOver);
	void OnItemRewardWindowClosed();

	UFUNCTION(BlueprintCallable, Category = "UI")
		void OnEndTipClosed();

	UFUNCTION()
		void PullStopService();

	void CheckStopServiceMsg();

	uint8 GetRewardInited() { return _bIsRewardInited; }

	void RefreshPingWidgetBySettingParam();

	void ShowPingWidgetLogic(bool IsShow);

	void AddHUDTaskInfo(FText TaskContent, FText TaskTarget, FText QuestType);

	TArray<FHUDTaskInfo>& GetHUDTaskInfos() {
		return TaskInfos;
	}

protected:
	//load map interface
	virtual void OnPreLoadMap(const FString& MapName) override;
	virtual void OnPostLoadMap(UWorld* LoadedWorld) override;
	virtual void OnPostLoadAllMaps() override;

	UFUNCTION()
		void OnEnterBattleSucceeded();
	void OnNetworkFailed(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void OnTravelFailed(UWorld* World, ETravelFailure::Type NetDriver, const FString& ErrorString);

	//battling
	UFUNCTION()
		void OnCharacterStateChange(uint8 state);

	UFUNCTION()
		void OnDayTimeChanged(EPWDayTimeType From, EPWDayTimeType To);

	void FindSkyLightBlender();

	UFUNCTION()
		void OnAllSkyLightDataLevelLoaded();

	UPROPERTY()
		APWSkyLightBlender* SkyLightBlender;

	UFUNCTION()
		void OnLocalRuntimeDataInited();

	UFUNCTION()
		void OnQuestComplete(const FPWTUnionStruct& QuestStruct);

private:
	void _PreLoadUI();

	void _ShowWaittingForStartUI(class UPWUIManager* UIManager);
	void _ShowParachuteUI(class UPWUIManager* UIManager);
	void _ShowFightingUI(class UPWUIManager* UIManager);
	void _ShowGameOverUI(class UPWUIManager* UIManager);
	void _ShowEndUI();

	//狼人模式
	void _ShowWaittingForStartUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData);
	void _ShowPrepareForFightUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData);	//on the plane
	void _ShowParachuteUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData);	//human ParachuteUI
	void _ShowFightingUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData);
	void _ShowGameOverUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData);
	void _ShowEndUI_Werewolf(class UPWUIManager* UIManager, class APWCharacterData_Runtime* RuntimeData);

	void HideTotalLoadings();
	void _ApplyConsoleConfig();
	void _CalculateMaxVolume();

	void _SetVivoxAreaChannelName();
	void _LeaveVivoxChannels();
private:
	FString			DSAddr;
	FString			Token;
	int64           BattleId;
	bool			_bShowEnemyInfo = true;
	float			_EnemyInfoVisibleDistance = 10000.0f;
	uint64			_TeamID = 0;

	FDelegateHandle		EnterBattleFailed;
	FDelegateHandle		LeaveBattleMapLoadedHandle;
	FCMD_S2C_BattleAwardNotify SelfRewardV2;

	uint8				_bIsRewardInited = false;
	uint8				_bIsEndingTipPlayed = false;

	FTimerHandle		_InitRewardTimerHandle;

	FTimerHandle		PullStopServiceTimerHandle;

	FBox _MaxVolumeOfLevels;

	uint8 bAllSkyDataLevelLoaded = false;

	UPROPERTY()
		bool IsInitial = false;

	//游戏内任务完成提示
	TArray<FHUDTaskInfo> TaskInfos;
};

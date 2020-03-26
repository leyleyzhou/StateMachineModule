// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


//USTRUCT(BlueprintType)
//struct PWGAME_API FPWBattlePassRewardConfig : public FTableRowBase
//{
//	GENERATED_BODY()
//
//public:
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWBPRewardConfig")
//		int32 TaskID = -1;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWBPRewardConfig")
//		FPWBPLevelAwards AwardsInfo;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWBPRewardConfig")
//		int32 SpecialShowAwardLevel = -1;
//
//	FPWBattlePassRewardConfig() : TaskID(-1), SpecialShowAwardLevel(-1) {}
//
//};
//MAKE_DH_DATATABLE_WITH_INT32_KEY(FPWBattlePassRewardConfigTable, FPWBattlePassRewardConfig, TEXT("/Game/DataTables/DHDT/BattlePass/BattlePassAwardsInfo.BattlePassAwardsInfo"), TaskID);


//int32 CurBPSeasonVal = NetManager->GetPlayerFlagValueByIndex(PLAYER_FLAG32::Player_Flag32_BattlePass_ID);
//const FPWBattlePassConfig* BattlePassData = FPWBattlePassConfigTable::GetInstance()->GetRowByKey(CurBPSeasonVal);
//if (BattlePassData)
//{
//	return BattlePassData->SeasonIDOnlyForShow;
//}

class FBasePluginModule : public IModuleInterface
{
public:



	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

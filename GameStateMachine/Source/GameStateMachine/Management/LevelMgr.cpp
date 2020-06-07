#include "PWLevelMgr.h"
#include "PWLibrary.h"
#include "PWGameInstance.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/GameplayStatics.h"
#include "Classes/Engine/LevelStreaming.h"
#include "PWProcedureManager.h"
#include "Classes/Engine/WorldComposition.h"
#include "Public/TimerManager.h"
#include "PlayerController/PWPlayerController.h"
#include "Classes/Kismet/KismetSystemLibrary.h"
#include "Engine/LevelBounds.h"
#include "Optimization/PWGameOptimizationSettings.h"
#include "HAL/FileManagerGeneric.h"
#include "PWPreloadCacheManager.h"
#include "PWStreamingLevelManager.h"

static const FString GamePathFlag = "/Game";
const int32 MAX_DISTANCE = 99999999;

//const FString StringText_LandScape = TEXT("LandSplit");
//const FString StringText_House = TEXT("House");
//const FString StringText_PrismCity_LD = TEXT("PrismCity_LD");
//const FString StringText_Landscape_Global = TEXT("Landscape_Global");
//
//
//namespace PWLevelLayer {
//	const FString String_NoStreaming = TEXT("NoStreaming");
//	const FString String_UnCategorized = TEXT("UnCategorized");
//	const FString String_LD = TEXT("LD");
//	const FString String_Props = TEXT("Props");
//	const FString String_House = TEXT("House");
//	const FString String_Land = TEXT("Land");
//	const int32 DefaultStreamingDistance = 50000;
//	const int32 HouseStreamingDistance = 200000;
//	const int32 MaxStreamingDistance = 9000000;
//}

UPWLevelMgr::UPWLevelMgr(const FObjectInitializer& ObjectIntializer /*= FObjectInitializer::Get()*/)
	:Super(ObjectIntializer)
{

}

UPWLevelMgr* UPWLevelMgr::Get(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
	{
		return GameInstance->GetLevelManager();
	}
	return nullptr;
}

//void UPWLevelMgr::HandlePostLoadMap(UWorld* LoadedWorld)
//{
//	//stops level streaming to confirm player enter
//
//	ToggleAllLevelStreaming(false);
//	//
//	UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::HandlePostLoadMap++++"));
//	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
//	{
//		UPWLevelMgr::Get(this)->BeginLoadSubLevels();
//		UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::SetWorldSetting---111 "));
//	}
//}

void UPWLevelMgr::BeginLoadSubLevels()
{
	UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::BeginLoadSubLevels-----"));

	//-----------------------------------------------------------------------
	UWorld* World = GetWorld();
	if (World && World->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		const FString LevelName = World->GetName();
		if (LevelName == TEXT("Lobby_Main"))
		{
			UPWGameInstance* GI = Cast<UPWGameInstance>(World->GetGameInstance());
			if (GI)
			{
				UE_LOG(LogTemp, Log, TEXT("--------- LevelName is Lobby_Main ---------"));
				GI->HandlePostLoadAllMap();
				return;
			}
		}
	}
	//-----------------------------------------------------------------------

	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		RestrictLevelStreamingFramePayload();

		FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
			[this](float) -> bool
			{
				if (!IsValidLowLevel())
				{
					return false;
				}

				if (HasAllSubLevelsLoaded_Client(true))
				{
					UnrestrictLevelStreamingFramePayload();
					return false;
				}

				return true;
			}
		),
			0.5
				);
	}

	TotalLoadTime = GetWorld()->GetTimeSeconds();

	if (GetWorld()->GetWorldSettings()->bEnableWorldComposition)
	{
		if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			ServerLoadMapAccordingByConfig();
			//ToggleAllLevelStreaming(true);
			if (!GetWorld()->GetTimerManager().IsTimerActive(ServerLoadCheckTH))
			{
				GetWorld()->GetTimerManager().SetTimer(ServerLoadCheckTH, this, &UPWLevelMgr::ServerAllLoadedTimer, 0.1f, true);
			}
		}
		else
		{
			//get start to load sub levels
			SetLevelStreamingInfoOnMapLoaded();
		}

	}
	else
	{
		TryToLoad();
	}
}


void UPWLevelMgr::ServerLoadMapAccordingByConfig(bool bToggleStreaming /*= true*/)
{
	UPWPreloadCacheManager::Get(this)->ServerLoadMap();
	return;

	// get all level paths.
	TArray<FString> LevelPathsList;
	FPWMapConfig MC;
	bool success = UPWBlueprintLibrary::GetMapConfig(this, MC);
	if (success)
	{
		for (int32 i = 0; i < MC.LevelStreamingInfo.Num(); ++i)
		{
			FString LevelPath = MC.LevelStreamingInfo[i].LevelPath;
			LevelPathsList.AddUnique(LevelPath);
			UE_LOG(LogTemp, Warning, TEXT("Server test level path %s "), *LevelPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::ServerLoadMapAccordingByConfig() failed  MapID = %d"), UPWBlueprintLibrary::GetMapID(this));
	}
	//
	UWorldComposition* WC = GetWorld()->WorldComposition;
	if (WC && LevelPathsList.Num() > 0)
	{
		TArray<FWorldCompositionTile>& TileList = WC->GetTilesList();

		for (FWorldCompositionTile& WCTile : TileList)
		{
			if (IsExistInMapConfig(WCTile.PackageName.ToString(), LevelPathsList))
			{
				WCTile.Info.Layer.DistanceStreamingEnabled = bToggleStreaming;
				WCTile.Info.Layer.StreamingDistance = MAX_DISTANCE;
				UE_LOG(LogTemp, Warning, TEXT("Server test load level name %s "), *WCTile.PackageName.ToString());
			}
			//UE_LOG(LogTemp, Warning, TEXT("Server test check level name %s "), *WCTile.PackageName.ToString());
		}
		UE_LOG(LogTemp, Warning, TEXT("ServerLoadMapAccordingByConfig set streaming level  %d"), bToggleStreaming);
	}
}

bool UPWLevelMgr::IsExistInMapConfig(const FString PackageName, TArray<FString> PathsList)
{
	for (auto LevelPath : PathsList)
	{
		if (PackageName.Contains(LevelPath, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;
}

void UPWLevelMgr::ServerAllLoadedTimer()
{
	if (IsServerAllLoaded())
	{
		GetWorld()->GetTimerManager().ClearTimer(ServerLoadCheckTH);

		UE_LOG(LogTemp, Warning, TEXT("ServerAllLoadedTimer"));
		FinishLoadSubLevels();
	}
}

void UPWLevelMgr::ClientAllLoadedTimer()
{
	if (ClientAllLoaded())
	{
		GetWorld()->GetTimerManager().ClearTimer(ClientLoadCheckTH);

		UE_LOG(LogTemp, Warning, TEXT("ClientAllLoadedTimer"));
		FinishLoadSubLevels();
	}

}

void UPWLevelMgr::GetLevelsByPredicate(FString Predicate, TArray<FName>& outlevels)
{
	if (Predicate.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("GetLevelsByPredicate string is null"));
		return;
	}

	//UE_LOG(LogTemp, Error, TEXT("GetLevelsByPredicate string is %s"), *Predicate);
	for (ULevelStreaming* LevelStreaming : GetWorld()->GetStreamingLevels())
	{

		if (LevelStreaming != nullptr)
		{
			FName LevelName = LevelStreaming->GetWorldAssetPackageFName();

			if (LevelName.ToString().Contains(Predicate))
			{
				outlevels.Add(LevelName);
				//UE_LOG(LogTemp, Error, TEXT("GetLevelsByPredicate outlevels %s"), *LevelName.ToString());
			}
		}
	}
}

void UPWLevelMgr::GetLevelsByPredicate(FString Predicate, TArray<ULevelStreaming*>& outlevels)
{
	if (Predicate.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("GetLevelsByPredicate string is null"));
		return;
	}

	//UE_LOG(LogTemp, Error, TEXT("GetLevelsByPredicate string is %s"), *Predicate);
	for (ULevelStreaming* LevelStreaming : GetWorld()->GetStreamingLevels())
	{

		if (LevelStreaming != nullptr)
		{
			FName LevelName = LevelStreaming->GetWorldAssetPackageFName();

			if (LevelName.ToString().Contains(Predicate))
			{
				outlevels.Add(LevelStreaming);
				//UE_LOG(LogTemp, Error, TEXT("GetLevelsByPredicate outlevels %s"), *LevelName.ToString());
			}
		}
	}
}


void UPWLevelMgr::LoadSubLevelsByConfig()
{
	FPWMapConfig Config;
	if (UPWBlueprintLibrary::GetMapConfig(this, Config) == false)
		return;

	int64 BattleStartTime = UPWGameInstance::Get(this)->GetBattleStartTime();
	for (auto& Info : Config.LevelStreamingInfo)
	{
		int64 StartTime = Info.StartTime.ToUnixTimestamp();
		int64 EndTime = Info.EndTime.ToUnixTimestamp();
		if (BattleStartTime >= StartTime && BattleStartTime <= EndTime)
		{
			FString Path = Info.LevelPath;
			UE_LOG(LogTemp, Log, TEXT("UPWLevelMgr::LoadSubLevelsByConfig Start Load LevelPath = %s"), *Path);
			UPWStreamingLevelManager::Get(this)->LoadStreamingLevel(Path, true, UPWStreamingLevelManager::FCallbackFunction::CreateLambda([Path](ULevelStreaming*) {
				UE_LOG(LogTemp, Log, TEXT("UPWLevelMgr::LoadSubLevelsByConfig Load Complete LevelPath = %s"), *Path);
				}));
		}
	}
}

void UPWLevelMgr::ClientLevelStreamingPause(int32 bPause)
{
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		float MaxSceTime = 999999999.f;
		float MinSceTime = 3.f;
		UWorldComposition* WC = GetWorld()->WorldComposition;
		if (WC)
		{
			bool Pause = bPause > 0 ? true : false;
			if (Pause)
			{
				WC->TilesStreamingTimeThreshold = MaxSceTime;
				GetWorld()->FlushLevelStreamingType = EFlushLevelStreamingType::None;
				UE_LOG(LogTemp, Warning, TEXT("ClientLevelStreamingPause= %f"), MaxSceTime);
			}
			else
			{
				WC->TilesStreamingTimeThreshold = MinSceTime;
				GetWorld()->FlushLevelStreamingType = EFlushLevelStreamingType::Full;
				UE_LOG(LogTemp, Warning, TEXT("ClientLevelStreamingPause= %f"), MinSceTime);
			}
		}
	}
}

void UPWLevelMgr::ClientLandScapeLOD(int32 Lod)
{
	//for (int32 LevelIndex = 0; LevelIndex < GetWorld()->StreamingLevels.Num(); LevelIndex++)
	//{
	//	ULevelStreaming* LevelStreaming = GetWorld()->StreamingLevels[LevelIndex];

	//	if (LevelStreaming != nullptr)
	//	{
	//		FName LevelName = LevelStreaming->GetWorldAssetPackageFName();
	//		if (LevelName.ToString().Contains(StringText_LandScape))
	//		{
	//			LevelStreaming->LevelLODIndex = Lod;
	//			UE_LOG(LogTemp, Warning, TEXT("Set %s ToLOD %d"), *LevelName.ToString(), Lod);
	//		}

	//	}
	//}
}


void UPWLevelMgr::SetTilesStreamingTimeThreshold(float TimeThreshold)
{
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{

		float MinSceTime = 3.f;
		UWorldComposition* WC = GetWorld()->WorldComposition;
		if (WC)
		{
			WC->TilesStreamingTimeThreshold = MinSceTime;
			UE_LOG(LogTemp, Warning, TEXT("ClientLevelStreamingPause= %f"), WC->TilesStreamingTimeThreshold);
		}
	}

}

void UPWLevelMgr::SetLevelStreamingInfoByLevelPath(const FString& LevelPath, bool bToggleStreaming, const int32 DistanceForStreaming)
{
	if (LevelPath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("SetLevelStreamingInfoByLevelPath LayerPath is Empty"));
		return;
	}

	UWorldComposition* WC = GetWorld()->WorldComposition;
	if (WC)
	{
		TArray<FWorldCompositionTile>& TileList = WC->GetTilesList();

		for (FWorldCompositionTile& WCTile : TileList)
		{
			if (WCTile.PackageName.ToString().Contains(LevelPath, ESearchCase::IgnoreCase))
			{
				WCTile.Info.Layer.DistanceStreamingEnabled = bToggleStreaming;
				if (DistanceForStreaming >= 0)
				{
					WCTile.Info.Layer.StreamingDistance = DistanceForStreaming;
				}

				UE_LOG(LogTemp, Warning, TEXT("SetLevelStreamingInfoByLevelPath  LevelPath is %s ,levelname is %s,bToggleStreaming %d,distance= %d"), *LevelPath, *WCTile.PackageName.ToString(), bToggleStreaming, WCTile.Info.Layer.StreamingDistance);
			}
		}
		//WC->UpdateStreamingState();
	}

}

/*
void UPWLevelMgr::SetGCPauseForAWhile(float PausedSec)
{
	class APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("SetGCPauseForWhile failed PC == NULL"));
	}

	FString Cmd = TEXT("s.ForceGCAfterLevelStreamedOut 0");
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), Cmd, PC);
	UE_LOG(LogTemp, Warning, TEXT("SetGCPauseForWhile"));

	FLatentActionInfo LatentInfo;
	LatentInfo.UUID = LoadUID++;
	LatentInfo.Linkage = LoadUID++;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName(TEXT("DelaySetGCPauseExc"));
	UKismetSystemLibrary::Delay(GetWorld(), PausedSec, LatentInfo);
}

void UPWLevelMgr::DelaySetGCPauseExc()
{
	class APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("DelaySetGCPauseExc failed PC == NULL"));
	}

	FString Cmd = TEXT("s.ForceGCAfterLevelStreamedOut 1");
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), Cmd, PC);
	UE_LOG(LogTemp, Warning, TEXT("DelaySetGCPauseExc "));
}
*/

extern void SetConsoleVariable(const FString& InName, int32 InValue);
extern void SetConsoleVariable(const FString& InName, float InValue);

void UPWLevelMgr::FinishLoadSubLevels()
{
	TotalLoadTime = GetWorld()->GetTimeSeconds() - TotalLoadTime;
	UE_LOG(LogTemp, Warning, TEXT("FinishLoadSubLevels LoadTime = %f"), TotalLoadTime);

	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		if (GetWorld()->GetWorldSettings()->bEnableWorldComposition)
		{
			UWorldComposition* WC = GetWorld()->WorldComposition;
			if (WC)
			{
				UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::SetWorldSetting--- 222"));
				for (ULevelStreaming* Level : WC->TilesStreaming)
				{
					ULevel* lv = Level->GetLoadedLevel();
					if (lv)
					{
						AWorldSettings* WorldSetting = lv->GetWorldSettings();
						if (WorldSetting)
						{
							WorldSetting->SetReplicates(false);
							UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::SetWorldSetting not replication %s"), *Level->GetWorldAssetPackageName());
						}

					}
				}
			}
		}
	}

	//Notify 
	//UPWGameInstance::Get(this)->GetProcedureManager()->OnPostLoadAllMaps();
	UPWGameInstance* GI = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		GI->HandlePostLoadAllMap();
	}

}

void UPWLevelMgr::TryToLoad()
{
	if (this == nullptr || GetWorld() == nullptr)
	{
		return;
	}

	LoadSubLevelsByConfig();

	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		if (HasAllSubLevelsLoaded_Server())
		{
			FinishLoadSubLevels();
			return;
		}

		ServerDoLoad();
	}
	else
	{
		if (HasAllSubLevelsLoaded_Client())
		{
			FinishLoadSubLevels();
			return;
		}

		ClientDoLoad();
	}
}


void UPWLevelMgr::ToggleAllLevelStreaming(bool bToggleStreaming)
{
	UWorldComposition* WC = GetWorld()->WorldComposition;
	if (WC)
	{
		TArray<FWorldCompositionTile>& TileList = WC->GetTilesList();

		for (FWorldCompositionTile& WCTile : TileList)
		{
			WCTile.Info.Layer.DistanceStreamingEnabled = bToggleStreaming;

		}
		UE_LOG(LogTemp, Warning, TEXT("ToggleAllLevelStreaming  bToggleStreaming %d"), bToggleStreaming);
	}
}

void UPWLevelMgr::SetLevelStreamingInfoOnMapLoaded()
{
	UPWPreloadCacheManager::Get(this)->OnMapLoaded();
	return;

	FPWMapConfig MC;
	bool success = UPWBlueprintLibrary::GetMapConfig(this, MC);
	if (success)
	{
		for (int32 i = 0; i < MC.LevelStreamingInfo.Num(); ++i)
		{
			bool bStartLoaded = MC.LevelStreamingInfo[i].bStartLoadedWithMaxDistance;
			FString LevelPath = MC.LevelStreamingInfo[i].LevelPath;
			int32 StreamingDistance = 0;
			if (bStartLoaded)
			{
				StreamingDistance = MAX_DISTANCE;
			}
			else
			{
				StreamingDistance = MC.LevelStreamingInfo[i].MapLoadedDistance;
			}

			SetLevelStreamingInfoByLevelPath(LevelPath, true, StreamingDistance);

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::SetLevelStreamingInfoOnMapLoaded() failed  MapID = %d"), UPWBlueprintLibrary::GetMapID(this));
	}
}

void UPWLevelMgr::SetLevelStreamingInfoOnThePlane()
{
	UPWPreloadCacheManager::Get(this)->OnThePlane();
	return;

	//FPWMapConfig  MC;
	//bool success = UPWBlueprintLibrary::GetMapConfig(this, MC);
	//if (success)
	//{
	//	for (int32 i = 0; i < MC.LevelStreamingInfo.Num(); ++i)
	//	{
	//		bool ret = MC.LevelStreamingInfo[i].bPauseStreaming_OnThePlane;
	//		if (ret)
	//		{
	//			FString LevelPath = MC.LevelStreamingInfo[i].LevelPath;
	//			SetLevelStreamingInfoByLevelPath(LevelPath, !ret);
	//		}

	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::SetLevelStreamingInfoOnThePlane() failed  MapID = %d"), UPWBlueprintLibrary::GetMapID(this));
	//}
}

void UPWLevelMgr::SetLevelStreamingInfoOnCharacterPrepareLanding()
{
	FPWMapConfig  MC;
	bool bSuccess = UPWBlueprintLibrary::GetMapConfig(this, MC);
	if (bSuccess)
	{

		for (int32 i = 0; i < MC.LevelStreamingInfo.Num(); ++i)
		{
			bool bOverwriteDistance = MC.LevelStreamingInfo[i].bUse_CharacterPrepareLandingDistance_Overwrite;
			if (bOverwriteDistance)
			{
				FString LevelPath = MC.LevelStreamingInfo[i].LevelPath;
				int32 MaxStreamingDistance = MC.LevelStreamingInfo[i].CharacterPrepareLandingDistance;
				SetLevelStreamingInfoByLevelPath(LevelPath, true, MaxStreamingDistance);
			}

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::SetLevelStreamingInfoOnCharacterPrepareLanding() failed  MapID = %d"), UPWBlueprintLibrary::GetMapID(this));
	}
}

void UPWLevelMgr::SetLevelStreamingInfoOnCharacterLanding()
{
	UPWPreloadCacheManager::Get(this)->OnCharacterLanding();
}

void UPWLevelMgr::HLODSetting_PerpareSkyDiving(class APlayerController* PC)
{
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("HLODSetting_GameBeginPlay failed PC == NULL"));
	}
	FString Cmd = TEXT("r.HLOD.DistanceOverride 50000");
	FString Cmd2 = TEXT("r.HLOD.DistanceScale 1");
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), Cmd, PC);
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), Cmd2, PC);
	UE_LOG(LogTemp, Warning, TEXT("HLODSetting_PerpareSkyDiving"));

}

void UPWLevelMgr::HLODSetting_PerpareLanding(class APlayerController* PC)
{
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("HLODSetting_GameBeginPlay failed PC == NULL"));
	}
	FString Cmd = TEXT("r.HLOD.DistanceOverride 20000");
	FString Cmd2 = TEXT("r.HLOD.DistanceScale 1");
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), Cmd, PC);
	UE_LOG(LogPWPC, Warning, TEXT("HLODSetting_PerpareLanding"));
}

void UPWLevelMgr::SetHLODDistanceOverride(class APlayerController* PC, float HLODDistance)
{
	FString Cmd = TEXT("r.HLOD.DistanceOverride ") + FString::SanitizeFloat(HLODDistance);
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), Cmd, PC);
	UE_LOG(LogPWPC, Warning, TEXT("SetHLODDistanceOverride %d"), HLODDistance);
}

static int32 PWLevelMgr_LevelIndex = -1;
void UPWLevelMgr::ClientDoLoad()
{
	if (PWLevelMgr_LevelIndex > -1)
		return;

	PWLevelMgr_LevelIndex = 0;
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float) -> bool {
		if (!IsValidLowLevel())
		{
			PWLevelMgr_LevelIndex = -1;
			return false;
		}
		const TArray<ULevelStreaming*>& LevelStreamings = GetWorld()->GetStreamingLevels();
		while (LevelStreamings.IsValidIndex(PWLevelMgr_LevelIndex))
		{
			ULevelStreaming* LevelStreaming = LevelStreamings[PWLevelMgr_LevelIndex++];

			if (LevelStreaming == nullptr)
			{
				continue;
			}

			if (!LevelStreaming->ShouldBeAlwaysLoaded())
			{
				continue;
			}

			if (LevelStreaming->HasLoadedLevel())
			{
				continue;
			}

			if (LevelStreaming->HasLoadRequestPending())
			{
				continue;
			}

			FLatentActionInfo LatentInfo;
			LatentInfo.UUID = LoadUID++;
			LatentInfo.Linkage = LoadUID++;
			FName LevelName = LevelStreaming->GetWorldAssetPackageFName();
			UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, LatentInfo);

			return true;
		}

		PWLevelMgr_LevelIndex = -1;
		return false;
		}), 0.2f);

	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float) -> bool {
		if (!IsValidLowLevel())
			return false;

		if (HasAllSubLevelsLoaded_Client(true))
		{
			FinishLoadSubLevels();
			return false;
		}

		return true;
		}), 0.5f);
}

void UPWLevelMgr::ServerDoLoad()
{
	for (ULevelStreaming* LevelStreaming : GetWorld()->GetStreamingLevels())
	{
		if (LevelStreaming == nullptr)
		{
			continue;
		}

		if (LevelStreaming->HasLoadedLevel())
		{
			continue;
		}

		if (LevelStreaming->HasLoadRequestPending())
		{
			continue;
		}

		FName LevelName = LevelStreaming->GetWorldAssetPackageFName();

		if (IsSubLevelInServerLoadingBlacklist(LevelName))
		{
			continue;
		}

		FLatentActionInfo LatentInfo;
		LatentInfo.UUID = LoadUID++;
		LatentInfo.Linkage = LoadUID++;
		UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, LatentInfo);
	}

	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float) -> bool {
		if (!IsValidLowLevel())
		{
			return false;
		}

		if (HasAllSubLevelsLoaded_Server())
		{
			FinishLoadSubLevels();
			return false;
		}

		return true;
		}), 0.5f);
}

bool UPWLevelMgr::HasAllSubLevelsLoaded_Client(bool OnlyCheckAlwaysLoadedSubLevel /*= false*/)
{
	for (ULevelStreaming* LevelStreaming : GetWorld()->GetStreamingLevels())
	{
		UE_LOG(LogTemp, Log, TEXT("UPWLevelMgr::HasAllSubLevelsLoaded_Client LevelName = %s %s"), *LevelStreaming->GetName(), *LevelStreaming->GetWorldAssetPackageName());
		if (OnlyCheckAlwaysLoadedSubLevel && !LevelStreaming->ShouldBeAlwaysLoaded())
		{
			continue;
		}

		if (LevelStreaming != nullptr && !LevelStreaming->IsLevelLoaded())
		{
			return false;
		}
#if WITH_EDITOR
		if (GetWorld()->IsPlayInEditor() && LevelStreaming && LevelStreaming->GetCurrentState() < ULevelStreaming::ECurrentState::LoadedVisible)
		{
			return false;
		}
#endif
	}

	return true;
}

bool UPWLevelMgr::HasAllSubLevelsLoaded_Server()
{
	for (ULevelStreaming* StreamedSubLevel : GetWorld()->GetStreamingLevels())
	{
		FName LevelName = StreamedSubLevel->GetWorldAssetPackageFName();
		if (IsSubLevelInServerLoadingBlacklist(LevelName))
		{
			continue;
		}

		if (StreamedSubLevel != nullptr && !StreamedSubLevel->IsLevelLoaded())
		{
			return false;
		}
	}

	return true;
}

bool UPWLevelMgr::IsServerAllLoaded()
{
	return UPWPreloadCacheManager::Get(this)->IsServerAllLoaded();
}

bool UPWLevelMgr::ClientAllLoaded()
{
	bool ret = false;
	FPWMapConfig  MC;
	bool success = UPWBlueprintLibrary::GetMapConfig(this, MC);
	if (!success)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::ClientAllLoaded() failed  MapID = %d"), UPWBlueprintLibrary::GetMapID(this));
		return true;
	}

	for (ULevelStreaming* LevelStreaming : GetWorld()->GetStreamingLevels())
	{

		if (LevelStreaming != nullptr && !LevelStreaming->IsLevelVisible())
		{
			FName LevelName = LevelStreaming->GetWorldAssetPackageFName();


			for (int32 i = 0; i < MC.LevelStreamingInfo.Num(); ++i)
			{
				if (MC.LevelStreamingInfo[i].bStartLoadedWithMaxDistance == false)
				{
					continue;
				}
				FString LevelPath = MC.LevelStreamingInfo[i].LevelPath;

				if (LevelName.ToString().Contains(LevelPath))
				{
					return ret;
				}

			}

		}
	}

	ret = true;
	return ret;
}

bool UPWLevelMgr::IsInFilterList()
{
	return false;
}


void UPWLevelMgr::UnrestrictLevelStreamingFramePayload()
{
	UE_LOG(LogTemp, Log, TEXT("UnrestrictLevelStreamingFramePayload() Invoked!"));

	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	SetConsoleVariable(TEXT("s.LevelStreamingActorsUpdateTimeLimit"), 5.0f);
	SetConsoleVariable(TEXT("r.Streaming.NumStaticComponentsProcessedPerFrame"), 25);
	//SetConsoleVariable(TEXT("r.DistanceFieldGI"), 1);
	SetConsoleVariable(TEXT("r.DistanceFieldAO"), 1);
	SetConsoleVariable(TEXT("r.DistanceFieldShadowing"), 1);
}

bool UPWLevelMgr::IsSubLevelInServerLoadingBlacklist(const FName& LevelName)
{
	auto& ServerSubLevelBlackList = GetMutableDefault<UPWGameOptimizationSettings>()->SubLevelsBlacklistForServer;
	return ServerSubLevelBlackList.Find(LevelName) != INDEX_NONE;
}

void UPWLevelMgr::RestrictLevelStreamingFramePayload()
{
	UE_LOG(LogTemp, Log, TEXT("RestrictLevelStreamingFramePayload Invoked!"));

	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	SetConsoleVariable(TEXT("s.ForceGCAfterLevelStreamedOut"), 0);
	SetConsoleVariable(TEXT("r.DistanceFieldGI"), 0);
	SetConsoleVariable(TEXT("r.DistanceFieldAO"), 0);
	SetConsoleVariable(TEXT("r.DistanceFieldShadowing"), 0);
	SetConsoleVariable(TEXT("s.LevelStreamingActorsUpdateTimeLimit"), 20.0f);
	SetConsoleVariable(TEXT("s.LevelStreamingComponentsRegistrationGranularity"), 1);
	SetConsoleVariable(TEXT("s.LevelStreamingComponentsUnregistrationGranularity"), 1);
	SetConsoleVariable(TEXT("r.Streaming.NumStaticComponentsProcessedPerFrame"), 50);
	SetConsoleVariable(TEXT("r.Streaming.DropMips"), 2);

}

TMap<FString, bool> UPWLevelMgr::GetLevelFilesFromDT()
{
	if (bGetDTOnce)
	{
		return LevelFilesToLoad;
	}
	//
	FPWMapConfig  MC;
	bool success = UPWBlueprintLibrary::GetMapConfig(this, MC);
	if (!success)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::GetLevelFilesFromDT() failed  MapID = %d"), UPWBlueprintLibrary::GetMapID(this));
		return LevelFilesToLoad;
	}

	// get all level paths.
//	IFileManager& FileManager = FFileManagerGeneric::Get();
//	if (success)
//	{
//		for (int32 i = 0; i < MC.LevelStreamingInfo.Num(); ++i)
//		{
//			FString LevelPath = MC.LevelStreamingInfo[i].LevelPath;
//			FString BindingsDir = FPaths::ProjectContentDir() / LevelPath.Right(LevelPath.Len() - GamePathFlag.Len());
//#if WITH_EDITOR
//			UE_LOG(LogTemp, Warning, TEXT("Server test level path %s "), *BindingsDir);
//#endif
//			BindingsDir = FileManager.ConvertToAbsolutePathForExternalAppForRead(*BindingsDir);
//			//
//			TArray<FString> FoundFiles;
//			FileManager.FindFiles(FoundFiles, *BindingsDir, TEXT(".umap"));
//			for (auto TmpFile : FoundFiles)
//			{
//				LevelFilesToLoad.Add(LevelPath + "/" + TmpFile.Left(TmpFile.Len() - 5), false);
//			}
//		}
//	}

	TArray<FString> LevelFolderPathsList;
	for (int32 i = 0; i < MC.LevelStreamingInfo.Num(); ++i)
	{
		FString LevelPath = MC.LevelStreamingInfo[i].LevelPath;
		LevelFolderPathsList.AddUnique(LevelPath);
	}

	UWorldComposition* WC = GetWorld()->WorldComposition;
	if (WC)
	{
		for (auto LevelStreaming : WC->TilesStreaming)
		{
			FString LevelName = LevelStreaming->PackageNameToLoad.ToString();
			if (IsExistInMapConfig(LevelName, LevelFolderPathsList))
			{
				LevelFilesToLoad.Add(LevelName, false);
			}
		}
	}

	if (LevelFilesToLoad.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPWLevelMgr::GetLevelFilesFromDT() failed  LVELFILES IS NULL"));
	}
	else
	{
		bGetDTOnce = true;
	}
	return LevelFilesToLoad;
}

bool UPWLevelMgr::IsLevelFileInMapConfig(const FString FilePathName, TArray<FString> PathsList)
{
	for (auto LevelPath : PathsList)
	{
		if (LevelPath.Contains(FilePathName, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;

}


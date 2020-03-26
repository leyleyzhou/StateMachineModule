#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Classes/Engine/EngineTypes.h"
#include "PWLevelMgr.generated.h"

//namespace PWLevelLayer {
//	extern const FString String_NoStreaming;
//	extern const FString String_LD;
//	extern const FString String_Props;
//	extern const FString String_House;
//	extern const FString String_Land;
//	extern const int32 DefaultStreamingDistance;
//	extern const int32 HouseStreamingDistance;
//	extern const int32 MaxStreamingDistance;
//}

USTRUCT()
struct FLevelStat
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		FName LevelName;

	UPROPERTY()
		float LoadPercentage = 0.f;

	UPROPERTY()
		class ULevelStreaming* LevelRef = nullptr;

	FLevelStat()
	{
	}
};

UENUM(BlueprintType)
enum class EPWStreamingType : uint8
{
	PWST_OnebyOne,
};

class APlayerController;
/**
 *
 */
UCLASS()
class PWGAME_API UPWLevelMgr : public UObject
{
	GENERATED_BODY()
public:
	UPWLevelMgr(const FObjectInitializer& ObjectIntializer = FObjectInitializer::Get());

	static UPWLevelMgr* Get(UObject* ContextObject);

	//It's not to handle when all of the maps are loaded,just main map
	//void HandlePostLoadMap(UWorld* LoadedWorld);
	void BeginLoadSubLevels();

	void ServerLoadMapAccordingByConfig(bool bToggleStreaming = true);
	bool IsExistInMapConfig(const FString PackageName, TArray<FString> PathsList);

	void ToggleAllLevelStreaming(bool bToggleStreaming);
	void SetLevelStreamingInfoOnMapLoaded();
	void SetLevelStreamingInfoOnThePlane();
	void SetLevelStreamingInfoOnCharacterPrepareLanding();
	void SetLevelStreamingInfoOnCharacterLanding();

public:
	//HLOD settings
	void HLODSetting_PerpareSkyDiving(class APlayerController* PC);
	void HLODSetting_PerpareLanding(class APlayerController* PC);
	void SetHLODDistanceOverride(class APlayerController* PC, float HLODDistance);
	void ClientLevelStreamingPause(int32 bPause);
	void ClientLandScapeLOD(int32 Lod);

	void SetTilesStreamingTimeThreshold(float TimeThreshold);
	void SetLevelStreamingInfoByLevelPath(const FString& LevelPath, bool bToggleStreaming, const int32 distanceForStreaming = -1);

	//void SetGCPauseForAWhile(float PausedSec = 5.f);
//private:
	//UFUNCTION()
	//void DelaySetGCPauseExc();

private:

	FTimerHandle ServerLoadCheckTH;
	FTimerHandle ClientLoadCheckTH;
	void ServerAllLoadedTimer();
	void ClientAllLoadedTimer();

	void GetLevelsByPredicate(FString Predicate, TArray<FName>& outlevels);
	void GetLevelsByPredicate(FString Predicate, TArray<ULevelStreaming*>& outlevels);

private:
	void LoadSubLevelsByConfig();
	void FinishLoadSubLevels();

	void TryToLoad();
	void ClientDoLoad();
	void ServerDoLoad();

	//TODO: @zhouminyi many version of these checking functions.
	// We should refactor this ASAP.
	bool HasAllSubLevelsLoaded_Client(bool OnlyCheckAlwaysLoadedSubLevel = false);
	bool HasAllSubLevelsLoaded_Server();

	bool IsServerAllLoaded();
	bool ClientAllLoaded();
	bool IsInFilterList();

	// We can set some console variables to restrict level streaming's payload every frame.
	// e.g. SetConsoleVariable(TEXT("s.LevelStreamingActorsUpdateTimeLimit"), 20.0f);
	// This command limit actors update time per frame.
	void RestrictLevelStreamingFramePayload();

	void UnrestrictLevelStreamingFramePayload();


private:

	// Exclude those sub levels in sever blacklist for optimization.
	bool IsSubLevelInServerLoadingBlacklist(const FName& LevelName);

private:

	float TotalLoadTime = 0.f;
	int32 LoadUID = 21000;

	TMap<FString, bool> LevelFilesToLoad;
	/**Get levels from datatable.*/
	TMap<FString, bool> GetLevelFilesFromDT();
	bool bGetDTOnce;
	bool IsLevelFileInMapConfig(const FString FilePathName, TArray<FString> PathsList);

};


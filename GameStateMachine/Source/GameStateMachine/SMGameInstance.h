//Copyright 2004 - 2017 Perfect World Co., Ltd.All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Runtime/Engine/Classes/Engine/StreamableManager.h"
#include "PWFXDataStructs.h"

#include "NetworkReplayStreaming.h"
#include "PWReplayInfo.h"

#include "PWLogManager.h"
#include "DHCharacterDataStructs.h"
#include "DHCommon.h"

#include "UIWidgetHead.h"
#if !PLATFORM_LINUX
#include "VivoxCore.h"
#endif
#include "PWVivoxManager.h"
#include "PWGameInstance.generated.h"

#define PW_START_CLIENT_CODE if (GetNetMode() != NM_DedicatedServer){
#define PW_END_CLIENT_CODE }

DECLARE_LOG_CATEGORY_EXTERN(LogPWGameInstance, Log, All);
class UPWFXManager;
class UPWDelegateManager;
class UPWCharacterManager;

class UPWUIManager;
class UPWDropManager;
class UPWRouteManager;
class UPWAirDropManager;
class UPWStandaloneAirDropManager;
class UPWLevelMgr;
class UPWStorageManager;
class UPWShopManager;
class UPWPromptManager;
class UPWOperationNoticeManager;
class UPWSafeCircleManager;
class UPWStandaloneSafeCircleManager;
class UPWAssetManager;
class UPWCheatManager;
class UPWLimousineDataManager;
class UPWSensitivityManager;
class UPWSaveGameManager;
//class UPWGMEMgr;
class UPWLevelMgr;
class FPWVoicePlatform;
class UPWPreloadCacheManager;
class UPWHornManager;
class UPWReplicationGraph;
class UPWDownloadImageManager;
class UPWPromptBase;
class UPWLogManager;
class UPWEnvTempManager;
class UPWSkyLightManager;
class UPWStreamingLevelManager;
class UPWClientNet;
class UPWDynamicActorManager;
class UPWMarkerManager;
class UPWCharacterSkillManager;
class UPWLobbyDataManager;

USTRUCT(BlueprintType)
struct FPWCharacterTypeData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CharacterTypeData")
		ECharacterType CharacterType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CharacterTypeData")
		TSubclassOf<APawn>		PawnClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CharacterTypeData")
		TSubclassOf<AController> ControllerClass;
};

UENUM(BlueprintType)
enum class ENetReplayStreamingModule : uint8
{
	SM_NULL		UMETA(DisplayName = "NULL Streamer"),
	SM_Memory	UMETA(DisplayName = "Memory Streamer"),
	SM_HTTP		UMETA(DisplayName = "HTTP Streamer"),
};

UCLASS()
class PWGAME_API UPWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPWGameInstance();

	static UPWGameInstance* Get(const UObject* ContextObject);


	virtual void PostInitProperties() override;

public:
	/** Starts the GameInstance state machine running */
	virtual void StartGameInstance() override;
#if WITH_EDITOR
	/* Called to actually start the game when doing Play/Simulate In Editor */
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;
#endif
	/** Called from InitializeStandalone or InitializeForPlayInEditor */
	virtual void Init() override;
	virtual void Shutdown() override;

	/** Call to preload any content before loading a map URL, used during seamless travel as well as map loading */
	virtual void PreloadContentForURL(FURL InURL) override;

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor);


public:
	/*Called before GameRestart*/
	void OnGameRestart();
public:
	FStreamableManager    StreamMgr;

public:


	UPWReplicationGraph* GetReplicationGraph();
	UPROPERTY()
		class UPWReplicationGraph* RepGraphInst;
	//ReplicationGraph dedicate server Interface
	void InitReplicationGraph();
	void FlushNetDormantActor(class AActor* actor) const;
	void ForceNetUpdateActor(class AActor* actor) const;
	void AddDependentActor(class AActor* OwnerActor, class AActor* DependentActor);
	//ReplicationGraph Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Manager")
		UPWDelegateManager* GetDelegateManager();

	UFUNCTION(BlueprintCallable, Category = "Manager")
		UPWCharacterManager* GetCharacterManager();

	UFUNCTION(BlueprintCallable, Category = "Manager")
		UPWUIManager* GetUIManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWLobbyUIManager* GetLobbyUIManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWDropManager* GetDropManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWRouteManager* GetRouteManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWAirDropManager* GetAirDropManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWStandaloneAirDropManager* GetStandaloneAirDropManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWLimousineDataManager* GetLimousineDataManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWStorageManager* GetStorageManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWLobbyDataManager* GetLobbyDataManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWShopNetManagerV2* GetShopNetManagerV2();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWShopManagerV2* GetShopManagerV2();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWInventoryNetManagerV2* GetInventoryNetManagerV2();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWInventoryManagerV2* GetInventoryManagerV2();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWLobbyBaseManager* GetLobbyBaseManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWShopManager* GetShopManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWPromptManager* GetPromptManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWOperationNoticeManager* GetOperationNoticeManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWAssetManager* GetAssetManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWSafeCircleManager* GetSafeCircleManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWStandaloneSafeCircleManager* GetStandaloneSafeCircleManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWDoubleCircleManager* GetDoubleCircleManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWCheatManager* GetCheatManager();

	UPWSensitivityManager* GetSensitivityManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		UPWSaveGameManager* GetSaveGameManager();

	//UFUNCTION(BlueprintCallable, Category = "Manager")
	//	FPWVoicePlatform* GetVoiceManager();

	UFUNCTION(BlueprintCallable, Category = "Manager")
		UPWLevelMgr* GetLevelManager();

	UFUNCTION(BlueprintCallable, Category = "Manager")
		class UPWDataStatisticsManager* GetDataStatisticsManager();

	UFUNCTION(BlueprintCallable, Category = "Manager")
		class UPWExtraEffectComponent* GetExtraEffectManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWSkillManager* GetSkillManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWMailManager* GetMailManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWActorTileManager* GetActorTileManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWPreloadCacheManager* GetPreloadCacheManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWHornManager* GetHornManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWWWiseManager* GetWWiseMananger();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWDownloadImageManager* GetDownloadImageManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWLogManager* GetLogManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWEnvTempManager* GetEnvTempManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWSkyLightManager* GetSkyLightManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UDHPlayerStartManager* GetPlayerStartManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UDHEvacuateManager* GetEvacuateManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWAirForteManager* GetAirforteManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWStreamingLevelManager* GetStreamingLevelManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWDynamicActorManager* GetDynamicActorManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWMarkerManager* GetMarkerManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWCharacterSkillManager* GetCharacterSkillManager();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWTrophyMananger* GetTrophyMananger();

	UFUNCTION(BlueprintPure, Category = "Manager")
		class UPWVivoxManager* GetVivoxManager();

public:
	template <typename T>
	int32 GetInstanceID()
	{
		static int32 InstID = GetNextID();

		return InstID;
	}

	/*
	 * 获取隶属GameInstance的单例
	 */
	template <
		typename T
	>
		T* GetInstance(bool AutoCreate = true)
	{
		int32 InstID = GetInstanceID<T>();

		if (_Instances.Num() <= InstID) {
			_Instances.SetNum(((InstID / 5) + 1) * 5);
		}

		UObject* Value = _Instances[InstID];
		if (!Value && AutoCreate) {
			Value = NewObject<T>(this);
			_Instances[InstID] = Value;
		}

		return static_cast<T*>(Value);
	}

	void RemoveInstanceByID(int32 InstID)
	{
		return_if_true(InstID < 0 || InstID >= _Instances.Num());

		_Instances[InstID] = nullptr;
	}
private:
	int32 GetNextID()
	{
		static volatile int32 Indexer = -1;

		return FPlatformAtomics::InterlockedIncrement(&Indexer);
	}

	UPROPERTY()
		TArray<UObject*> _Instances;
private:
	UPROPERTY()
		UPWDelegateManager* DelegateManager;

	UPROPERTY()
		UPWCharacterManager* CharacterManager;

	UPROPERTY()
		UPWUIManager* UIWidgetManager;

	UPROPERTY()
		UPWLobbyUIManager* LobbyUIWidgetManager;

	UPROPERTY()
		UPWDropManager* DropManager;

	UPROPERTY()
		UPWRouteManager* RouteManager;

	UPROPERTY()
		UPWAirDropManager* AirDropManager;

	UPROPERTY()
		UPWStandaloneAirDropManager* StandaloneAirDropManager;

	UPROPERTY()
		UPWLimousineDataManager* LimousineDataManager;

	UPROPERTY()
		UPWStorageManager* StorageManager;

	UPROPERTY()
		UPWShopManager* ShopManager;

	UPROPERTY()
		UPWLobbyDataManager* LobbyDataManager;
	UPROPERTY()
		class UPWShopNetManagerV2* ShopNetManagerV2;
	UPROPERTY()
		class UPWShopManagerV2* ShopManagerV2;

	UPROPERTY()
		class UPWInventoryNetManagerV2* InventoryNetManagerV2;
	UPROPERTY()
		class UPWInventoryManagerV2* InventoryManagerV2;

	UPROPERTY()
		class UPWLobbyBaseManager* LobbyBaseManager;

	UPROPERTY()
		UPWPromptManager* PromptManager;

	UPROPERTY()
		UPWOperationNoticeManager* OperationNoticeManager;

	UPROPERTY()
		UPWAssetManager* AssetManager;

	UPROPERTY()
		UPWSafeCircleManager* SafeCircleManager;

	UPROPERTY()
		UPWStandaloneSafeCircleManager* StandaloneSafeCircleManager;

	UPROPERTY()
		UPWDoubleCircleManager* DoubleCircleManager;

	UPROPERTY()
		UPWCheatManager* PWCheatManager;

	UPROPERTY()
		UPWSensitivityManager* SensitivityManager;

	UPROPERTY()
		UPWSaveGameManager* SaveGameManager;

	UPROPERTY()
		UPWVivoxManager* VivoxManager = nullptr;

	//UPROPERTY()
	//	UPWGMEMgr* GMEManager;
	UPROPERTY()
		UPWLevelMgr* LevleMgr;

	UPROPERTY()
		class UPWDataStatisticsManager* PWDataStatisticsManager;

	UPROPERTY()
		class UPWSkillManager* PWSkillManager;

	UPROPERTY()
		class UPWExtraEffectComponent* ExtraEffectManager;

	UPROPERTY()
		class UPWSkillManager* SkillManager;
	UPROPERTY()
		class UPWActorTileManager* ActorTileManager;

	UPROPERTY()
		class UPWMailManager* MailManager;

	UPROPERTY()
		class UPWPreloadCacheManager* PreloadCacheManager = nullptr;

	UPROPERTY()
		class UPWHornManager* HornManager = nullptr;

	UPROPERTY()
		class UPWWWiseManager* WWiseManager = nullptr;

	// begin replay
	UPROPERTY()
		class UPWDownloadImageManager* DownloadImageManager = nullptr;

	UPROPERTY()
		class UPWLogManager* LogManager = nullptr;

	UPROPERTY()
		class UPWEnvTempManager* EnvTempManager = nullptr;

	UPROPERTY()
		class UPWSkyLightManager* SkyLightManager = nullptr;

	UPROPERTY()
		class UDHPlayerStartManager* PlayerStartManager = nullptr;

	UPROPERTY()
		class UDHEvacuateManager* EvacuateManager = nullptr;

	UPROPERTY()
		class UPWAirForteManager* AirforteManager = nullptr;

	UPROPERTY()
		class UPWStreamingLevelManager* StreamingLevelManager = nullptr;

	UPROPERTY()
		class UPWDynamicActorManager* DynamicActorManager = nullptr;

	UPROPERTY()
		class UPWMarkerManager* MarkerManager = nullptr;
	UPROPERTY()
		class UPWCharacterSkillManager* CharacterSkillManager = nullptr;
	UPROPERTY()
		class UPWTrophyMananger* TrophyManager = nullptr;

public:

	/** Start recording a replay from blueprint. ReplayName = Name of file on disk, FriendlyName = Name of replay in UI */
	UFUNCTION(BlueprintCallable, Category = "Replays")
		void BP_StartRecordingReplay(FString ReplayName, FString FriendlyName, ENetReplayStreamingModule StreamerModule = ENetReplayStreamingModule::SM_NULL, float TimeBufferHintSeconds = -1);

	/** Start recording a running replay and save it, from blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Replays")
		void BP_StopRecordingReplay();

	/** Start playback for a previously recorded Replay, from blueprint */
	UFUNCTION(BlueprintCallable, Category = "Replays")
		void BP_PlayReplay(FString ReplayName, ENetReplayStreamingModule StreamerModule = ENetReplayStreamingModule::SM_NULL);

	/** Start looking for/finding replays on the hard drive */
	UFUNCTION(BlueprintCallable, Category = "Replays")
		void FindReplays();

	/** Apply a new custom name to the replay (for UI only) */
	UFUNCTION(BlueprintCallable, Category = "Replays")
		void RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName);

	/** Delete a previously recorded replay */
	UFUNCTION(BlueprintCallable, Category = "Replays")
		void DeleteReplay(const FString& ReplayName);

	virtual bool PlayReplay(const FString& InName, UWorld* WorldOverride = nullptr, const TArray<FString>& AdditionalOptions = TArray<FString>()) override;

	virtual void StartRecordingReplay(const FString& Name, const FString& FriendlyName, const TArray<FString>& AdditionalOptions) override;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Replays")
		void BP_OnFindReplaysComplete(const TArray<FPWReplayInfo>& AllReplays);

public:
	void HandlePreLoadMap(const FString& MapName);

	void HandlePostLoadMap(UWorld* World);

	void HandlePostLoadAllMap();

private:

	// for FindReplays() 
	TSharedPtr<INetworkReplayStreamer> NullStreamsPtr;
	TSharedPtr<INetworkReplayStreamer> InMemoryStreamerPtr;

	FEnumerateStreamsCallback OnEnumerateStreamsCompleteDelegate;

	void OnEnumerateStreamsComplete(const FEnumerateStreamsResult& EnumerateStreamsResult);

	// for DeleteReplays(..)
	FDeleteFinishedStreamCallback OnDeleteFinishedStreamCompleteDelegate;

	void OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& DeleteFinishedStreamResult);

	// AG_ET - Handle Controller connection events
	void OnControllerConnectionChanged(bool bIsConnected, int32 UserID, int32 ControllerId);
	UFUNCTION()
		void OnControllerPromptClosed(bool bOkPressed);
	UPROPERTY()
		UPWPromptBase* ControllerMessagePrompt = nullptr;

public:

	// in order to handle demo redirects, we have to cancel the demo, download, then retry
	// this tracks the demo we need to retry
	FString LastTriedDemo;
	bool bRetriedDemoAfterRedirects;

	inline void SetLastTriedDemo(const FString& NewName)
	{
		if (NewName != LastTriedDemo)
		{
			LastTriedDemo = NewName;
			bRetriedDemoAfterRedirects = false;
		}
	}
	inline FString GetLastTriedDemo() const
	{
		return LastTriedDemo;
	}

	bool IsKillcamWorld(UWorld* QueryWorld) const;

	// end replay

	//Map
public:
	void SetMapID(int32 MapId) { BattleMapID = MapId; }
	int32 GetMapID() { return BattleMapID; }

	void SetBattleStartTime(const int64& inTimeStamp) { BattleStartTimeStamp = inTimeStamp; }
	int64 GetBattleStartTime() { return BattleStartTimeStamp; }

	FString GetLobbyMapName();
	FString GetLobbyMapNameWithExtension();
protected:
	int32 BattleMapID = 0;
	int64 BattleStartTimeStamp = 0;

public:
	void ShowLoadingScreen(bool IsContentVisible = true);
	void UpdateLoadingScreenContentVisibility(bool inIsVisible);
	void HideLoadingScreen();

	//added by wangshu
	void ShowNewLoadingScreen(EUMGLoadingSceneType type);
	void HideNewLoadingScreen();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LoadingScreen")
		TSubclassOf<class UPWNewLoadingWidget> NewLoadingScreen;
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LoadingScreen")
		UClass* LoadingScreen;

	UPROPERTY()
		class UPWLoadingWidget* LoadingScreenWidget;

public:
	UClass* GetPawnClassByCharacterType(ECharacterType CharacterType);
	UClass* GetControllerClassByCharacterType(ECharacterType CharacterType);
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CharacterTypeData")
		TArray<FPWCharacterTypeData> CharacterTypeDataList;

};

template <
	typename T
>
class DHGameInstance
{
public:
	DEPRECATED(4.21, "Please override GetInstance instead")
		FORCEINLINE static T* Get(UObject* Context)
	{
		return GetInstance(Context);
	}

	FORCEINLINE static T* GetInstance(UObject* Context)
	{
		auto GameInstance = UPWGameInstance::Get(Context);
		if (GameInstance)
		{
			return GameInstance->GetInstance<T>();
		}
		return nullptr;
	}

	FORCEINLINE static void RemoveInstance(UObject* Context)
	{
		auto GameInstance = UPWGameInstance::Get(Context);
		if (GameInstance)
		{
			int32 InstID = GameInstance->GetInstanceID<T>();
			GameInstance->RemoveInstanceByID(InstID);
		}
	}
};
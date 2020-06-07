/**
* Copyright 2004-2017 Perfect World Co.,Ltd. All Rights Reserved.
* Created by ZP on 5.23.2017
*/

#pragma once
#include "CoreMinimal.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"
#include "Runtime/Engine/Classes/Engine/AssetManager.h" 
#include "PWGameInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PWInventoryDataStructs.h"
#include "UserWidget.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "ClientAroundInterface.h"
#include "PWMapConfig.h"
#include "PWPickup.h"
#include "SizeBox.h"
#include "JsonObjectConverter.h"
#include "JsonObject.h"
#include "JsonReader.h"
#include "JsonValue.h"
#include "JsonSerializer.h"
#include "PWGameDataConfig.h"
#include "PWUIStyleConfig.h"
#include "DHCharacterDataStructs.h"
#include "WWise/PWWWiseManager.h"
#include "PWCharacterData.h"
#include "SaveGame/PWSaveGameDefine.h"
#include "PWNetSlimData.h"
#include "PWLibrary.generated.h"


//~=============================================================================
//  MWLibrary defines the common function 
//  These are runtime functions Do not use in constructor
//~=============================================================================
enum class EInteractions : uint8;

UENUM()
enum class EPWSelectedDirection : uint8
{
	Left,
	Up,
	Right,
	Down
};

UENUM()
enum class EPWAboutLRButtonType : uint8
{
	L1,
	R1,
	L2,
	R2
};

UCLASS()
class PWGAME_API UPWBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
		static float GetMaxAutoPickupAmmoAmount();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
		static float GetMaxAutoPickupMedicalItemAmount();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
	static float GetNormalDistribution(FRandomStream& RS, float mean = 0.f, float Sigma = 1.f);
	
	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
	static float GetNormalDistribution_Test(float mean = 0.f, float Sigma = 1.f);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
	static void GetWeaponIconName(int32 WeaponId, class UTexture2D* &WeaponIcon, FText &WeaponName);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
	static void GetDeathIconName(uint8 DeathType, class UTexture2D* &WeaponIcon, FText &WeaponName);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
	static void GetDamageIconName(uint8 DamageType, class UTexture2D* &WeaponIcon, FText &WeaponName);

	/** Get float value by given variable name, return false if not found */
	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Variables")
	static bool GetFloatByName(UObject* Target, FName VarName, float &OutFloat);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Variables")
	static bool GetIntByName(UObject* Target, FName VarName, int32 &OutInt);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Variables")
	static bool SetFloatByName(UObject* Target, FName VarName, float NewValue, float &OutFloat);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Variables")
	static bool SetIntByName(UObject* Target, FName VarName, int32 NewValue, int32 &OutInt);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Variables")
	static bool IncreaseFloatByName(UObject* Target, FName VarName, float ValueToAdd, float &OutFloat);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Variables")
	static bool IncreaseIntByName(UObject* Target, FName VarName, int32 ValueToAdd, int32 &OutInt);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Debug")
	static class APWDebugActor* GetPWDebugActor(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Debug")
	static void RegisterNewDebugTarget(UObject* TargetObj, TArray<FName> VariableNames);

	// 打印FPS
	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Debug")
	static void LogPerformance();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWDelegateManager* GetDelegateManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWUIManager* GetUIManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWOperationNoticeManager* GetOperationNoticeManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWProcedureManager* GetProcedureManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWRouteManager* GetRouteManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWPromptManager* GetPromptManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWStorageManager* GetStorageManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWSaveGameManager* GetSaveGameManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWMailManager* GetMailManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWShopManager* GetShopManager(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Manager", meta = (WorldContext = "ContextObject"))
		static UPWTrophyMananger* GetTrophyMananger(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObject"))
	static APWCharacterBase* GetLocalPWCharacter(const UObject* ContextObject);

    UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObject"))
        static APWPlayerControllerBase* GetLocalPWController(const UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObject"))
		static APWCharacterData_Runtime* GetLocalRuntimeData(const UObject* ContextObject);

    UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObject"))
        static APWCharacterData_Runtime* GetViewedRuntimeData(const UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObject"))
		static UPWInventoryComponent* GetLocalInventory(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Replays")
	static bool IsKillcamWorldObject(const UObject* ContextObj);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary")
		static int32 GetStringLengthUTF8(FString str);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Item")
		static FPWItem GetItemDataByID(int32 ItemID);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Item")
		static FPWStorageItem GetStorageItemDataByID(int32 ItemID);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObject"))
		static UPWSkillManager* GetSkillManager(UObject* ContextObj);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObject"))
		static UPWLogManager* GetLogManager(UObject* ContextObj);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client")
		static bool IsPlayWithGamePad();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client")
		static bool IsPlayWithSteam();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Client")
		static bool IsPlayWithPS4();

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Client")
		static void PWLIBDrawRealLineCircle(UPARAM(ref) FPaintContext& Context, const FVector2D& Center, float Radius, UCurveFloat* SmoothCurve, FLinearColor Tint, bool bAntiAlias);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Client")
		static void PWLIBDrawDottedLineCircle(UPARAM(ref) FPaintContext& Context, const FVector2D& Center, float Radius, UCurveFloat* RealLineLenRadiusCurve, UCurveFloat* EmptyLineLenRadiusCurve, FLinearColor Tint, bool bAntiAlias);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Client")
		static void PWLIBDrawDottedLine(UPARAM(ref) FPaintContext& Context, const FVector2D& From, const FVector2D& To, float RealLineLen, float EmptyLineLen, const FColor& Color, bool bAntiAlias);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Client")
		static float GetHealthBuffMaxTime();

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Client")
		static float GetSpeedBuffMaxTime();

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Client", meta = (WorldContext = "ContextObj"))
		static void UpdateBattleUI(UObject* ContextObj);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Map")
		static int32 GetMapID(UObject* ContextObj);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Map")
		static EMapType GetMapType(UObject* ContextObj);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Map", meta = (WorldContext = "ContextObj"))
		static bool GetMapConfig(UObject* ContextObj, FPWMapConfig& MapConfig);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Map", meta = (WorldContext = "ContextObj"))
		static EMapSeason GetSeasonType(UObject* ContextObj);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Map", meta = (WorldContext = "ContextObj"))
		static float GetBornLocationShowTime(UObject* ContextObj);

	// @Param EndingTime (second)
	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Item", meta = (WorldContext = "ContextObject"))
		static void GetTimeRemain(UObject* ContextObject, int EndingTime, bool& bIsExpire, int& Day, int& Hour, int& Minute);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Item", meta = (WorldContext = "ContextObject"))
		static float GetFightingDeltaTime(UObject* ContextObject);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Item")
		static bool IsArmor(int32 ItemID);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | Item")
		static float GetArmorDurabilityPercentage(int32 ItemID, APWPickup* PickupActor);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Browser", meta = (WorldContext = "ContextObject"))
		static void OpenUserLink(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Browser", meta = (WorldContext = "ContextObject"))
		static void OpenPrivateLink(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | UI", meta = (WorldContext = "ContextObject"))
		static void ShowBornOperationUI(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | UI", meta = (WorldContext = "ContextObject"))
		static void ShowGamePadOperationUI(UObject* ContextObject);

    UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | UI", meta = (WorldContext = "ContextObject"))
        static void ShowReleaseNotesUI(UObject* ContextObject);

	UFUNCTION()
		static FString GetCultureName();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | PS4")
		static bool IsEnterCrossNotCircle(UObject* ContextObj);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | StandaloneCopy", meta = (WorldContext = "ContextObj"))
		static void ShowStandaloneNotice(UObject* ContextObj, int32 ContentID);

	//根据安全区域的 padding 算出 sizebox 具体能放多少行
	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObj", DeprecatedFunction))
		static int32 GetRowCountPerSizeBox(UObject* ContextObj, USizeBox* sizeBox, float rowHeight = 0.f);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | StandaloneCopy", meta = (WorldContext = "ContextObject"))
		static void PlayUISound(UObject* ContextObject, USoundBase* NormalSound, UAkAudioEvent* WwiseSound);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget")
		static int32 GetHighestWidgetOrder();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary |GameConfig")
		static FPWGameDataConfig GetGameDataConfig();

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary|Grenade")
		static bool GetGrenadeTypeFromID(int32 ItemID, EGrenadeType& Type);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary|Item")
		static EPWItemCategory GetItemCategory(int32 ItemID);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary|Item")
		static int32 GetItemZOrder(int32 ItemID);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary|Item")
		static bool GetUIStyle(FPWUIStyleConfig& OutConfig);

	UFUNCTION(BlueprintPure, Category = "PWBlueprintLibrary | AirLine", meta = (WorldContext = "ContextObject"))
		static TArray<FVector> GetAirlineData(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static void SetWWiseSoundVolme(UObject* ContextObject, const EWWiseSonundVolumeType SoundType, const float Volume);
	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
	static TArray<FCharacterSlimInfor> GetCharacterSlimInfor(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static void PlayMovePageSound(UObject* ContextObject,int32 ChangedVal);
	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static void PlayClosePageSound(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static void PlayConfirmSound(UObject* ContextObject);

	//------------ old Lobbywidget used
	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static void PlaySlideLeftRightSound(UObject* ContextObject,bool IsLeft);

	//------------ old Lobbywidget used
	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static void PlaySlideUpDownSound(UObject* ContextObject, bool IsUp);

	UFUNCTION(BlueprintCallable, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static void PlaySoundByUIAudioType(UObject* ContextObject, EWwiseUIAudioType type);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
		static FPWNetSlimInfor GetNetInfor(UObject* ContextObject);

	//------------ for new Lobbywidget  
	static void PlaySlideSelectedSound(UObject* ContextObject, EPWSelectedDirection InDirection);

	static void PlaySoundAboutLRPressed(UObject* ContextObject, EPWAboutLRButtonType Type);

	static void PlaySoundWhenActionUpPressed(UObject* ContextObject);

	static void PlaySoundWhenActionLeftPressed(UObject* ContextObject);
	//------------for new Lobbywidget end

	static FLinearColor GetQualityLinearColor(EItemQuality Quality);
	/**Get Interact top tips.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PWBlueprintLibrary | Widget", meta = (WorldContext = "ContextObject"))
	static FText GetInteractTopTip(UObject* ContextObject, const EInteractions Action);

	/** whether show widget type in tabpage,only use for bool variable.*/
	static bool IsShowWidgetByType(UObject* ContextObject, const EPWSaveTabPageType TabPageType, const EPWSGType Type);

	static int32 GetAmmoItemIDByType(EAmmoType Type);
};

namespace PWLIB
{

	/**
	* getGameInstance
	*
	*/
	UFUNCTION(BlueprintCallable, Category = "MWLIB")
		PWGAME_API UPWGameInstance* GetGameInst();

	PWGAME_API UWorld* GetWorld();

	FString GetCharacterTableNameByCharacterType(EPWPlayerCareerType type);
	///**
	//* GetJsonTableInst
	//*
	//*/
	//PWGAME_API UMWJsonTable* GetJsonTableInst();

	/**
	* Gets Specific GameMode
	* returns the current GameMode casted to the desired type. * /
	*/
	template<class T>
	T* GetGameModeSpecific()
	{
		if (GWorld == nullptr)
		{
			return nullptr;
		}

		return Cast<T>(GWorld->GetAuthGameMode());
	}

	/**
	* Gets Specific GameState
	* returns the current GameState casted to the desired type. * /
	*/
	template<class T>
	T* GetGameState()
	{
		return GWorld ? Cast<T>(GWorld->GetGameState()) : nullptr;
	}

	/**
	* Load BP Dynamically
	*
	*
	* @param filePath - Path from content. e.g  FString  filePath = FPaths::GameContentDir() + TEXT("Json/AppConfig.json");
	* @param outArray - out data .
	*/
	PWGAME_API UClass* LoadBPFromPathName(FString PathName);
	PWGAME_API UClass* LoadBPFromPathName(FStringAssetReference StringAssetRef);
	PWGAME_API void  UnLoadBPFromPathName(FString PathName);
	PWGAME_API void  UnLoadBPFromPathName(FStringAssetReference StringAssetRef);

	PWGAME_API UClass* FindClassOfName(FString ClassName, FString PathToLoadIfNotFind = "");

	template<typename AssetType>
	AssetType* SynchronouslyLoadAsset(FSoftObjectPath StringReference)
	{
		FString TmpAssetName = StringReference.GetLongPackageName();
		const int32 MinPathLength = 5;
		return_null_if_true(TmpAssetName.IsEmpty() || TmpAssetName.Len() <= MinPathLength);
		bool bIsExist = FPackageName::DoesPackageExist(TmpAssetName) || FPackageName::IsScriptPackage(TmpAssetName);
#if WITH_EDITOR
		if (!bIsExist)
		{
			UE_LOG(LogTemp, Error, TEXT("SynchronouslyLoadAsset:: file %s does not exist "), *TmpAssetName);
		}
#endif
		if (StringReference.IsValid() && bIsExist)
		{
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			AssetType* LoadedAsset = Cast<AssetType>(Streamable.LoadSynchronous(StringReference));
			if (ensureMsgf(LoadedAsset, TEXT("Unable to load asset %s"), *StringReference.ToString()))
			{
				return LoadedAsset;
			}
		}
		return NULL;
	}

	template<typename AssetType>
	AssetType* SynchronouslyLoadAsset(FString AssetStringPath)
	{
		FStringAssetReference StringReference;
		StringReference.SetPath(AssetStringPath);

		return SynchronouslyLoadAsset<AssetType>(StringReference);
	}
	/** A helper function to load a TAssetPtr */
	template<typename AssetType>
	AssetType* SynchronouslyLoadAsset(TAssetPtr<AssetType>& AssetPointer)
	{
		const FStringAssetReference& StringReference = AssetPointer.ToSoftObjectPath();
		return SynchronouslyLoadAsset<AssetType>(StringReference);
	}


	/**
	* Load Asset Synchronized
	*
	*
	* @param AssetPath - path.
	*/
	template<typename T>
	T* LoadAssetSync(FStringAssetReference AssetPath)
	{
		T* ret = nullptr;

		ret = Cast<T>(GetGameInst()->StreamMgr.LoadSynchronous(AssetPath, true));

		return ret;
	}

    template<typename AssetType>
    TSharedPtr<FStreamableHandle> AsynchronouslyLoadAsset(FSoftObjectPath StringReference)
    {
        FString TmpAssetName = StringReference.GetLongPackageName();
        const int32 MinPathLength = 5;
        return_null_if_true(TmpAssetName.IsEmpty() || TmpAssetName.Len() <= MinPathLength);
        bool bIsExist = FPackageName::DoesPackageExist(TmpAssetName) || FPackageName::IsScriptPackage(TmpAssetName);
#if WITH_EDITOR
        if (!bIsExist)
        {
            UE_LOG(LogTemp, Error, TEXT("AsynchronouslyLoadAsset:: file %s does not exist "), *TmpAssetName);
        }
#endif
        if (StringReference.IsValid() && bIsExist)
        {
            FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
            return Streamable.RequestAsyncLoad(StringReference);
        }
        return nullptr;
    }

	/**
	* Load Asset Simple Asynchronous
	*
	*
	* @param AssetPath - path.
	*/
	PWGAME_API void LoadAssetSimpleAsync(FStringAssetReference AssetPath);

	PWGAME_API void UnLoadAssetFromPath(FStringAssetReference AssetRef);

	/**
	* Load Single Asset Asynchronous
	*
	*
	* @param AssetPath - Path.
	*/
	PWGAME_API void LoadAssetASync(FStringAssetReference AssetRef, FStreamableDelegate Del);
	PWGAME_API void LoadAssetASync(FString AssetPath, FStreamableDelegate Del);

	/**
	* Load  Asset Array Asynchronous
	*
	*
	* @param AssetPath - PathArray.
	*/
	PWGAME_API void LoadAssetArrayASync(TArray<FStringAssetReference> AssetArr, FStreamableDelegate Del);


	// 读/写变量相关 begin

	static bool GetFloatByName(UObject* Target, FName VarName, float &OutFloat);

	static bool GetIntByName(UObject* Target, FName VarName, int32 &OutInt);

	static bool SetFloatByName(UObject* Target, FName VarName, float NewValue, float &OutFloat);

	static bool SetIntByName(UObject* Target, FName VarName, int32 NewValue, int32 &OutInt);

	static bool IncreaseFloatByName(UObject* Target, FName VarName, float ValueToAdd, float &OutFloat);

	static bool IncreaseIntByName(UObject* Target, FName VarName, int32 ValueToAdd, int32 &OutInt);

	PWGAME_API void SetAccountArea(const FString &Area);


	// 读写变量 end


		/**
		* Parse single JSON file.
		*
		*
		* @param filePath - Path from content. e.g  FString  filePath = FPaths::GameContentDir() + TEXT("Json/AppConfig.json");
		* @param outArray - out data .
		*/
	template<typename StructName>
	void ParseJsonArray2Array(FString filePath, TArray<StructName>* outArray)
	{
		if (filePath.IsEmpty())
			return;

		FString FileContents;
		if (!FFileHelper::LoadFileToString(FileContents, *filePath))
		{
			return;
		}

		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);
		TSharedPtr<FJsonValue> pJsonValue;

		if (FJsonSerializer::Deserialize(Reader, pJsonValue) && pJsonValue.IsValid())
		{


			TArray<TSharedPtr<FJsonValue>> arr = pJsonValue->AsArray();

			for (TSharedPtr<FJsonValue> JsonValue : arr)
			{
				StructName TempStruct;
				FJsonObjectConverter::JsonObjectToUStruct<StructName>(JsonValue->AsObject().ToSharedRef(), &TempStruct, 0, 0);
				outArray->Add(TempStruct);
			}
		}
	}

	PWGAME_API void ParseJsonArray2BoolArray(FString filePath, TArray<bool>* outArray);

	template<typename StructName>
	void ParseJsonStringObject2Array(FString str, TArray<StructName>* outArray)
	{
		if (str.IsEmpty())
			return;

		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(str);
		TSharedPtr<FJsonValue> pJsonValue;

		if (FJsonSerializer::Deserialize(Reader, pJsonValue) && pJsonValue.IsValid())
		{

			TArray<TSharedPtr<FJsonValue>> arr = pJsonValue->AsArray();

			for (TSharedPtr<FJsonValue> JsonValue : arr)
			{
				StructName TempStruct;
				FJsonObjectConverter::JsonObjectToUStruct<StructName>(JsonValue->AsObject().ToSharedRef(), &TempStruct, 0, 0);
				outArray->Add(TempStruct);
			}
		}
	}

	template<typename StructName>
	void ParseJsonStringObject2UStruct(FString str, StructName* out)
	{
		if (str.IsEmpty())
			return;

		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(str);
		TSharedPtr<FJsonObject> JsonObject;

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			StructName TempStruct;
			FJsonObjectConverter::JsonObjectToUStruct<StructName>(JsonObject.ToSharedRef(), &TempStruct, 0, 0);
			*out = TempStruct;
		}
	}

	/**
	* Parse single JSON file.
	*
	*
	* @param filePath - Path from content. e.g  FString  filePath = FPaths::GameContentDir() + TEXT("Json/AppConfig.json");
	* @param outArray - out data .
	*/
	template<typename StructName>
	void ParseJsonArray2TMap(FString filePath, TMap<int32, uint8*>* outMap)
	{
		if (filePath.IsEmpty())
			return;

		FString FileContents;
		if (!FFileHelper::LoadFileToString(FileContents, *filePath))
		{
			return;
		}

		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);
		TSharedPtr<FJsonValue> pJsonValue;

		if (FJsonSerializer::Deserialize(Reader, pJsonValue) && pJsonValue.IsValid())
		{

			TArray<TSharedPtr<FJsonValue>> arr = pJsonValue->AsArray();

			for (TSharedPtr<FJsonValue> JsonValue : arr)
			{
				StructName* TempStruct = new StructName();

				FJsonObjectConverter::JsonObjectToUStruct<StructName>(JsonValue->AsObject().ToSharedRef(), &*TempStruct, 0, 0);

				if (TempStruct)
				{
					outMap->Add(TempStruct->Id, (uint8*)TempStruct);
				}

			}
		}
	}

	/**
	* Get Data Field Array.
	*
	*
	* @param filePath - Path from content. e.g  FString  filePath = FPaths::GameContentDir() + TEXT("Json/AppConfig.json");
	* @param outJsonObj - out data .
	*/
	PWGAME_API TArray<TArray<FString>>* GetDataFields(const FString filePath);


	/**
	* Write single JSON file.
	*
	*
	* @param filePath - Path from content. e.g  FString  filePath = FPaths::GameContentDir() + TEXT("Json/AppConfig.json");
	* @param inArray - in data .
	*/
	template<typename StructName>
	void WriteJsonArray2File(const FString filePath, const TArray<StructName>* inArray)
	{
		if (filePath.IsEmpty() || inArray == nullptr)
			return;

		TArray<TSharedPtr<FJsonValue>> JsonVals;
		TArray<TArray<FString>>* Fields = GetDataFields(filePath);
		if (Fields != nullptr)
		{
			for (auto i = 0; i < inArray->Num(); i++)
			{
				TSharedPtr<FJsonObject> JsonObj = FJsonObjectConverter::UStructToJsonObject<StructName>((*inArray)[i]);

				for (auto pair : JsonObj->Values)
				{
					if (!(*Fields)[i].Contains(pair.Key))
					{
						JsonObj->Values.Remove(pair.Key);
					}
				}

				TSharedPtr<FJsonValue> JsonVal = TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObj));
				JsonVals.Emplace(JsonVal);
			}
		}

		delete Fields;

		FString FileContents;

		TSharedRef< TJsonWriter<> > JsonWriter = TJsonWriterFactory<>::Create(&FileContents);
		FJsonSerializer::Serialize(JsonVals, JsonWriter);

		FFileHelper::SaveStringToFile(FileContents, *filePath);
	}
	/**
	* Write single JSON file.
	*
	* @zpj
	* @param filePath - Path from content.
	* @param inTMap - data in map.
	*/
	template<typename StructName>
	void WriteJsonTMap2File(const FString filePath, const TMap<FName, StructName>* inTMap)
	{
		TArray<StructName> TmpArray;
		if (inTMap->Num() > 0)
		{
			for (auto It = inTMap->CreateConstIterator(); It; ++It)
			{
				TmpArray.Add(It.Value());
			}
			WriteDataToFile(filePath, &TmpArray, EFileWrite::FILEWRITE_Append);
		}
	}

	/**
	*  TArray -> Json -> File
	*
	*
	* @param filePath - Path from content. e.g  FString  filePath = FPaths::GameContentDir() + TEXT("Json/AppConfig.json");
	* @param inArray - in data .
	*/
	template<typename StructName>
	void WriteDataToFile(const FString filePath, TArray<StructName>* inArray, const EFileWrite FileFlag = FILEWRITE_None)
	{
		if (filePath.IsEmpty() || inArray == nullptr)
			return;

		TArray<TSharedPtr<FJsonValue>> JsonVals;

		for (auto i = 0; i < inArray->Num(); i++)
		{
			TSharedPtr<FJsonObject> JsonObj = FJsonObjectConverter::UStructToJsonObject<StructName>((*inArray)[i]);

			TSharedPtr<FJsonValue> JsonVal = TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObj));
			JsonVals.Emplace(JsonVal);
		}


		FString FileContents;

		TSharedRef< TJsonWriter<> > JsonWriter = TJsonWriterFactory<>::Create(&FileContents);
		FJsonSerializer::Serialize(JsonVals, JsonWriter);

		FFileHelper::SaveStringToFile(FileContents, *filePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), FileFlag);
	}

	/**
	* Run external app
	*
	*
	* @param processPath - path.
	*/
	PWGAME_API bool RunProcess(FString processPath);

	/*template<typename TEnum>*/
	PWGAME_API FString GetEnumValueAsString(const FString& EnumName, int32 Index);

	PWGAME_API FText GetEnumValueAsDisplayName(const FString& EnumName, int32 InIndex);

	/*
	* Get all file names in the folder path.
	* bShortName : true ,get just the name of the file.
	* false, get name include the asset file path.
	*/
	PWGAME_API bool GetAllFileNames(const FString FolderPath, TArray<FName> &NameList, bool bShortName = true);

	/**Set flag false.*/
	template<typename T>
	void ClearCacheDataFlag(TMap<TWeakObjectPtr<T>, bool>& CacheDataList)
	{
		TArray<TWeakObjectPtr<T>> ActorList;
		CacheDataList.GetKeys(ActorList);
		for (int32 i = 0; i < ActorList.Num(); i++)
		{
			if (ActorList[i].IsValid())
			{
				CacheDataList[ActorList[i]] = false;
			}
		}
	}
	/**Get all actors in distance.*/
	template<typename T>
	void GetAllActorsofClassInDistance(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TMap<TWeakObjectPtr<T>, bool>& OutActors, const FVector CharacteLoc, const float Dist)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		// We do nothing if no is class provided, rather than giving ALL actors!
		if (ActorClass && World)
		{
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				T* TmpActor = Cast<T>(*It);
				if (TmpActor->IsValidLowLevel() && FVector::DistSquared(TmpActor->GetActorLocation(), CharacteLoc) < Dist*Dist)
				{
					if (OutActors.Contains(TmpActor))
					{
						OutActors[TmpActor] = true;
					}
					else
					{
						OutActors.Add(TmpActor, true);
					}
				}
			}//end for.
		}// end if.
	}

	//************************************
	// Method:    LoadActorsResource
	// FullName:  APWPlayerController::LoadActorsResource
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: TArray<T * > ActorArray
	// Parameter: const int32 Index
	// Parameter: const int32 Length
	//************************************
	template<class T>
	void LoadActorsResource(TArray<TWeakObjectPtr<T>> ActorArray, const int32 Index, const int32 Length)
	{
		int32 ActorsNum = ActorArray.Num();
		for (int32 i = Index; i < Index + Length && i < ActorsNum; i++)
		{
			if (ActorArray[i].IsValid()
				&& ActorArray[i].Get()->GetClass()
				&& ActorArray[i].Get()->GetClass()->ImplementsInterface(UClientAroundInterface::StaticClass()) == true)
			{
				if (ActorArray[i].IsValid())
				{
					ActorArray[i].Get()->ClientLoadResource();
				}
			}
		}//end for.
	}
	//************************************
	// Method:    TickLoadAroundActors
	// FullName:  APWPlayerController::TickLoadAroundActors
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: float DeltaSeconds
	// Parameter: float & AroundTickTime
	// Parameter: const float StepTime
	// Parameter: const FVector CharacterLocation
	// Parameter: const float LoadResourceDist
	// Parameter: const float InViewDist
	// Parameter: TMap<T *
	// Parameter: bool> & CacheNewActors
	// Parameter: TArray<TWeakObjectPtr<T>> & CacheInViewActors
	// Parameter: TArray<T * > & LoadActorList
	// Parameter: int32 & iBatchCount
	// Parameter: int32 & BatchSize
	// Parameter: const int32 LoadFPSNum
	//************************************
	template<class T>
	void TickLoadAroundActors(float DeltaSeconds, float& AroundTickTime, const float StepTime, const FVector CharacterLocation, const float LoadResourceDist, const float InViewDist,
		TMap<TWeakObjectPtr<T>, bool>& CacheNewActors,
		TArray<TWeakObjectPtr<T>>& CacheInViewActors, TArray<TWeakObjectPtr<T>>& LoadActorList, int32& iBatchCount, int32& BatchSize, const int32 LoadFPSNum = 20)
	{
		AroundTickTime += DeltaSeconds;
		//float CheckLoadResourceDistance = LoadResourceDist; // 1Km
		//float CheckInViewDistance = InViewDist; // 200m ,becasuse no every tick will collection .so vehicle max speed ~=36m/s。so 2s*36m = 72m. in view.
		FVector CharacterLoc = CharacterLocation;
		if (AroundTickTime > StepTime)
		{
			AroundTickTime = 0.0f;
			PWLIB::ClearCacheDataFlag<T>(CacheNewActors);
			PWLIB::GetAllActorsofClassInDistance<T>(GetWorld(), T::StaticClass(), CacheNewActors, CharacterLoc, LoadResourceDist);
			TArray<TWeakObjectPtr<T>> TmpActorArray;
			CacheNewActors.GetKeys(TmpActorArray);
			CacheInViewActors.Empty(500);
			LoadActorList.Empty(500);
			for (int32 i = 0; i < TmpActorArray.Num(); i++)
			{
				if (TmpActorArray[i].IsValid() && TmpActorArray[i].Get()->IsValidLowLevel())
				{
					if (true == CacheNewActors[TmpActorArray[i]])
					{
						if (FVector::DistSquared(TmpActorArray[i].Get()->GetActorLocation(), CharacterLoc) <= InViewDist*InViewDist)
						{
							CacheInViewActors.Add(TmpActorArray[i]);
						}
						//PickupActorList[i]->ClientLoadResource();
						LoadActorList.Add(TmpActorArray[i]);
					}
					else
					{
						TmpActorArray[i].Get()->ClientUnloadResource();
						CacheNewActors.Remove(TmpActorArray[i]);
					}
				}
			}
			//
			int32 ActorNum = LoadActorList.Num();
			iBatchCount = 0;
			BatchSize = FMath::CeilToInt(ActorNum*1.0f / LoadFPSNum);
		}
	}


	/**Check client character type.*/
	ECharacterType GetLocalCharacterType();
	/**Check client career type.*/
	EPWPlayerCareerType GetLocalCareerType();
	/**IsCareer equal.*/
	static bool IsCareerBitMaskEqual(const EPWPlayerCareerType CareerType, const int32 BitMaskCareerType)
	{
		return ((uint32)CareerType > 0) && ((1 << (uint32)CareerType) & BitMaskCareerType) > 0;
	}
	/**Set actor to ground according to the terrain falling gradient.*/
	void SetActorCloseToGround(AActor *ActorShouldIgnore, AActor* Actor, const FVector& Location, const FRotator& Rotation);

	FText GetLocTextFromBodyPart(EBodyPart BodyPart);

}

/************************************************************************/
/*  Set Timer by handle.                                                */
/************************************************************************/
#define PW_SETTIMER_LOOP(handle, FunctionName, InRate) PW_SETTIMER(handle, FunctionName, InRate, true)
#define PW_SETTIMER_UNLOOP(handle, FunctionName, InRate) PW_SETTIMER(handle, FunctionName, InRate, false)
#define PW_SETTIMER(handle, FunctionName, InRate, bLoop)\
if (!GetWorldTimerManager().IsTimerActive(handle) && (InRate) >= 0.0f)\
{\
	GetWorldTimerManager().SetTimer(handle, this, &ThisClass::FunctionName, InRate, bLoop); \
}
#define PW_CLEARTIMER(handle) (GetWorldTimerManager().ClearTimer(handle))
#define PW_ISTIMERACTIVE(handle) (GetWorldTimerManager().IsTimerActive(handle))

#define  ENGINE_VESION_CHECK() static_assert((ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 21) && ENGINE_PATCH_VERSION == 0, "code cross the border");
//////////////////////////////////////////////////////////////////////////

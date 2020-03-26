#include "PWLibrary.h"
#include "DebugTools/Debug/PWDebugActor.h"
#include "Kismet/GameplayStatics.h"
#include "PWWeaponDataStructs.h"
#include "PWDataTable.h"
#include "PWDamageConfig.h"
#include "DeathMessage.h"
#include "UnrealClient.h"
#include "Runtime/AutomationWorker/Public/IAutomationWorkerModule.h"
#include "PWCharacter.h"
#include "PWCharacterData_Runtime.h"
#include "PlayerController/PWPlayerController.h"
#include "PWGameSettings.h"
#include "WidgetBlueprintLibrary.h"
#include "Rendering/DrawElements.h"
#include "PWBuffDataStructs.h"
#include "PWProcedureManager.h"
#include "PWDroppedEquipment.h"
#include "Client/PWProcedureStandaloneCopy.h"
#include "PWStandaloneNoticeWidget.h"
#include "PWUIManager.h"
#include "PWGameState.h"
#include "PWAirForteManager.h"
#include "PWGameDataConfig.h"
#include "FileHelper.h"
#include "Paths.h"
#include "PWUIStyleConfig.h"
#include "GameFramework/PlayerController.h"
#include "PWPlayerControllerBase.h"
#include "PWCharacterBase.h"
#include "DHCommon.h"
#include "AkGameplayStatics.h"
#include "Engine/NetConnection.h"
#include "PWInventoryDataStructs.h"
#include "SaveGame/PWSaveGameDefine.h"
#include "DHInteractTipsStruct.h"
#include "Werewolf/Common/PWTeachingInImageWidget.h"
#include "Client/PWProcedureBattle.h"
#include "PWSaveGameManager.h"
#include "SaveGame/PWSGBase.h"
#if PLATFORM_PS4
#include "Online/PS4/DHOnlinePS4.h"
#endif
#include "PWInteractionWidget.h"
#include "PWDamageEvents.h"


#define L1SOUND -1
#define R1SOUND 1

extern ENGINE_API float GAverageFPS;
static FString AreaName = TEXT("SIEA");

PWGAME_API UPWGameInstance* PWLIB::GetGameInst()
{
	if (GWorld == nullptr)
		return nullptr;

	UPWGameInstance* ret = Cast<UPWGameInstance>(GWorld->GetGameInstance());
	return ret;
}

PWGAME_API UWorld * PWLIB::GetWorld()
{
	return GWorld.GetReference();
}

FString PWLIB::GetCharacterTableNameByCharacterType(EPWPlayerCareerType type)
{
	if (type == EPWPlayerCareerType::CPPCT_Human)
		return TEXT("PWCharacterData_Human");
	if (type == EPWPlayerCareerType::CPPCT_Wolf)
		return TEXT("PWCharacterData_Wolf");
	if (type == EPWPlayerCareerType::CPPCT_Werewolf)
		return TEXT("PWCharacterData_Werewolf");
	if (type == EPWPlayerCareerType::CPPCT_Leopard)
		return TEXT("PWCharacterData_Leopard");
	if (type == EPWPlayerCareerType::CPPCT_DemiLeopard)
		return TEXT("PWCharacterData_DemiLeopard");

	return FString();
}

PWGAME_API void PWLIB::SetAccountArea(const FString &Area)
{
	AreaName = Area;
}


//PWGAME_API UMWJsonTable* MWLIB::GetJsonTableInst()
//{
//	return GetGameInst()->GetJsonTableSafe();
//}

PWGAME_API UClass* PWLIB::LoadBPFromPathName(FString PathName)
{
	UClass* ret = nullptr;
	FStringAssetReference refPath;
	refPath.SetPath(PathName + TEXT("_C"));

	auto* Obj = refPath.ResolveObject();

	if (Obj == nullptr)
	{
		Obj = refPath.TryLoad();
	}

	if (Obj != nullptr)
	{
		ret = Cast<UClass>(Obj);
	}

	return ret;
}

PWGAME_API UClass* PWLIB::LoadBPFromPathName(FStringAssetReference StringAssetRef)
{
	UClass* ret = nullptr;
	ret = LoadBPFromPathName(StringAssetRef.ToString());

	return ret;
}

PWGAME_API void PWLIB::UnLoadBPFromPathName(FString PathName)
{
	FStringAssetReference refPath = PathName;

	auto* Obj = refPath.ResolveObject();

	if (!GIsEditor && Obj)
	{
		GetGameInst()->StreamMgr.Unload(refPath);
	}

}

PWGAME_API UClass* PWLIB::FindClassOfName(FString ClassName, FString PathToLoadIfNotFind)
{
	UClass* Result = nullptr;

	Result = FindObject<UClass>(ANY_PACKAGE, *ClassName);
	if (!Result)
	{
		FString Path = PathToLoadIfNotFind + "." + ClassName;
		Result = StaticLoadClass(APWDebugActor::StaticClass(), NULL, *Path, NULL, LOAD_None, NULL);
	}

	return Result;
}

PWGAME_API void PWLIB::UnLoadBPFromPathName(FStringAssetReference StringAssetRef)
{
	UnLoadBPFromPathName(StringAssetRef.ToString());
}

PWGAME_API void PWLIB::LoadAssetSimpleAsync(FStringAssetReference AssetRef)
{
	if (!GIsEditor)
	{
		GetGameInst()->StreamMgr.RequestAsyncLoad(AssetRef, FStreamableDelegate(), 0, true);
	}
}

PWGAME_API void PWLIB::LoadAssetASync(FStringAssetReference AssetRef, FStreamableDelegate Del)
{
	if (!GIsEditor)
	{
		GetGameInst()->StreamMgr.RequestAsyncLoad(AssetRef, Del);
	}
}

PWGAME_API void PWLIB::LoadAssetASync(FString AssetPath, FStreamableDelegate Del)
{
	if (!GIsEditor)
	{
		FStringAssetReference refPath = AssetPath;
		GetGameInst()->StreamMgr.RequestAsyncLoad(refPath, Del);
	}
}

PWGAME_API void PWLIB::LoadAssetArrayASync(TArray<FStringAssetReference> AssetRef, FStreamableDelegate Del)
{
	//if (!GIsEditor)
	//{
		GetGameInst()->StreamMgr.RequestAsyncLoad(AssetRef, Del);
	//}
}

bool PWLIB::GetFloatByName(UObject* Target, FName VarName, float &OutFloat)
{
	if (Target) //make sure Target was set in blueprints. 
	{
		float FoundFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);  // try to find float property in Target named VarName
		if (FloatProp) //if we found variable
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);  // get the value from FloatProp
			OutFloat = FoundFloat;
			return true;
		}
	}
	return false;
}

bool PWLIB::GetIntByName(UObject* Target, FName VarName, int32 &OutInt)
{
	if (Target)
	{
		int32 FoundInt;
		UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName);  //this time I'm using UIntProperty as I'm searching for int
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			OutInt = FoundInt;
			return true;
		}
	}
	return false;
}

bool PWLIB::SetFloatByName(UObject* Target, FName VarName, float NewValue, float &OutFloat)
{
	if (Target)
	{
		float FoundFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FloatProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			OutFloat = FoundFloat;
			return true;
		}
	}
	return false;
}

bool PWLIB::SetIntByName(UObject* Target, FName VarName, int32 NewValue, int32 &OutInt)
{
	if (Target)
	{
		int32 FoundInt;
		UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			IntProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			OutInt = FoundInt;
			return true;
		}
	}
	return false;
}

bool PWLIB::IncreaseFloatByName(UObject* Target, FName VarName, float ValueToAdd, float &OutFloat)
{
	if (Target)
	{
		float FoundFloat, IncreasedFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);
		{
			if (FloatProp)
			{
				FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
				IncreasedFloat = FoundFloat + ValueToAdd;  //increase the value 
				FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat); //and then set it
				OutFloat = IncreasedFloat;
				return true;
			}
		}
	}
	return false;
}

bool PWLIB::IncreaseIntByName(UObject* Target, FName VarName, int32 ValueToAdd, int32 &OutInt)
{
	if (Target)
	{
		int32 FoundInt, IncreasedInt;
		UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			IncreasedInt = FoundInt + ValueToAdd;
			IntProp->SetPropertyValue_InContainer(Target, IncreasedInt);
			OutInt = IncreasedInt;
			return true;
		}
	}
	return false;
}

PWGAME_API void PWLIB::ParseJsonArray2BoolArray(FString filePath, TArray<bool>* outArray)
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
			outArray->Add(JsonValue->AsBool());
		}
	}
}

PWGAME_API void PWLIB::UnLoadAssetFromPath(FStringAssetReference AssetRef)
{
	auto* Obj = AssetRef.ResolveObject();

	if (!GIsEditor && Obj)
	{
		GetGameInst()->StreamMgr.Unload(AssetRef);
	}

}

PWGAME_API TArray<TArray<FString>>* PWLIB::GetDataFields(const FString filePath)
{
	if (filePath.IsEmpty())
		return nullptr;

	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *filePath))
	{
		return nullptr;
	}

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);
	TSharedPtr<FJsonValue> pJsonValue;

	if (FJsonSerializer::Deserialize(Reader, pJsonValue) && pJsonValue.IsValid())
	{
		TArray<TSharedPtr<FJsonValue>> arr = pJsonValue->AsArray();
		TArray<TArray<FString>>* StrArrPPtr = new TArray<TArray<FString>>();
		for (auto var : arr)
		{
			TArray<FString> StrArrPtr;
			var->AsObject()->Values.GenerateKeyArray(StrArrPtr);
			StrArrPPtr->Emplace(StrArrPtr);
		}

		return StrArrPPtr;
	}
	return nullptr;
}

PWGAME_API bool PWLIB::RunProcess(FString processPath)
{

	int fRet = 0;
#if PLATFORM_WINDOWS
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW;


	fRet = CreateProcess(
		processPath.GetCharArray().GetData()
		, NULL
		, NULL
		, NULL
		, NULL
		, NULL
		, NULL
		, NULL
		, &si
		, &pi
	);
#endif
	return fRet > 0 ? true : false;
}

FString PWLIB::GetEnumValueAsString(const FString& EnumName, int32 Index)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}
	return enumPtr->GetNameStringByIndex(Index);
}

FText PWLIB::GetEnumValueAsDisplayName(const FString& EnumName, int32 InIndex) {
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!enumPtr)
	{
		return FText::FromString(TEXT("Invalid"));
	}
	return enumPtr->GetDisplayNameTextByIndex(InIndex);
}

float UPWBlueprintLibrary::GetMaxAutoPickupAmmoAmount()
{
	return 300.f;
}

float UPWBlueprintLibrary::GetMaxAutoPickupMedicalItemAmount()
{
	return 6.f;
}

float UPWBlueprintLibrary::GetNormalDistribution(FRandomStream& RS, float mean /*= 0.f*/, float Sigma /*= 1.f*/)
{
	static bool bVailable = false;
	static float NextND = 0.f;
	if (bVailable)
	{
		bVailable = false;
		return NextND;
	}

	// Box-Muller transform

	const float Rand1 = FMath::Max<float>(RS.FRand(), SMALL_NUMBER);
	const float Rand2 = FMath::Max<float>(RS.FRand(), SMALL_NUMBER);
	const float SqrtLn = Sigma * FMath::Sqrt(-2.f * FMath::Loge(Rand1));
	const float Rand2TwoPi = Rand2 * 2.f * PI;
	const float Z1 = SqrtLn * FMath::Cos(Rand2TwoPi);

	//store next ND
	bVailable = true;
	NextND = mean + SqrtLn * FMath::Sin(Rand2TwoPi);

	return mean + Z1;
}

float UPWBlueprintLibrary::GetNormalDistribution_Test(float mean /*= 0.f*/, float Sigma /*= 1.f*/)
{
	FRandomStream RS(FMath::Rand());
	return GetNormalDistribution(RS, mean, Sigma);
}

void UPWBlueprintLibrary::GetWeaponIconName(int32 WeaponId, class UTexture2D* &WeaponIcon, FText &WeaponName)
{
	FPWWeaponInfo WeaponInfo;
	if (FPWDataTable::Get().GetItemById<FPWWeaponInfo>(WeaponId, WeaponInfo))
	{
		WeaponIcon = PWLIB::SynchronouslyLoadAsset<UTexture2D>(WeaponInfo.UIIcon.ToSoftObjectPath());
		WeaponName = WeaponInfo.ItemName;
	}
}

void UPWBlueprintLibrary::GetDeathIconName(uint8 DeathType, class UTexture2D* &WeaponIcon, FText &WeaponName)
{
	int32 ConfigID = -1;
	EPWDeadType Death = (EPWDeadType)DeathType;
	switch (Death)
	{
	case EPWDeadType::DeadType_CirclePosison:
		ConfigID = 8200001;
		break;
	case EPWDeadType::DeadType_Falling:
		ConfigID = 8200004;
		break;
	case EPWDeadType::DeadType_Vehicle:
		ConfigID = 8200002;
		break;
	case EPWDeadType::DeadType_VehicleBomb:
		ConfigID = 8200003;
		break;
	case EPWDeadType::DeadType_Grenade:
		ConfigID = 8200005;
		break;
	case EPWDeadType::DeadType_LeaveMatch:
		ConfigID = 8200006;
		break;
	case EPWDeadType::DeadType_Melee:
		ConfigID = 8200007;
		break;
	case EPWDeadType::DeadType_Suffocating:
		ConfigID = 8200008;
		break;
	case EPWDeadType::DeadType_Bomber:
		ConfigID = 8200009;
		break;
	case EPWDeadType::DeadType_FallingObject:
		ConfigID = 8200010;
		break;
	case EPWDeadType::DeadType_DieOut:
		ConfigID = 8200011;
		break;
	case EPWDeadType::DeadType_Frostbite:
		ConfigID = 8200013;
		break;
	//case EPWDeadType::DeadType_Wolf:
	//	ConfigID = 8200014;
	//	break;
	//case EPWDeadType::DeadType_Terminate:
	//	ConfigID = 8200015;
	//	break;
	case EPWDeadType::DeadType_Weapon:
		ConfigID = 8200016;
		break;
	default:
		break;
	}
	FPWDamageConfig DamageInfo;
	if (FPWDataTable::Get().GetItemById<FPWDamageConfig>(ConfigID, DamageInfo))
	{
		WeaponIcon = PWLIB::SynchronouslyLoadAsset<UTexture2D>(DamageInfo.UIIcon.ToSoftObjectPath());
		WeaponName = DamageInfo.DamageName;
	}
}

void UPWBlueprintLibrary::GetDamageIconName(uint8 DamageType, class UTexture2D* &WeaponIcon, FText &WeaponName)
{
	int32 ConfigID = -1;
	EPWDamageTypeNew Damage = (EPWDamageTypeNew)DamageType;
	switch (Damage)
	{
	//case EPWDamageType::DT_CirclePoison:
	//	ConfigID = 8200001;
	//	break;
	case EPWDamageTypeNew::DT_Falling:
		ConfigID = 8200004;
		break;
	//case EPWDamageType::DT_Vehicle:
	//	ConfigID = 8200002;
	//	break;
	//case EPWDamageType::DT_VehicleBomb:
	//	ConfigID = 8200003;
	//	break;
	case EPWDamageTypeNew::DT_Grenade:
		ConfigID = 8200005;
		break;
	//case EPWDamageType::DT_Melee:
	////case EPWDamageType::DT_WolfAttack:
	//	ConfigID = 8200007;
	//	break;
	//case EPWDamageType::DT_Suffocating:
	//	ConfigID = 8200008;
	//	break;
	//case EPWDamageType::DT_Bomber:
	//	ConfigID = 8200009;
	//	break;
	//case EPWDamageType::DT_FallingObject:
	//	ConfigID = 8200010;
	//	break;
	//case EPWDamageType::DT_Frostbite:
	//	ConfigID = 8200013;
	//	break;
	//case EPWDamageType::DT_WolfAttack:
	//	ConfigID = 8200014;
	//	break;
	default:
		break;
	}
	FPWDamageConfig DamageInfo;
	if (FPWDataTable::Get().GetItemById<FPWDamageConfig>(ConfigID, DamageInfo))
	{
		WeaponIcon = PWLIB::SynchronouslyLoadAsset<UTexture2D>(DamageInfo.UIIcon.ToSoftObjectPath());
		WeaponName = DamageInfo.DamageName;
	}
}

bool UPWBlueprintLibrary::GetFloatByName(UObject* Target, FName VarName, float &OutFloat)
{
	return PWLIB::GetFloatByName(Target, VarName, OutFloat);
}

bool UPWBlueprintLibrary::GetIntByName(UObject* Target, FName VarName, int32 &OutInt)
{
	return PWLIB::GetIntByName(Target, VarName, OutInt);
}


bool UPWBlueprintLibrary::SetFloatByName(UObject* Target, FName VarName, float NewValue, float &OutFloat)
{
	return PWLIB::SetFloatByName(Target, VarName, NewValue, OutFloat);
}

bool UPWBlueprintLibrary::SetIntByName(UObject* Target, FName VarName, int32 NewValue, int32 &OutInt)
{
	return PWLIB::SetIntByName(Target, VarName, NewValue, OutInt);
}

bool UPWBlueprintLibrary::IncreaseFloatByName(UObject* Target, FName VarName, float ValueToAdd, float &OutFloat)
{
	return PWLIB::IncreaseFloatByName(Target, VarName, ValueToAdd, OutFloat);
}

bool UPWBlueprintLibrary::IncreaseIntByName(UObject* Target, FName VarName, int32 ValueToAdd, int32 &OutInt)
{
	return PWLIB::IncreaseIntByName(Target, VarName, ValueToAdd, OutInt);
}

APWDebugActor* UPWBlueprintLibrary::GetPWDebugActor(UObject* WorldContext)
{
	if (WorldContext == nullptr)
		return nullptr;

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, APWDebugActor::StaticClass(), OutActors);

	for (AActor* A : OutActors)
	{
		APWDebugActor* DA = Cast<APWDebugActor>(A);
		if (DA)
		{
			return DA;
		}
	}

	UWorld* World = WorldContext->GetWorld();
	if (World)
	{
		UClass* DebugActorClass = PWLIB::FindClassOfName("BP_PWDebugActor_C", "/Game/BP/RD/Debug/BP_PWDebugActor");
		FActorSpawnParameters SP;
		APWDebugActor* DebugActor = World->SpawnActor<APWDebugActor>(DebugActorClass, SP);
		if (DebugActor)
		{
			return DebugActor;
		}
	}

	return nullptr;
}

void UPWBlueprintLibrary::RegisterNewDebugTarget(UObject* TargetObj, TArray<FName> VariableNames)
{
	APWDebugActor* DebugActor = GetPWDebugActor(TargetObj);
	if (DebugActor)
	{
		DebugActor->RegisterNewTarget(TargetObj, VariableNames);
	}
}

void UPWBlueprintLibrary::LogPerformance()
{


	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Error, TEXT("DedicatedServerPFS=%s"), *FString::Printf(TEXT("%0.2f"), GAverageFPS));
	}
	else
	{
		if (GWorld == nullptr || GWorld->GetGameViewport() == nullptr)
		{
			return;
		}

		const FStatUnitData* StatUnitData = GWorld->GetGameViewport()->GetStatUnitData();

		FAutomationPerformanceSnapshot PerfSnapshot;
		PerfSnapshot.Changelist = FString::FromInt(0);
		PerfSnapshot.BuildConfiguration = EBuildConfigurations::ToString(FApp::GetBuildConfiguration());
		PerfSnapshot.MapName = GWorld->GetMapName();
		PerfSnapshot.MatineeName = TEXT("Null");
		PerfSnapshot.AverageFPS = FString::Printf(TEXT("%0.2f"), GAverageFPS);
		PerfSnapshot.AverageFrameTime = FString::Printf(TEXT("%0.2f"), StatUnitData->FrameTime);
		PerfSnapshot.AverageGameThreadTime = FString::Printf(TEXT("%0.2f"), StatUnitData->GameThreadTime);
		PerfSnapshot.AverageRenderThreadTime = FString::Printf(TEXT("%0.2f"), StatUnitData->RenderThreadTime);
		PerfSnapshot.AverageGPUTime = FString::Printf(TEXT("%0.2f"), StatUnitData->GPUFrameTime);
		// PerfSnapshot.PercentOfFramesAtLeast60FPS = ???;	// @todo
		// PerfSnapshot.PercentOfFramesAtLeast60FPS = ???;	// @todo

		const FString PerfSnapshotAsCommaDelimitedString = PerfSnapshot.ToCommaDelimetedString();

		UE_LOG(LogTemp, Error, TEXT("%s"), *PerfSnapshotAsCommaDelimitedString);
	}

}

UPWDelegateManager* UPWBlueprintLibrary::GetDelegateManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetDelegateManager();

	return nullptr;
}

UPWUIManager* UPWBlueprintLibrary::GetUIManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetUIManager();
	return nullptr;
}

UPWOperationNoticeManager* UPWBlueprintLibrary::GetOperationNoticeManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetOperationNoticeManager();
	return nullptr;
}

UPWProcedureManager* UPWBlueprintLibrary::GetProcedureManager(UObject* ContextObject)
{
	return UPWProcedureManager::GetInstance(ContextObject);
}

UPWRouteManager* UPWBlueprintLibrary::GetRouteManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetRouteManager();
	return nullptr;
}

UPWPromptManager* UPWBlueprintLibrary::GetPromptManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetPromptManager();
	return nullptr;
}

UPWStorageManager* UPWBlueprintLibrary::GetStorageManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetStorageManager();
	return nullptr;
}

UPWSkillManager* UPWBlueprintLibrary::GetSkillManager(UObject* ContextObj) {
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObj);
	if (GameInstance)
		return GameInstance->GetSkillManager();
	return nullptr;
}

UPWLogManager* UPWBlueprintLibrary::GetLogManager(UObject* ContextObj) {
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObj);
	if (GameInstance)
		return GameInstance->GetLogManager();
	return nullptr;
}

bool UPWBlueprintLibrary::IsPlayWithGamePad()
{
#if PLATFORM_PS4
	return true;
#else
	return UPWGameSettings::Get()->ClientSettings.bWithGamePad;
#endif
}

bool UPWBlueprintLibrary::IsPlayWithSteam()
{
//#if PLATFORM_PS4 || WITH_EDITOR
//	return false;
//#else
//	FString FilePath = FPaths::ProjectDir() / TEXT("Binaries") / TEXT("Win64") / TEXT("Platform.ini");
//	return !FPaths::FileExists(FilePath);
//#endif
	return false;
}

bool UPWBlueprintLibrary::IsPlayWithPS4()
{
#if PLATFORM_PS4
	return true;
#else
	return false;
#endif
}

#if 0
void UPWBlueprintLibrary::PWLIBDrawRealLineCircle(UPARAM(ref) FPaintContext& Context, const FVector2D& Center, float Radius, UCurveFloat* SmoothCurve,  FLinearColor Tint, bool bAntiAlias) {
	float D = 3.14f / 180.0f;
	if (Radius < 1)
	{
		Radius = 1;
	}
	TArray<FVector2D> RealLinePoints;
	int32 PointNum = 2 * 3.14 * Radius * SmoothCurve->GetFloatValue(Radius);
	float AnglePerPoint = 360.0f / PointNum;
	RealLinePoints.SetNum(PointNum + 1);
	int32 PointIndex = 0;
	float Angle = 0.0f;
	for (; PointIndex <= PointNum; ++PointIndex) {
		float x = Radius * FMath::Cos(PointIndex * AnglePerPoint * D);
		float y = Radius * FMath::Sin(PointIndex * AnglePerPoint * D);
		RealLinePoints[PointIndex] = (FVector2D(Center.X + x, Center.Y + y));
	}
	UWidgetBlueprintLibrary::DrawLines(Context, RealLinePoints, Tint, bAntiAlias);
}

void UPWBlueprintLibrary::PWLIBDrawDottedLineCircle(UPARAM(ref) FPaintContext& Context, const FVector2D& Center, float Radius, UCurveFloat* RealLineLenRadiusCurve, UCurveFloat* EmptyLineLenRadiusCurve, FLinearColor Tint, bool bAntiAlias){
	float D = 3.14f / 180.0f;
	float AngleAcc = 0;
	float RealLineLen = RealLineLenRadiusCurve->GetFloatValue(Radius);
	float EmptyLineLen = EmptyLineLenRadiusCurve->GetFloatValue(Radius);
	if (Radius<1)
	{
		Radius = 1;
	}
	float RealLineAngle = 360 / ((3.14f * 2 * Radius) / RealLineLen);
	float EmptyAngle = 360 / ((3.14f * 2 * Radius) / EmptyLineLen);
	int32 RealLineAngleDrawTime = FMath::CeilToInt(RealLineAngle / 3);
	while (AngleAcc <= 360) {
		float CachedLastX = INT_MAX;
		float CachedLastY = INT_MAX;
		for (int32 i= 0; i < RealLineAngleDrawTime; ++i)
		{
			float FromX, FromY;
			if (CachedLastX != INT_MAX)
			{	
				FromX = CachedLastX;
				FromY = CachedLastY;
			}
			else {
				FromX = Radius * FMath::Cos(AngleAcc *D);
				FromY = Radius * FMath::Sin(AngleAcc* D);
			}
			AngleAcc += RealLineAngle / RealLineAngleDrawTime;
			float ToX = Radius * FMath::Cos(AngleAcc*D);
			float ToY = Radius * FMath::Sin(AngleAcc*D);
			CachedLastX = ToX;
			CachedLastY = ToY;
			
			UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(Center.X + FromX, Center.Y + FromY), FVector2D(Center.X + ToX, Center.Y + ToY), Tint, bAntiAlias);
		}

		AngleAcc += EmptyAngle;
	}
}

void UPWBlueprintLibrary::PWLIBDrawDottedLine(UPARAM(ref) FPaintContext& Context, const FVector2D& From, const FVector2D& To, float RealLineLen, float EmptyLineLen, const FColor& Color, bool bAntiAlias) {
	FVector2D Vector = (To - From);
	float Len = Vector.Size();
	int32 Fragments = Len / (RealLineLen + EmptyLineLen);
	Vector.Normalize();
	FVector2D RealLineAcc = Vector * RealLineLen;
	FVector2D EmptyLineAcc = Vector * EmptyLineLen;
	FVector2D Acc = From;
	for (int32 i = 0; i < Fragments; ++i)
	{
		FVector2D FragmentFrom = Acc;
		FVector2D FragmentTo = Acc + RealLineAcc;
		UWidgetBlueprintLibrary::DrawLine(Context, FragmentFrom, FragmentTo, Color, bAntiAlias);
		Acc += RealLineAcc;
		Acc += EmptyLineAcc;
	}
	UWidgetBlueprintLibrary::DrawLine(Context, Acc, To, Color, bAntiAlias);
}
#else
static void CalcLineCirclePoints(TArray<FVector2D>& InPoints, const FVector2D& InCenter, float InRadius, int32 InNum, float InInitial, float InDelta)
{
	InPoints.SetNum(InNum + 1);
	float AngleAcc = InInitial;
	for (int32 Idx = 0; Idx <= InNum; ++Idx)
	{
		float ScalarSin = FMath::Sin(AngleAcc);
		float ScalarCos = FMath::Cos(AngleAcc);
		AngleAcc += InDelta;
		const FVector2D Point = FVector2D(InRadius * ScalarCos, InRadius * ScalarSin);
		InPoints[Idx] = InCenter + Point;
	}
}

void UPWBlueprintLibrary::PWLIBDrawRealLineCircle(UPARAM(ref) FPaintContext& Context, const FVector2D& Center, float Radius, UCurveFloat* SmoothCurve, FLinearColor Tint, bool bAntiAlias)
{
	if (Radius < 1) Radius = 1;

	const float Value = SmoothCurve->GetFloatValue(Radius);
	const int32 Num = FMath::Clamp((int32)Value, 10, 200);
	const float Delta = 2 * PI / Num;

	TArray<FVector2D> Points;
	CalcLineCirclePoints(Points, Center, Radius, Num, 0, Delta);

	FSlateDrawElement::MakeLines(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		Tint,
		bAntiAlias);
}

void UPWBlueprintLibrary::PWLIBDrawDottedLineCircle(UPARAM(ref) FPaintContext& Context, const FVector2D& Center, float Radius, UCurveFloat* RealLineLenRadiusCurve, UCurveFloat* EmptyLineLenRadiusCurve, FLinearColor Tint, bool bAntiAlias)
{
	if (Radius < 1) Radius = 1;

	const float Value1 = RealLineLenRadiusCurve->GetFloatValue(Radius);
	const float Value2 = EmptyLineLenRadiusCurve->GetFloatValue(Radius);
	const int32 Num = FMath::Clamp((int32)Value1, 10, 200);
	const float Ratio = FMath::Clamp(Value2, 0.0f, 1.0f);
	const float TotalAngle = 2 * PI / Num;
	const float RealAngle = TotalAngle * Ratio;
	const float EmptyAngle = TotalAngle - RealAngle;

	TArray<FVector2D> PointsA, PointsB;
	CalcLineCirclePoints(PointsA, Center, Radius, Num, 0, TotalAngle);
	CalcLineCirclePoints(PointsB, Center, Radius, Num, RealAngle, TotalAngle);

	TArray<FVector2D> Line; Line.AddDefaulted(2);
	for (int32 Idx = 0; Idx < Num; Idx++)
	{
		Line[0] = PointsA[Idx];
		Line[1] = PointsB[Idx];
		FSlateDrawElement::MakeLines(
			Context.OutDrawElements,
			Context.MaxLayer,
			Context.AllottedGeometry.ToPaintGeometry(),
			Line,
			ESlateDrawEffect::None,
			Tint,
			bAntiAlias);
	}
}

void UPWBlueprintLibrary::PWLIBDrawDottedLine(UPARAM(ref) FPaintContext& Context, const FVector2D& From, const FVector2D& To, float RealLineLen, float EmptyLineLen, const FColor& Color, bool bAntiAlias)
{
	FVector2D Dir = From - To;
	const float TotalLen = RealLineLen + EmptyLineLen;
	const int32 Num = Dir.Size() / TotalLen;
	const float LeftLen = Dir.Size() - Num * TotalLen;
	Dir.Normalize();
	FVector2D PointA = To;
	FVector2D PointB = To + Dir * RealLineLen;
	const FVector2D Delta = Dir * TotalLen;
	TArray<FVector2D> Line; Line.AddDefaulted(2);
	for (int32 Idx = 0; Idx < Num; Idx++)
	{
		Line[0] = PointA; PointA += Delta;
		Line[1] = PointB; PointB += Delta;
		FSlateDrawElement::MakeLines(
			Context.OutDrawElements,
			Context.MaxLayer,
			Context.AllottedGeometry.ToPaintGeometry(),
			Line,
			ESlateDrawEffect::None,
			Color,
			bAntiAlias);
	}
	Line[0] = PointA;
	Line[1] = PointA + Dir * FMath::Min(LeftLen, RealLineLen);
	FSlateDrawElement::MakeLines(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		Line,
		ESlateDrawEffect::None,
		Color,
		bAntiAlias);
}
#endif

float UPWBlueprintLibrary::GetHealthBuffMaxTime()
{
	static float MaxTime = 0.0f;
	if (FMath::IsNearlyZero(MaxTime))
	{
		TArray<FPWBuffInfo> BuffList;
		FPWDataTable::Get().GetDataByDataType<FPWBuffInfo>(BuffList);
		for (auto BuffInfo : BuffList)
		{
			if (BuffInfo.BuffFlag == EBuffFlags::BF_Energy_Health)
			{
				MaxTime += BuffInfo.Duration;
			}
		}
	}
	return MaxTime;
}

float UPWBlueprintLibrary::GetSpeedBuffMaxTime()
{
	static float MaxTime = 0.0f;
	if (FMath::IsNearlyZero(MaxTime))
	{
		TArray<FPWBuffInfo> BuffList;
		FPWDataTable::Get().GetDataByDataType<FPWBuffInfo>(BuffList);
		for (auto BuffInfo : BuffList)
		{
			if (BuffInfo.BuffFlag == EBuffFlags::BF_Energy_Speed)
			{
				MaxTime += BuffInfo.Duration;
			}
		}
	}
	return MaxTime;
}

void UPWBlueprintLibrary::UpdateBattleUI(UObject* ContextObj)
{
	if (ContextObj == nullptr)
		return;
	if (UPWProcedureManager::GetInstance(ContextObj) == nullptr)
		return;

	if (UPWProcedureManager::GetInstance(ContextObj)->GetCurState() == ProcedureState::ProcedureState_Battle)
	{
		UPWProcedureBattle* BattleProcedure = Cast<UPWProcedureBattle>(UPWProcedureManager::GetInstance(ContextObj)->GetProcedure(ProcedureState::ProcedureState_Battle));
		if (BattleProcedure)
		{
			BattleProcedure->UpdateBattleUI();
		}
	}
	else if (UPWProcedureManager::GetInstance(ContextObj)->GetCurState() == ProcedureState::ProcedureState_StandaloneCopy)
	{
		UPWProcedureStandaloneCopy* StandaloneCopyProcedure = Cast<UPWProcedureStandaloneCopy>(UPWProcedureManager::GetInstance(ContextObj)->GetProcedure(ProcedureState::ProcedureState_StandaloneCopy));
		if(StandaloneCopyProcedure)
		{
			StandaloneCopyProcedure->UpdateUI();
		}
	}
}

int32 UPWBlueprintLibrary::GetMapID(UObject* ContextObj)
{
	if (ContextObj == nullptr || ContextObj->GetWorld() == nullptr)
		return -1;
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObj);
	if (GameInstance)
	{
		return GameInstance->GetMapID();
	}
	return -1;
}

EMapType UPWBlueprintLibrary::GetMapType(UObject* ContextObj)
{
	FPWMapConfig Cfg;
	if (GetMapConfig(ContextObj, Cfg))
	{
		return Cfg.MapType;
	}
	return EMapType::EMapType_None;
}

bool UPWBlueprintLibrary::GetMapConfig(UObject* ContextObj, FPWMapConfig& MapConfig)
{
	int32 MapID = UPWBlueprintLibrary::GetMapID(ContextObj);
	if (FPWDataTable::Get().GetItemById<FPWMapConfig>(MapID, MapConfig) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPWBlueprintLibrary::GetMapConfig failed, when ,mapID = %d"), MapID);
		return false;
	}
	return true;
}

EMapSeason UPWBlueprintLibrary::GetSeasonType(UObject* ContextObj)
{
	EMapSeason  SeasonResult = EMapSeason::EMapSeason_None;
	FPWMapConfig MapConfig;
	if (UPWBlueprintLibrary::GetMapConfig(ContextObj, MapConfig))
	{
		SeasonResult = MapConfig.Season;
	}
	return SeasonResult;
}

float UPWBlueprintLibrary::GetBornLocationShowTime(UObject* ContextObj)
{
	APWCharacter* Char = Cast<APWCharacter>(GetLocalPWCharacter(ContextObj));
	if (Char)
	{
		return Char->CharacterData->BornLocationShowTime;
	}
	return -1;
}

void UPWBlueprintLibrary::GetTimeRemain(UObject* WorldContextObject, int EndingTime, bool& bIsExpire, int& Day, int& Hour, int& Minute)
{
	check(0);	//GetServerTime需要在新协议里重新实现
	//int totalSecond = EndingTime - GetNetManager(WorldContextObject)->GetServerTime() / 1000;
	int totalSecond = 0;
	if (totalSecond < 0)
	{
		bIsExpire = true;
	}
	else
	{
		int totalMinute = totalSecond / 60;
		Minute = totalMinute % 60;
		int totalHour = totalMinute / 60;
		Hour = totalHour % 24;
		Day = totalHour / 24;
	}
}

float UPWBlueprintLibrary::GetFightingDeltaTime(UObject* ContextObject)
{
	APWGameState* PWGS = Cast<APWGameState>(ContextObject->GetWorld()->GetGameState());
	if (PWGS)
	{
		if (PWGS->FightingStartTime > 0)
		{
			return PWGS->GetServerWorldTimeSeconds() - PWGS->FightingStartTime;
		}
	}
	return -1.0f;
}

bool UPWBlueprintLibrary::IsArmor(int32 ItemID)
{
	FPWEquipmentItem ItemInfo;
	bool ret = FPWDataTable::Get().GetPWEquipmentItem(ItemID, ItemInfo);
	if (ret)
	{
		return ItemInfo.BodyPart == EBodyPart::Vest || ItemInfo.BodyPart == EBodyPart::Helm;
	}
	return false;
}

float UPWBlueprintLibrary::GetArmorDurabilityPercentage(int32 ItemID, APWPickup* PickupActor)
{
	FPWBattleEquipmentItem ItemInfo;
	bool ret = FPWDataTable::Get().GetPWBattleEquipmentItem(ItemID, ItemInfo);
	APWDroppedEquipment* DroppedEquipment = Cast<APWDroppedEquipment>(PickupActor);
	if (ret && DroppedEquipment)
	{
		return DroppedEquipment->GetDurability() / ItemInfo.Durability;
	}
	return 1.f;
}

static const FString GetShortAreaName()
{
    FString Area = TEXT("na");
    if (AreaName == TEXT("SIEA")) {
        Area = TEXT("na");
    } else if (AreaName == TEXT("SIEE")) {
        Area = TEXT("euro");
    } else if (AreaName == TEXT("SIEJA")) {
        Area = TEXT("asia");
    }
    return Area;
}

void UPWBlueprintLibrary::OpenUserLink(UObject* ContextObject)
{
	FString Url = FString::Printf(TEXT("http://www.det-i.com/%s/launcher/%s/service.html"), *GetShortAreaName(), *GetCultureName());

#if PLATFORM_PS4
	UDHOnlinePS4::GetInstance(ContextObject)->ShowBrowser(Url);
#endif

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Opening User Agreement Link %s"), *Url);
#endif
}

void UPWBlueprintLibrary::OpenPrivateLink(UObject* ContextObject)
{
	FString Url = FString::Printf(TEXT("http://www.det-i.com/%s/launcher/%s/privacy.html"), *GetShortAreaName(), *GetCultureName());

#if PLATFORM_PS4
	UDHOnlinePS4::GetInstance(ContextObject)->ShowBrowser(Url);
#endif

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Opening Privacy Policy Link %s"), *Url);
#endif
}

void UPWBlueprintLibrary::ShowBornOperationUI(UObject* ContextObject)
{
	UPWUIManager* UIManager = UPWUIManager::Get(ContextObject);
	if (UIManager)
	{
		UIManager->ForceShowUI(EUMGID::EUMGID_WW_BornOperation);
		UPWTeachingInImageWidget* BornOperationWidget = Cast<UPWTeachingInImageWidget>(UIManager->GetWidget(EUMGID::EUMGID_WW_BornOperation));
		if (BornOperationWidget)
		{
			BornOperationWidget->SwitchToSettingMode();
		}
	}
}

void UPWBlueprintLibrary::ShowGamePadOperationUI(UObject* ContextObject)
{
	if (UPWUIManager::Get(ContextObject))
	{
		UPWUIManager::Get(ContextObject)->ForceShowUI(EUMGID::EUMGID_WW_GamePadOperation);
	}
}

void UPWBlueprintLibrary::ShowReleaseNotesUI(UObject* ContextObject)
{
    if (UPWUIManager::Get(ContextObject))
    {
        UPWUIManager::Get(ContextObject)->ForceShowUI(EUMGID::EUMGID_ReleaseNotesWidget);
    }
}

FString UPWBlueprintLibrary::GetCultureName()
{
    FString Language = TEXT("en");
    FString Culture = FInternationalization::Get().GetCurrentCulture()->GetName();
    UE_LOG(LogTemp, Display, TEXT("Current culture is %s"), *Culture);
    if (Culture.StartsWith(TEXT("zh-Hans"))) {
        Language = TEXT("zh_hans");
    } else if (Culture.StartsWith(TEXT("zh"))) {
        Language = TEXT("zh_hant");
    } else if (Culture.StartsWith(TEXT("ja"))) {
        Language = TEXT("ja");
    } else {
        Language = TEXT("en");
    }
    return Language;
}

bool UPWBlueprintLibrary::IsEnterCrossNotCircle(UObject* ContextObj)
{
#if PLATFORM_PS4
	return UDHOnlinePS4::GetInstance(ContextObj)->IsEnterCrossNotCircle();
#endif
	return false;
}

void UPWBlueprintLibrary::ShowStandaloneNotice(UObject* ContextObj, int32 ContentID)
{
	UPWStandaloneNoticeWidget* widget = Cast<UPWStandaloneNoticeWidget>(UPWUIManager::Get(ContextObj)->GetWidget(EUMGID::EUMGID_StandaloneNotice));
	if (widget)
	{
		widget->ShowNotice(ContentID);
	}
}

int32 UPWBlueprintLibrary::GetRowCountPerSizeBox(UObject* ContextObj, USizeBox* sizeBox, float rowHeight /*= 0.f*/)
{
	if (!sizeBox || rowHeight == 0)
	{
		return 7;
	}
	FVector4 padding;
	FVector2D scale;
	FVector4 spillOver;

	UWidgetBlueprintLibrary::GetSafeZonePadding(ContextObj, padding, scale, spillOver);
	return FMath::TruncToInt((sizeBox->HeightOverride - (padding.Y + padding.W)) / rowHeight);
}

void UPWBlueprintLibrary::PlayUISound(UObject* ContextObject, USoundBase* NormalSound, UAkAudioEvent* WwiseSound)
{
	if (UPWGameSettings::Get()->bUseWWise && WwiseSound)
	{
		UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
		if (GameInstance)
		{
			UPWWWiseManager* wiseMgr = GameInstance->GetWWiseMananger();
			if (wiseMgr && ContextObject->IsA<AActor>())
			{
				wiseMgr->PWPlayEvent(WwiseSound, nullptr, Cast<AActor>(ContextObject), true, FVector::ZeroVector, FRotator::ZeroRotator);
				//wiseMgr->GameStatic_PlaySoundAtLoc(WwiseSound, FVector(), FRotator(), ContextObject);
			}
		}
	}
	else if (NormalSound)
	{
		UGameplayStatics::PlaySound2D(ContextObject, NormalSound);
	}
	
}

int32 UPWBlueprintLibrary::GetHighestWidgetOrder()
{
	return 1000;
}

FPWGameDataConfig UPWBlueprintLibrary::GetGameDataConfig()
{
	const FPWGameDataConfig* cfg = GetInstanceOfJson<FPWGameDataConfig>();
	if (cfg)
	{
		return *cfg;
	}
	return FPWGameDataConfig();
}

bool UPWBlueprintLibrary::GetGrenadeTypeFromID(int32 ItemID, EGrenadeType& Type)
{
	FPWGrenadeInfo Item;
	if (FPWDataTable::Get().GetItemById<FPWGrenadeInfo>(ItemID, Item))
	{
		Type = Item.GrenadeType;
		return true;
	}
	return false;
}

EPWItemCategory UPWBlueprintLibrary::GetItemCategory(int32 ItemID)
{
	EPWItemCategory ret = EPWItemCategory::None;

	FPWItem Item;
	if (FPWDataTable::Get().GetItemById<FPWItem>(ItemID, Item))
	{
		ret = Item.Category;
	}

	return ret;
}

int32 UPWBlueprintLibrary::GetItemZOrder(int32 ItemID)
{
	int32 ZOrder = INDEX_NONE;

	FPWItem Item;
	if (FPWDataTable::Get().GetItemById<FPWItem>(ItemID, Item))
	{
		ZOrder = Item.ZOrder;
	}

	return ZOrder;
}

bool UPWBlueprintLibrary::GetUIStyle(FPWUIStyleConfig& OutConfig)
{
	static TArray<FPWUIStyleConfig> ConfigList;

	if (ConfigList.IsValidIndex(0))
	{
		OutConfig = ConfigList[0];
		return true;
	}

	const FPWUIStyleConfig* cfg = GetInstanceOfJson<FPWUIStyleConfig>();
	if (cfg)
	{
		OutConfig = *cfg;
#if !WITH_EDITOR
		ConfigList.Add(*cfg);
#endif
		return true;
	}
	return false;
	/*FString DirPath = FPaths::ProjectContentDir() + FString("Datatables/Tables/Config/");
	FString FilePath = DirPath + FString("UIStyle.json");

	FString OutFileContent;
	FFileHelper::LoadFileToString(OutFileContent, *FilePath);

	if (DHJsonUtils::FromJson(OutFileContent, &OutConfig))
	{
	ConfigList.Add(OutConfig);
	return true;
	}*/
}

TArray<FVector> UPWBlueprintLibrary::GetAirlineData(UObject* ContextObject)
{
	return UPWAirForteManager::Get(ContextObject)->GetAirlineData();
}

void UPWBlueprintLibrary::SetWWiseSoundVolme(UObject* ContextObject, const EWWiseSonundVolumeType SoundType, const float Volume)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	return_if_null(GameInstance);
	UPWWWiseManager* wiseMgr = GameInstance->GetWWiseMananger();
	return_if_null(wiseMgr);
	FName CurrentTypeName = wiseMgr->GetWWiseAudioName(SoundType);
	UAkGameplayStatics::SetRTPCValue(CurrentTypeName, Volume*100.0f, 0.0f, nullptr);
}

TArray<FCharacterSlimInfor> UPWBlueprintLibrary::GetCharacterSlimInfor(UObject* ContextObject)
{
	TArray<FCharacterSlimInfor> CharacterSlimList;
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return CharacterSlimList;
	}
	APWGameState* PWGS = Cast<APWGameState>(ContextObject->GetWorld()->GetGameState());
	if (PWGS)
	{
		CharacterSlimList = PWGS->GetAllCharacterSlimInfor();
		CharacterSlimList.Sort([](const FCharacterSlimInfor& LHS, const FCharacterSlimInfor& RHS) {return ((int32)LHS.CharacterType) > (int32)RHS.CharacterType; });
	}
	return CharacterSlimList;
}

void UPWBlueprintLibrary::PlayMovePageSound(UObject* ContextObject, int32 ChangedVal)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		if (ChangedVal == L1SOUND)
		{
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_L1_Normal);
		}
		else if (ChangedVal == R1SOUND)
		{
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_R1_Normal);
		}
	}
}

void UPWBlueprintLibrary::PlaySoundByUIAudioType(UObject* ContextObject, EWwiseUIAudioType type)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(type);
	}
}

void UPWBlueprintLibrary::PlayClosePageSound(UObject* ContextObject)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_RightDown_Normal);
	}
}

void UPWBlueprintLibrary::PlayConfirmSound(UObject* ContextObject)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_RightRight_Normal);
	}
}

void UPWBlueprintLibrary::PlaySoundWhenActionUpPressed(UObject* ContextObject)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_RightUp_Normal);
	}
}

void UPWBlueprintLibrary::PlaySoundWhenActionLeftPressed(UObject* ContextObject)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_RightLeft_Normal);
	}
}

void UPWBlueprintLibrary::PlaySlideLeftRightSound(UObject* ContextObject, bool IsLeft)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		if (IsLeft)
		{
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuLeft_Normal);
		}
		else
		{
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuRight_Normal);
		}
	}
}
void UPWBlueprintLibrary::PlaySlideUpDownSound(UObject* ContextObject, bool IsUp)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		if (IsUp)
		{
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuUp_Normal);
		}
		else
		{
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuDown_Normal);
		}
	}
}

void UPWBlueprintLibrary::PlaySlideSelectedSound(UObject* ContextObject, EPWSelectedDirection InDirection)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}
	
	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		switch (InDirection)
		{
		case EPWSelectedDirection::Down:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuDown_Normal);
			break;
		case EPWSelectedDirection::Up:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuUp_Normal);
			break;
		case EPWSelectedDirection::Right:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuRight_Normal);
			break;
		case EPWSelectedDirection::Left:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_MenuLeft_Normal);
			break;
		}
	}
}

void UPWBlueprintLibrary::PlaySoundAboutLRPressed(UObject* ContextObject, EPWAboutLRButtonType Type)
{
	if (ContextObject == nullptr || ContextObject->GetWorld() == nullptr)
	{
		return;
	}

	if (UPWGameSettings::Get() && UPWGameSettings::Get()->bUseWWise)
	{
		switch (Type)
		{
		case EPWAboutLRButtonType::L1:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_L1_Normal);
			break;
		case EPWAboutLRButtonType::L2:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_L2_Normal);
			break;
		case EPWAboutLRButtonType::R1:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_R1_Normal);
			break;
		case EPWAboutLRButtonType::R2:
			UPWWWiseManager::Get(ContextObject)->PlayUIWwiseAudio(EWwiseUIAudioType::EWUI_R2_Normal);
			break;
		}
	}
}

FPWNetSlimInfor UPWBlueprintLibrary::GetNetInfor(UObject* ContextObject)
{
	FPWNetSlimInfor OutSlimInfor;
	UWorld* world = ContextObject->GetWorld();
	if (!world)
	{
		return OutSlimInfor;
	}
	APlayerController * PlayerController = world->GetFirstPlayerController();
	if (PlayerController)
	{
		UNetConnection* NetConnection = PlayerController->GetNetConnection();
		if (NetConnection)
		{
			OutSlimInfor.PingValue = (int32)(NetConnection->AvgLag * 1000.0f);

			OutSlimInfor.DownloadDataInfor = FPWNetSlimData((float)NetConnection->InBytesPerSecond, NetConnection->InPacketsPerSecond, NetConnection->InPacketsLost);
			OutSlimInfor.UploadDataInfor = FPWNetSlimData((float)NetConnection->OutBytesPerSecond, NetConnection->OutPacketsPerSecond, NetConnection->OutPacketsLost);
		}
	}
	return OutSlimInfor;
}

UPWSaveGameManager* UPWBlueprintLibrary::GetSaveGameManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetSaveGameManager();
	return nullptr;
}

UPWMailManager* UPWBlueprintLibrary::GetMailManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetMailManager();
	return nullptr;
}

UPWShopManager* UPWBlueprintLibrary::GetShopManager(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetShopManager();
	return nullptr;
}

UPWTrophyMananger* UPWBlueprintLibrary::GetTrophyMananger(UObject* ContextObject)
{
	UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	if (GameInstance)
		return GameInstance->GetTrophyMananger();
	return nullptr;
}

APWCharacterBase* UPWBlueprintLibrary::GetLocalPWCharacter(const UObject* ContextObject)
{
	APWCharacterData_Runtime* PWPC = GetLocalRuntimeData(ContextObject);
	if (PWPC)
	{
		return PWPC->GetCharacter();
	}
	return nullptr;
}

APWPlayerControllerBase* UPWBlueprintLibrary::GetLocalPWController(const UObject* ContextObj)
{
    if (ContextObj)
    {
        UWorld* world = ContextObj->GetWorld();
        if (world && (!world->IsServer() || world->GetNetMode() == ENetMode::NM_Standalone))
        {
            return Cast<APWPlayerControllerBase>(world->GetFirstPlayerController());
        }
    }
    return nullptr;
}

APWCharacterData_Runtime* UPWBlueprintLibrary::GetLocalRuntimeData(const UObject* ContextObj)
{
	if (ContextObj)
	{
		UWorld* world = ContextObj->GetWorld();
		if (world && (!world->IsServer() || world->GetNetMode() == ENetMode::NM_Standalone))
		{
			APWPlayerControllerBase* PWPCBase = Cast<APWPlayerControllerBase>(world->GetFirstPlayerController());
			if (PWPCBase)
			{
				return PWPCBase->GetRuntimeData();
			}
		}
	}
	return nullptr;
}

APWCharacterData_Runtime* UPWBlueprintLibrary::GetViewedRuntimeData(const UObject* ContextObject)
{
    APWCharacterData_Runtime* LocalRuntime = GetLocalRuntimeData(ContextObject);
    if (!LocalRuntime)
    {
        return nullptr;
    }
    return LocalRuntime->GetCurrentViewTarget_RuntimeData();
}

UPWInventoryComponent* UPWBlueprintLibrary::GetLocalInventory(UObject* ContextObject)
{
	APWCharacterData_Runtime* RuntimeData = GetLocalRuntimeData(ContextObject);
	if (RuntimeData)
	{
		APWCharacterBase* PWCharacter = RuntimeData->GetCharacter();
		if (PWCharacter)
		{
			return PWCharacter->GetInventory();
		}
	}
	return nullptr;
}

bool UPWBlueprintLibrary::IsKillcamWorldObject(const UObject* ContextObj)
{
	if (ContextObj == nullptr)
		return false;
	return UPWGameInstance::Get(ContextObj)->IsKillcamWorld(ContextObj->GetWorld());
}

int32 UPWBlueprintLibrary::GetStringLengthUTF8(FString str)
{
	TStringConversion<FTCHARToUTF8_Convert> conversion(*str);
	return conversion.Length();
}

FPWItem UPWBlueprintLibrary::GetItemDataByID(int32 ItemID)
{
	FPWItem item;
	FPWDataTable::Get().GetItemById<FPWItem>(ItemID, item);
	return item;
}

FPWStorageItem UPWBlueprintLibrary::GetStorageItemDataByID(int32 ItemID)
{
	const FPWStorageItem* ItemPtr = FPWDataTable::Get().GetStorageItemPtrById(ItemID);
	if (ItemPtr)
		return *ItemPtr;
	return FPWStorageItem();
}

PWGAME_API bool PWLIB::GetAllFileNames(const FString FolderPath, TArray<FName>& NameList, bool bShortName)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	// Need to do this if running in the editor with -game to make sure that the assets in the following path are available
	TArray<FString> PathsToScan;
	PathsToScan.Add(FolderPath);
	AssetRegistry.ScanPathsSynchronous(PathsToScan);
	TArray<FAssetData> DataTableAssets;
	AssetRegistry.GetAssetsByPath(FName(*FolderPath), DataTableAssets, true, false);
	if (DataTableAssets.Num() > 0)
	{
		if (bShortName)
		{	// only file name.
			for (FAssetData AssetData : DataTableAssets)
			{
				NameList.Add(AssetData.AssetName);
			}
		}
		else
		{	// name include file path.
			for (FAssetData AssetData : DataTableAssets)
			{
				NameList.AddUnique(AssetData.PackageName);
			}
		}
		return true;
	}
	return false;
}

ECharacterType PWLIB::GetLocalCharacterType()
{
	check(GetWorld());
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("PWLIB::GetLocalCharacterType shoulde not be called on server."));
		check(false);
		return ECharacterType::CT_Max;
	}

	if (GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		ensure(LocalPlayer != nullptr);
		UGameInstance* GameInstance = LocalPlayer->GetGameInstance();
		if (GameInstance != nullptr)
		{
			APWPlayerControllerBase* PWPlayerControlerBase = Cast<APWPlayerControllerBase>(GameInstance->GetFirstLocalPlayerController());
			if (PWPlayerControlerBase && PWPlayerControlerBase->GetCurControlledCharacter())
			{
				ECharacterType  CharcterType = PWPlayerControlerBase->GetCurControlledCharacter()->GetCharacterType();
#if WITH_EDITOR
				FString StrCharacterType = PWLIB::GetEnumValueAsString(TEXT("ECharacterType"), (int32)CharcterType);
				//UE_LOG(LogTemp, Log, TEXT("PWLIB::GetLocalCharacterType  character type %s."), *StrCharacterType);
#endif
				return CharcterType;
			}
		}
	}
	return ECharacterType::CT_Max;
}

EPWPlayerCareerType PWLIB::GetLocalCareerType()
{
	check(GetWorld());
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("PWLIB::GetLocalCareerType shoulde not be called on server."));
		check(false);
		return EPWPlayerCareerType::CPPCT_None;
	}

	if (GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		ensure(LocalPlayer != nullptr);
		UGameInstance* GameInstance = LocalPlayer->GetGameInstance();
		if (GameInstance != nullptr)
		{
			APWPlayerControllerBase* PWPlayerControlerBase = Cast<APWPlayerControllerBase>(GameInstance->GetFirstLocalPlayerController());
			if (PWPlayerControlerBase && PWPlayerControlerBase->GetCurControlledCharacter())
			{
				EPWPlayerCareerType  CareerType = PWPlayerControlerBase->GetCurControlledCharacter()->GetPlayerCareeerType();
#if WITH_EDITOR
				FString StrCareerType = PWLIB::GetEnumValueAsString(TEXT("EPWPlayerCareerType"), (int32)CareerType);
				//UE_LOG(LogTemp, Log, TEXT("PWLIB::GetLocalCareerType  Careeer type %s."), *StrCareerType);
#endif
				return CareerType;
			}
		}
	}
	return EPWPlayerCareerType::CPPCT_None;
}

void PWLIB::SetActorCloseToGround(AActor *ActorShouldIgnore, AActor* Actor, const FVector& Location, const FRotator& Rotation)
{
	const float MaxAngle = 75.0f;
	return_if_true(Actor == nullptr);
	// Only works for urn mesh actor
	FHitResult HitResult;
	FHitResult HitResult2;
	FHitResult HitResult3;

	FVector Start = Location;
	FVector Start2 = Location + FVector::ForwardVector * 10.f;
	FVector Start3 = Location + FVector::RightVector * 10.f;

	FVector Direction = -FVector::UpVector;
	float MaxHeight = 4000.f;
	FVector End = Start + (Direction * MaxHeight);
	FVector End2 = Start2 + (Direction * MaxHeight);
	FVector End3 = Start3 + (Direction * MaxHeight);

	TArray<AActor*> ActorsToIgnore;
	if (ActorShouldIgnore)
		ActorsToIgnore.AddUnique(ActorShouldIgnore);

	TArray<TEnumAsByte<EObjectTypeQuery> >ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Vehicle));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible));
	TArray<AActor*> ActorsToIgnore_Sphere;
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(Actor, Start, MaxHeight, ObjectTypes, AActor::StaticClass(), ActorsToIgnore_Sphere, OutActors);
	for (AActor* Ignores : OutActors)
	{
		ActorsToIgnore.AddUnique(Ignores);
	}

	bool bOnGround = UKismetSystemLibrary::LineTraceSingle(Actor, Start, End, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	bool bOnGround2 = UKismetSystemLibrary::LineTraceSingle(Actor, Start2, End2, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult2, true);
	bool bOnGround3 = UKismetSystemLibrary::LineTraceSingle(Actor, Start3, End3, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult3, true);

	if (bOnGround)
	{
		Actor->SetActorLocation(HitResult.Location);
	}

	if (bOnGround2 && bOnGround3)
	{
		FVector Vec1, Vec2;
		Vec1 = HitResult.Location - HitResult2.Location;
		Vec2 = HitResult.Location - HitResult3.Location;

		FVector PlaneNormal = FVector::CrossProduct(Vec1, Vec2);
		PlaneNormal.Normalize();

		FVector RotateAxis = FVector::CrossProduct(FVector::UpVector, PlaneNormal);
		RotateAxis.Normalize();

		float Angle = FMath::Acos(FVector::DotProduct(FVector::UpVector, PlaneNormal));
		float CurrentAngle = FMath::RadiansToDegrees(Angle);
#if WITH_EDITOR
		//UE_LOG(LogTemp, Log, TEXT("PWLIB::SetActorCloseToGround %f"), CurrentAngle);
#endif
		if (CurrentAngle <= MaxAngle)
		{
			FQuat DeltaRot = FQuat(RotateAxis, Angle);
			Actor->SetActorRotation(DeltaRot);
		}
		Actor->AddActorLocalRotation(Rotation);
	}
}

FText PWLIB::GetLocTextFromBodyPart(EBodyPart BodyPart)
{
	FText BodyPartName;
	switch (BodyPart)
	{
	case EBodyPart::Chest_Shirt:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PT_TOP");
		break;
	case EBodyPart::Hands_Gloves:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_GLVES");
		break;
	case EBodyPart::Coat:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_COAT");
		break;
	case EBodyPart::Pants:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_PANTS");
		break;
	case EBodyPart::Leggings:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_SOCKS");
		break;
	case EBodyPart::Feet_Shoes:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_SHOES");
		break;
	case EBodyPart::Hat:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_HAT");
		break;
	case EBodyPart::Beard:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_MASK");
		break;
	case EBodyPart::Glasses:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_GLASSES");
		break;
	case EBodyPart::Necklace:
		BodyPartName = LOCTABLE("Lang", "GL_CL_PART_SCARF");
		break;
	case EBodyPart::Hair:
	case EBodyPart::Head:
	case EBodyPart::Earrings:
	case EBodyPart::Watch_Bracelet:
	case EBodyPart::Tatto:
	case EBodyPart::Helm:
	case EBodyPart::Vest:
	case EBodyPart::Backpack:
	case EBodyPart::Race_Skin:
	case EBodyPart::Painting:
	case EBodyPart::BehaivorController:
	case EBodyPart::Belt:
	case EBodyPart::MAX:
	default:
		break;
	}
	return BodyPartName;
}


FLinearColor UPWBlueprintLibrary::GetQualityLinearColor(EItemQuality Quality)
{
	FPWUIStyleConfig UIStyleConfig;
	if (UPWBlueprintLibrary::GetUIStyle(UIStyleConfig) && UIStyleConfig.ItemQualityColor.IsValidIndex((int32)Quality))
	{
		return FLinearColor(FColor::FromHex(UIStyleConfig.ItemQualityColor[(int32)Quality]));
	}
	return FLinearColor(0, 0, 0, 0);
}

FText UPWBlueprintLibrary::GetInteractTopTip(UObject* ContextObject, const EInteractions Action)
{
	FText Tip;
//	UPWUIManager* UIManager = GetUIManager(ContextObject);
	if (/*UIManager && */IsShowWidgetByType(ContextObject,EPWSaveTabPageType::PSTPT_Tutorial, EPWSGType::EPWSGType_InteractionVis))
	{
		auto TipListDT = DHInteractTopTipsDT::GetInstance();
		if (!TipListDT || TipListDT->IsEmpty() || Action == EInteractions::None)
		{
			return Tip;
		}
		else
		{
			const FDHInteractTipsStruct* InteractdTip = TipListDT->GetRowByKey(EInteractionTopTips(Action));
			if (InteractdTip)
			{
				return InteractdTip->Content;
			}
		}
	}
	return Tip;
}

bool UPWBlueprintLibrary::IsShowWidgetByType(UObject* ContextObject, const EPWSaveTabPageType TabPageType, const EPWSGType Type)
{
	return_false_if_true(EPWSaveTabPageType::PSTPT_None == TabPageType || EPWSGType::EPWSGType_None == Type);
	return_false_if_null(UPWSaveGameManager::Get(ContextObject));
	UPWSGBase* PWSGBaseTabPage = UPWSaveGameManager::Get(ContextObject)->GetPWSGTabPage(TabPageType);
	if (PWSGBaseTabPage && PWSGBaseTabPage->IsValidLowLevel())
	{
		return PWSGBaseTabPage->GetBoolValueByType(Type);
	}
	return false;
}

int32 UPWBlueprintLibrary::GetAmmoItemIDByType(EAmmoType Type)
{
	TArray<FPWAmmoItem> AmmoItemArray;
	FPWDataTable::Get().GetDataByDataType<FPWAmmoItem>(AmmoItemArray);

	for (auto AmmoDT : AmmoItemArray)
	{
		if (Type == AmmoDT.AmmoType)
		{
			return AmmoDT.ItemID;
		}
	}
	return INDEX_NONE;
}


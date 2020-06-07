// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintSetLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/AssetManager.h"
#include "UObject/SoftObjectPath.h"
bool UMyBlueprintSetLibrary::GetFloatByName(UObject* Target, FName VarName, float& OutFloat)
{
	if (Target)
	{
		float FoundFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(),VarName);
		if (FloatProp)
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			OutFloat = FoundFloat;
			return true;
		}
	}
	return false;
}

bool UMyBlueprintSetLibrary::GetIntByName(UObject* Target, FName VarName, int32& OutInt)
{
	if (Target)
	{
		int32 FoundInt = INDEX_NONE;
		UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			OutInt= FoundInt;
			return true;
		}
	}
	return false;
}

bool UMyBlueprintSetLibrary::SetIntByName(UObject* Target, FName VarName, int32 NewValue, int32& OutInt)
{
	if (Target)
	{
		int32 FoundInt = INDEX_NONE;
		UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			IntProp->SetPropertyValue_InContainer(Target,NewValue);
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			OutInt = FoundInt;
			return true;
		}
	}
	return false;
}

bool UMyBlueprintSetLibrary::SetFloatByName(UObject* Target, FName VarName, float NewValue, float& OutFloat)
{
	if (Target)
	{
		float FoundFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FloatProp->SetPropertyValue_InContainer(Target, NewValue);
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			OutFloat = FoundFloat;
			return true;
		}
	}
	return false;
}

bool UMyBlueprintSetLibrary::IncreaseFloatByName(UObject* Target, FName VarName, float NewValueToAdd, float& OutFloat)
{
	if (Target)
	{
		float FoundFloat,IncreasedFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			IncreasedFloat = FoundFloat + NewValueToAdd;
			FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat);
			OutFloat = IncreasedFloat;
			return true;
		}
	}
	return false;
}

bool UMyBlueprintSetLibrary::IncreaseIntByName(UObject* Target, FName VarName, int32 NewValueToAdd, int32& OutInt)
{
	if (Target)
	{
		int32 FoundInt, IncreasedInt;
		UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			IncreasedInt = FoundInt + NewValueToAdd;
			IntProp->SetPropertyValue_InContainer(Target, IncreasedInt);
			OutInt = IncreasedInt;
			return true;
		}
	}
	return false;
}

FText UMyBlueprintSetLibrary::GetEnumValueAsDisplayName(const FString& EnumName, int32 Inindex)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!enumPtr)
	{
		return FText::FromString(TEXT("Invalid"));
	}

	return enumPtr->GetDisplayNameTextByIndex(Inindex);
}

FString UMyBlueprintSetLibrary::GetEnumValueAsString(const FString& EnumName, int32 Inindex)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetNameStringByIndex(Inindex);
}

void UMyBlueprintSetLibrary::LoadAssetASync(TArray<FStringAssetReference> AssetArr, FStreamableDelegate Del)
{
	FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
	AssetLoader.RequestAsyncLoad(AssetArr,Del);
}

UClass* UMyBlueprintSetLibrary::LoadBPFromPathName(FString PathName)
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

UClass* UMyBlueprintSetLibrary::LoadBPFromPathName(FStringAssetReference StringAssetRef)
{
	UClass* ret = nullptr;
	ret = LoadBPFromPathName(StringAssetRef.ToString());

	return ret;
}

void UMyBlueprintSetLibrary::UnLoadBPFromPathName(FString PathName)
{
	FStringAssetReference refPath = PathName;
	auto* Obj = refPath.ResolveObject();
	FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
	AssetLoader.Unload(refPath);
}

void UMyBlueprintSetLibrary::UnLoadBPFromPathName(FStringAssetReference StringAssetRef)
{
	FStringAssetReference refPath = StringAssetRef;
	auto* Obj = refPath.ResolveObject();
	FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
	AssetLoader.Unload(refPath);
}

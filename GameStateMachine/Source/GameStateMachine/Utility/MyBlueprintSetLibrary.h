// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintSetLibrary.h"
#include "Engine/StreamableManager.h"
#include "Runtime/Engine/Classes/Engine/AssetManager.h"
#include "MyBlueprintSetLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMESTATEMACHINE_API UMyBlueprintSetLibrary : public UBlueprintSetLibrary
{
	GENERATED_BODY()

    UFUNCTION(BlueprintCallable,Category = "Variables")
    static bool GetFloatByName(UObject* Target,FName VarName,float &OutFloat);

	UFUNCTION(BlueprintCallable, Category = "Variables")
	static bool GetIntByName(UObject* Target, FName VarName, int32& OutInt);
    
	UFUNCTION(BlueprintCallable, Category = "Variables")
		static bool SetIntByName(UObject* Target, FName VarName,int32 NewValue,int32& OutInt);

	UFUNCTION(BlueprintCallable, Category = "Variables")
		static bool SetFloatByName(UObject* Target, FName VarName, float NewValue,float& OutFloat);


	UFUNCTION(BlueprintCallable, Category = "Variables")
		static bool IncreaseFloatByName(UObject* Target, FName VarName, float NewValueToAdd, float& OutFloat);

	UFUNCTION(BlueprintCallable, Category = "Variables")
		static bool IncreaseIntByName(UObject* Target, FName VarName, int32 NewValueToAdd, int32& OutInt);

	UFUNCTION(BlueprintCallable, Category = "Variables")
		static FText GetEnumValueAsDisplayName(const FString& EnumName,int32 Inindex);

	UFUNCTION(BlueprintCallable, Category = "Variables")
		static FString GetEnumValueAsString(const FString& EnumName, int32 Inindex);

	/************************************************************************/
	/* Load Asset                                                           */
	/************************************************************************/
	//UFUNCTION(BlueprintCallable, Category = "LoadAsset")
	static void LoadAssetASync(TArray<FStringAssetReference> AssetArr,FStreamableDelegate Del);
	static UClass* LoadBPFromPathName(FString PathName);
	static UClass* LoadBPFromPathName(FStringAssetReference StringAssetRef);

	static void UnLoadBPFromPathName(FString PathName);
	static void UnLoadBPFromPathName(FStringAssetReference StringAssetRef);

	//UFUNCTION(BlueprintCallable, Category = "LoadAsset")
	/*template <typename Cls>
	static UClass* FindClassOfName(FString ClassName, Cls cls,FString PathToLoadIfNotFind = "");*/
	template <typename Cls>
	UClass* FindClassOfName(FString ClassName, Cls cls, FString PathToLoadIfNotFind = "");
	/************************************************************************/
	/* Set Timer by Handle                                                  */
	/************************************************************************/
	#define TP_SETTIMER_LOOP(handle,FunctionName,InRate) TP_SETTIME(handle,FunctionName,InRate,true)
	#define TP_SETTIMER_UNLOOP(handle,FunctionName,InRate) TP_SETTIME(handle,FunctionName,InRate,false)

	#define TP_SETTIME(handle,FunctionName,InRate,bLoop)\
	if (!GetWorldTimerManager().IsTimerActive(handle) && (InRate) >= 0.0f)\
	{\
		GetWorldTimerManager().SetTimer(handle,this,&ThisClass::FunctionName,InRate,bLoop);\
	}
	#define  TP_CLEARTIMER(handle) (GetWorldTimerManager().ClearTimer(handle))
	#define  TP_ISTIMERaCTIVE(handle) (GetWorldTimerManager().IsTimerActive(handle))

	template <typename Cls>
	UClass* UMyBlueprintSetLibrary::FindClassOfName(FString ClassName, Cls cls, FString PathToLoadIfNotFind)
	{
		UClass* Result = nullptr;

		Result = FindObject<UClass>(ANY_PACKAGE, *ClassName);
		if (!Result)
		{
			FString Path = PathToLoadIfNotFind + "." + ClassName;
			Result = StaticLoadClass(cls::StaticClass(), NULL, *Path, NULL, LOAD_None, NULL);
		}

		return Result;
	}
};

// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "PWDelegateManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputButtonRepeat, FName, ActionInputName);		//长按

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputButtonDoubleRepeat, FName, ActionInputName); //双击的第二下长按

#define  NEWORGETMANAGER_CPP(ClassName,VariableName)\
if (VariableName == nullptr)\
VariableName = NewObject<ClassName>(this);\
return VariableName;



/**
 *
 */
UCLASS(BlueprintType)
class PWGAME_API UPWDelegateManager : public UObject
{
	GENERATED_BODY()
public:
	static UPWDelegateManager* Get(UObject* ContextObject);

	UPROPERTY(BlueprintAssignable, Category = "Battle Client")
		FOnInputButtonRepeat OnInputButtonRepeat;

	UPROPERTY(BlueprintAssignable, Category = "Battle Client")
		FOnInputButtonDoubleRepeat OnInputButtonDoubleRepeat;
};



UPWDelegateManager* UPWDelegateManager::Get(UObject* ContextObject)
{
	//UPWGameInstance* GameInstance = UPWGameInstance::Get(ContextObject);
	//if (GameInstance)
	//{
	//	return GameInstance->GetDelegateManager();
	//}

	//NEWORGETMANAGER_CPP(UPWDelegateManager, DelegateManager);


	//TODO  need to new a singlelone;
	return nullptr;
}
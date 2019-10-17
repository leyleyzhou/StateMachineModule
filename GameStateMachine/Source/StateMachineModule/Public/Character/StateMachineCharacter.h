// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMMCommon.h"
#include "StateMachineCharacter.generated.h"


UCLASS()
class STATEMACHINEMODULE_API AStateMachineCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStateMachineCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//RegisterCommand   And  UnregisterCommand
	template<typename Cls>
	void RegisterCommand(const FName& Cmd,Cls* Obj,void(Cls::*functor)());
	template<typename Cls>
	void UnregisterCommand(const FName& Cmd,Cls* Obj,void(Cls::*functor)());

	template<typename Cls, typename T1>
	void RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1));
	template<typename Cls, typename T1>
	void UnregisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1));

	template<typename Cls, typename T1, typename T2>
	void RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2));
	template<typename Cls, typename T1, typename T2>
	void UnregisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2));

	template<typename Cls, typename T1, typename T2, typename T3>
	void RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2, T3 p3));
	template<typename Cls, typename T1, typename T2, typename T3>
	void UnregisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2, T3 p3));
	/*----------------------------------------------------------------------------------------------------------------------*/
	//RegisterBlueprintFunc   And  UnregisterBlueprintFunc 
	template<typename Cls>
	void RegisterBlueprintCommand(const FName& Cmd, Cls* Obj, const FName& FuncName);
	template<typename Cls>
	void UnregisterBlueprintCommand(const FName& Cmd, Cls* Obj, const FName& FuncName);
	/*----------------------------------------------------------------------------------------------------------------------*/
	//DoCommandFunc
	UFUNCTION(BlueprintCallable)
	bool CharacterDoCommand(FName Cmd);

	template<typename T1>
	bool CharacterDoCommand(FName Cmd, T1 Param1);

	template<typename T1, typename T2>
	bool CharacterDoCommand(FName Cmd, T1 Param1, T2 Param2);

	template<typename T1, typename T2, typename T3>
	bool CharacterDoCommand(FName Cmd, T1 Param1, T2 Param2, T3 Param3);

	//设置AbilityCommand的转换规则
	UFUNCTION(BlueprintCallable)
	void SetCommandConversion(FName SourceCommand, FName TargetCommand, bool DelayExc = false, bool CacheEveryExecute = false);
	UFUNCTION(BlueprintCallable)
	void RemoveCommandConversion(FName SourceCommand, FName TargetCommand);

	/*----------------------------------------------------------------------------------------------------------------------*/

private:
	bool ApplyCommandConversion(const FName& CmdName, const FString& ParamSignature, const TArray<uint8>& Params);
	bool DoCommandInternal(const FString& FuncSignature, const TArray<uint8>& Params);

private:
	TMap<FName, TArray<CommandFunctionBase*> > CommandFunctions;

	TArray<CommandConversion> CommandConversions;
};

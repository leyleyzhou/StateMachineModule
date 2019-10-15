// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StateMachineCharacter.generated.h"

#pragma region ExtractParmaType

template <typename T>
class RemoveReferenceTypeUtil
{
public:
	typedef T Type;
};

template <typename T>
class RemoveReferenceTypeUtil<const T&>
{
public:
	typedef T Type;
};

template<typename T>
class RemoveReferenceTypeUtil<T&>
{
public:
	typedef T Type;
};

#pragma endregion

#pragma  region CommandFunctionDefine
class CommandFunctionBase
{
public:
	CommandFunctionBase() : bBlueprintFunc(false) {}
	virtual ~CommandFunctionBase() {}
	
	virtual void DoCommand(const TArray<uint8>& ParamData) {}
	bool bBlueprintFunc = false;
};

template<typename Cls>
class CommandFuncParam0 : public CommandFunctionBase
{
public:
	typedef void(Cls::*FunctionType) (void);
	Cls* Obj;
	FunctionType Functor;

	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		(Obj->*Functor)();
	}
};

template<typename Cls,typename T1>
class CommandFuncParam1 : public CommandFunctionBase
{
public:
	typedef void(Cls::*FunctionType) (T1 Param1);
	Cls* Obj;
	FunctionType Functor;
	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		int32 DataOffset = 0;
		typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
		const T1NoRefType& Param1 = *((const T1NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);
		
		check(DataOffset == ParamData.Num());
		if (DataOffset == ParamData.Num())
		{
			(Obj->*Functor)(Param1);
		}
	}
};
#pragma endregion


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
};

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
class CommandBlueprintFunction : public CommandFunctionBase
{
public:
	Cls* Obj;
	FName FuncName;

	CommandBlueprintFunction()
	{
		bBlueprintFunc = true;
	}

	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		UFunction* pFindFunc = Obj->FindFunction(FuncName);
		if (pFindFunc)
		{
			if (pFindFunc->ParmsSize == ParamData.Num())
			{
				Obj->ProcessEvent(pFindFunc, (void*)ParamData.GetData());
			}
			else
			{
				check(false);
			}
		}
	}
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

template<typename Cls,typename T1,typename T2>
class CommandFuncParam2 :public CommandFunctionBase
{
public:
	typedef void(Cls::* FunctionType) (T1 Param1, T2 Param2);
	Cls* Obj;
	FunctionType Functor;
	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		int32 DataOffset = 0;
		typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
		const T1NoRefType& Parma1 = *((const T1NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		typedef typename RemoveReferenceTypeUtil<T2>::Type T2NoRefType;
		const T2NoRefType& Parma1 = *((const T2NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		check(DataOffset == ParamData.Num());
		if (DataOffset == ParamData.Num())
		{
			(Obj->*Functor)(Param1, Param2);
		}
	}

};

template<typename Cls, typename T1, typename T2, typename T3>
class CommandFuncParam3 :public CommandFunctionBase
{
public:
	typedef void(Cls::* FunctionType) (T1 Param1, T2 Param2, T3 Param3);
	Cls* Obj;
	FunctionType Functor;
	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		int32 DataOffset = 0;
		typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
		const T1NoRefType& Parma1 = *((const T1NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		typedef typename RemoveReferenceTypeUtil<T2>::Type T2NoRefType;
		const T2NoRefType& Parma1 = *((const T2NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		typedef typename RemoveReferenceTypeUtil<T3>::Type T3NoRefType;
		const T3NoRefType& Parma1 = *((const T3NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T3NoRefType);

		check(DataOffset == ParamData.Num());
		if (DataOffset == ParamData.Num())
		{
			(Obj->*Functor)(Param1, Param2, Param3);
		}
	}

};
#pragma endregion

#pragma region ParamSignature

template<typename T>
class ParamSignatureString
{
public:

};

template<typename T>
class ParamSignatureString<T*>
{
public:
	static FString GetParamSig()
	{
		return FString(TEXT("Ptr")) + ParamSignatureString<T>::GetParamSig();
	}
};

template<typename T>
class ParamSignatureString<T&>
{
public:
	static FString GetParamSig()
	{
		return ParamSignatureString<T>::GetParamSig(); 
	}
};

template<typename T>
class ParamSignatureString<const T>
{
public:
	static FString GetParamSig()
	{
		return FString(TEXT("Cst")) + ParamSignatureString<T>::GetParamSig();
	}
};

template<> class ParamSignatureString<int64> { public: static FString GetParamSig() { return TNameOf<int64>::GetName(); } };
template<> class ParamSignatureString<int32> { public: static FString GetParamSig() { return TNameOf<int32>::GetName(); } };
template<> class ParamSignatureString<int16> { public: static FString GetParamSig() { return TNameOf<int16>::GetName(); } };
template<> class ParamSignatureString<int8> { public: static FString GetParamSig() { return TNameOf<int8>::GetName(); } };
template<> class ParamSignatureString<uint64> { public: static FString GetParamSig() { return TNameOf<uint64>::GetName(); } };
template<> class ParamSignatureString<uint32> { public: static FString GetParamSig() { return TNameOf<uint32>::GetName(); } };
template<> class ParamSignatureString<uint16> { public: static FString GetParamSig() { return TNameOf<uint16>::GetName(); } };
template<> class ParamSignatureString<uint8> { public: static FString GetParamSig() { return TNameOf<uint8>::GetName(); } };
template<> class ParamSignatureString<float> { public: static FString GetParamSig() { return TNameOf<float>::GetName(); } };
template<> class ParamSignatureString<double> { public: static FString GetParamSig() { return TNameOf<double>::GetName(); } };
template<> class ParamSignatureString<FVector> { public: static FString GetParamSig() { return TEXT("FVector"); } };
template<> class ParamSignatureString<FRotator> { public: static FString GetParamSig() { return TEXT("FRotator"); } };
template<> class ParamSignatureString<bool> { public: static FString GetParamSig() { return TEXT("bool"); } };

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

	//RegisterBlueprintFunc   And  UnregisterBlueprintFunc 
	template<typename Cls>
	void RegisterBlueprintCommand(const FName& Cmd, Cls* Obj, const FName& FuncName);
	template<typename Cls>
	void UnregisterBlueprintCommand(const FName& Cmd, Cls* Obj, const FName& FuncName);

private:
	TMap<FName, TArray<CommandFunctionBase*> > CommandFunctions;
};

template<typename Cls>
void AStateMachineCharacter::RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)())
{
	CommandFuncParam0<Cls>* CommandFunc = new CommandFuncParam0<Cls>();
	CommandFunc.Obj = Obj;
	CommandFunc->Functor = functor;

	TArray<CommandFunctionBase*>* SignFuncs = CommandFunctions.Find(Cmd);
	if (!SignFuncs)
	{
		SignFuncs= &CommandFunctions.Add(Cmd);
	}
	SignFuncs->Push(CommandFunc);
}

template<typename Cls, typename T1>
void AStateMachineCharacter::RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1))
{
	CommandFuncParam1<Cls, T1>* CommandFunction = new CommandFuncParam1<Cls, T1>();
	CommandFunction->Functor = functor;
	CommandFunction->Obj = Obj;

	FString FuncSignature = Cmd.ToString() + ParamSignatureString< typename RemoveReferenceTypeUtil<T1>::Type >::GetParamSig();
	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (!SigFuncs)
	{
		SigFuncs = &CommandFunctions.Add(FuncSignature);
	}

	SigFuncs->Push(CommandFunction);
}

template<typename Cls, typename T1, typename T2>
void AStateMachineCharacter::RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2))
{
	CommandFuncParam2<Cls, T1, T2>* CommandFunction = new CommandFuncParam2<Cls, T1, T2>();
	CommandFunction->Functor = functor;
	CommandFunction->Obj = Obj;

	FString FuncSignature = Cmd.ToString() + ParamSignatureString< typename RemoveReferenceTypeUtil<T1>::Type >::GetParamSig() + ParamSignatureString< typename RemoveReferenceTypeUtil<T2>::Type >::GetParamSig();
	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (!SigFuncs)
	{
		SigFuncs = &CommandFunctions.Add(FuncSignature);
	}

	SigFuncs->Push(CommandFunction);
}

template<typename Cls, typename T1, typename T2, typename T3>
void AStateMachineCharacter::RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2, T3 p3))
{
	CommandFuncParam3<Cls, T1, T2, T3>* CommandFunction = new CommandFuncParam3<Cls, T1, T2, T3>();
	CommandFunction->Functor = functor;
	CommandFunction->Obj = Obj;

	FString FuncSignature = Cmd.ToString() + ParamSignatureString< typename RemoveReferenceTypeUtil<T1>::Type >::GetParamSig() + ParamSignatureString< typename RemoveReferenceTypeUtil<T2>::Type >::GetParamSig() + ParamSignatureString< typename RemoveReferenceTypeUtil<T3>::Type >::GetParamSig();
	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (!SigFuncs)
	{
		SigFuncs = &CommandFunctions.Add(FuncSignature);
	}

	SigFuncs->Push(CommandFunction);
}

template<typename Cls>
void AStateMachineCharacter::UnregisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)())
{
	TArray<CommandFunctionBase*>* SignFuncs = CommandFunctions.Find(Cmd);
	if (SignFuncs)
	{
		for (int32 i = SignFuncs->Num() -1 ;i >= 0;i--)
		{
			if (!(*SignFuncs)[i]->bBlueprintFunc)
			{
				CommandFuncParam0<Cls>* Commandfunc = (CommandFuncParam0<Cls>*)(*SignFuncs)[i];
				if (Commandfunc->Obj == Obj && Commandfunc->Functor == functor)
				{
					delete Commandfunc;
					SignFuncs->RemoveAt(i);
				}
			}
		}
	}

	if (SignFuncs->Num() == 0)
	{
		CommandFunctions.Remove(Cmd);
	}
}

template<typename Cls, typename T1>
void AStateMachineCharacter::UnregisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1))
{
	FString FuncSignature = Cmd.ToString() + ParamSignatureString< typename RemoveReferenceTypeUtil<T1>::Type >::GetParamSig();
	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (SigFuncs)
	{
		for (int32 i = SigFuncs->Num() - 1; i >= 0; --i)
		{
			if (!(*SigFuncs)[i]->bBlueprintFunc)
			{
				CommandFuncParam1<Cls, T1>* CmdFunc = (CommandFuncParam1<Cls, T1>*)(*SigFuncs)[i];
				if (CmdFunc->Obj == Obj && CmdFunc->Functor == functor)
				{
					delete CmdFunc;
					SigFuncs->RemoveAt(i);
				}
			}
		}

		if (SigFuncs->Num() == 0)
		{
			CommandFunctions.Remove(FuncSignature);
		}
	}
}

template<typename Cls, typename T1, typename T2>
void AStateMachineCharacter::UnregisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2))
{
	FString FuncSignature = Cmd.ToString() + ParamSignatureString< typename RemoveReferenceTypeUtil<T1>::Type >::GetParamSig() + ParamSignatureString< typename RemoveReferenceTypeUtil<T2>::Type >::GetParamSig();
	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (SigFuncs)
	{
		for (int32 i = SigFuncs->Num() - 1; i >= 0; --i)
		{
			if (!(*SigFuncs)[i]->bBlueprintFunc)
			{
				CommandFuncParam2<Cls, T1, T2>* CmdFunc = (CommandFuncParam2<Cls, T1, T2>*)(*SigFuncs)[i];
				if (CmdFunc->Obj == Obj && CmdFunc->Functor == functor)
				{
					delete CmdFunc;
					SigFuncs->RemoveAt(i);
				}
			}
		}

		if (SigFuncs->Num() == 0)
		{
			CommandFunctions.Remove(FuncSignature);
		}
	}
}

template<typename Cls, typename T1, typename T2, typename T3>
void AStateMachineCharacter::UnregisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)(T1 p1, T2 p2, T3 p3))
{
	FString FuncSignature = Cmd.ToString() + ParamSignatureString< typename RemoveReferenceTypeUtil<T1>::Type >::GetParamSig() + ParamSignatureString< typename RemoveReferenceTypeUtil<T2>::Type >::GetParamSig() + ParamSignatureString< typename RemoveReferenceTypeUtil<T3>::Type >::GetParamSig();
	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (SigFuncs)
	{
		for (int32 i = SigFuncs->Num() - 1; i >= 0; --i)
		{
			if (!(*SigFuncs)[i]->bBlueprintFunc)
			{
				CommandFuncParam3<Cls, T1, T2, T3>* CmdFunc = (CommandFuncParam3<Cls, T1, T2, T3>*)(*SigFuncs)[i];
				if (CmdFunc->Obj == Obj && CmdFunc->Functor == functor)
				{
					delete CmdFunc;
					SigFuncs->RemoveAt(i);
				}
			}
		}

		if (SigFuncs->Num() == 0)
		{
			CommandFunctions.Remove(FuncSignature);
		}
	}
}

template<typename Cls>
void AStateMachineCharacter::RegisterBlueprintCommand(const FName& Cmd, Cls* Obj, const FName& FuncName)
{
	UFunction* pFindFunc = Obj->FindFunction(FuncName);
	if (nullptr == pFindFunc)
		return;

	CommandBlueprintFunction<Cls>* CommandFunction = new CommandBlueprintFunction<Cls>();
	CommandFunction->FuncName = FuncName;
	CommandFunction->Obj = Obj;

	FString FuncSignature = Cmd.ToString();
	for (UProperty* Property = (UProperty*)pFindFunc->Children; Property && (Property->PropertyFlags & (CPF_Parm)) == CPF_Parm; Property = (UProperty*)Property->Next)
	{
		FuncSignature += Property->GetCPPType();
	}

	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (!SigFuncs)
	{
		SigFuncs = &CommandFunctions.Add(FName(*FuncSignature));
	}

	SigFuncs->Push(CommandFunction);
}

template<typename Cls>
void AStateMachineCharacter::UnregisterBlueprintCommand(const FName& Cmd, Cls* Obj, const FName& FuncName)
{
	UFunction* pFindFunc = Obj->FindFunction(FuncName);
	if (nullptr == pFindFunc)
		return;

	FString FuncSignature = Cmd.ToString();
	for (UProperty* Property = (UProperty*)pFindFunc->Children; Property && (Property->PropertyFlags & (CPF_Parm)) == CPF_Parm; Property = (UProperty*)Property->Next)
	{
		FuncSignature += Property->GetCPPType();
	}

	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FuncSignature);
	if (SigFuncs)
	{
		for (int32 i = SigFuncs->Num() - 1; i >= 0; --i)
		{
			if ((*SigFuncs)[i]->bBlueprintFunc)
			{
				CommandBlueprintFunction<Cls>* CmdFunc = (CommandBlueprintFunction<Cls>*)(*SigFuncs)[i];
				if (CmdFunc->Obj == Obj && CmdFunc->FuncName == FuncName)
				{
					delete CmdFunc;
					SigFuncs->RemoveAt(i);
				}
			}
		}

		if (SigFuncs->Num() == 0)
		{
			CommandFunctions.Remove(FuncSignature);
		}
	}
}

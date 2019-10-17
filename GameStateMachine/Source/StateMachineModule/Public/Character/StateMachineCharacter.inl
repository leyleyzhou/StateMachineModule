#pragma once
#include "CoreMinimal.h"
#include "SMMCommon.h"
#include "StateMachineCharacter.h"

template<typename Cls>
void AStateMachineCharacter::RegisterCommand(const FName& Cmd, Cls* Obj, void(Cls::* functor)())
{
	CommandFuncParam0<Cls>* CommandFunc = new CommandFuncParam0<Cls>();
	CommandFunc.Obj = Obj;
	CommandFunc->Functor = functor;

	TArray<CommandFunctionBase*>* SignFuncs = CommandFunctions.Find(Cmd);
	if (!SignFuncs)
	{
		SignFuncs = &CommandFunctions.Add(Cmd);
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
		for (int32 i = SignFuncs->Num() - 1; i >= 0; i--)
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


template<typename T1>
bool AStateMachineCharacter::CharacterDoCommand(FName Cmd, T1 Param1)
{
	typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
	FString ParamSignature = ParamSignatureString< T1NoRefType >::GetParamSig();

	TArray<uint8> ParamData;
	int32 DataOffset = 0;
	ParamData.AddDefaulted(sizeof(T1NoRefType));

	*((T1NoRefType*)&ParamData[DataOffset]) = Param1;
	DataOffset += sizeof(T1NoRefType);

	bool bFoundConversion = ApplyCommandConversion(Cmd, ParamSignature, ParamData);
	if (!bFoundConversion)
	{
		FString FuncSignature = Cmd.ToString() + ParamSignature;
		return DoCommandInternal(FuncSignature, ParamData);
	}
	return true;
}

template<typename T1, typename T2>
bool AStateMachineCharacter::CharacterDoCommand(FName Cmd, T1 Param1, T2 Param2)
{
	typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
	typedef typename RemoveReferenceTypeUtil<T2>::Type T2NoRefType;
	FString ParamSignature = ParamSignatureString< T1NoRefType >::GetParamSig() + ParamSignatureString< T2NoRefType >::GetParamSig();

	TArray<uint8> ParamData;
	int32 DataOffset = 0;
	ParamData.AddDefaulted(sizeof(T1NoRefType) + sizeof(T2NoRefType));

	*((T1NoRefType*)&ParamData[DataOffset]) = Param1;
	DataOffset += sizeof(T1NoRefType);

	*((T2NoRefType*)&ParamData[DataOffset]) = Param2;
	DataOffset += sizeof(T2NoRefType);

	bool bFoundConversion = ApplyCommandConversion(Cmd, ParamSignature, ParamData);
	if (!bFoundConversion)
	{
		FString FuncSignature = Cmd.ToString() + ParamSignature;
		return DoCommandInternal(FuncSignature, ParamData);
	}
	return true;
}

template<typename T1, typename T2, typename T3>
bool AStateMachineCharacter::CharacterDoCommand(FName Cmd, T1 Param1, T2 Param2, T3 Param3)
{
	typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
	typedef typename RemoveReferenceTypeUtil<T2>::Type T2NoRefType;
	typedef typename RemoveReferenceTypeUtil<T3>::Type T3NoRefType;

	FString ParamSignature = ParamSignatureString< T1NoRefType >::GetParamSig() + ParamSignatureString< T2NoRefType >::GetParamSig() + ParamSignatureString< T3NoRefType >::GetParamSig();

	TArray<uint8> ParamData;
	int32 DataOffset = 0;
	ParamData.AddDefaulted(sizeof(T1NoRefType) + sizeof(T2NoRefType) + sizeof(T3NoRefType));

	*((T1NoRefType*)&ParamData[DataOffset]) = Param1;
	DataOffset += sizeof(T1NoRefType);

	*((T2NoRefType*)&ParamData[DataOffset]) = Param2;
	DataOffset += sizeof(T2NoRefType);

	*((T3NoRefType*)&ParamData[DataOffset]) = Param3;
	DataOffset += sizeof(T3NoRefType);

	bool bFoundConversion = ApplyCommandConversion(Cmd, ParamSignature, ParamData);
	if (!bFoundConversion)
	{
		FString FuncSignature = Cmd.ToString() + ParamSignature;
		return DoCommandInternal(FuncSignature, ParamData);
	}
	return true;
}


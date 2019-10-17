// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineCharacter.h"

// Sets default values
AStateMachineCharacter::AStateMachineCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStateMachineCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStateMachineCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStateMachineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AStateMachineCharacter::CharacterDoCommand(FName Cmd)
{
	TArray<uint8> EmptyData;
	FString ParamSignature = TEXT("");

	bool bFoundConversion = ApplyCommandConversion(Cmd, ParamSignature, EmptyData);
	if (!bFoundConversion)
	{
		FString FuncSignature = Cmd.ToString();
		return DoCommandInternal(FuncSignature, EmptyData);
	}

	return true;
}

void AStateMachineCharacter::SetCommandConversion(FName SourceCommand, FName TargetCommand, bool DelayExc /*= false*/, bool CacheEveryExecute /*= false*/)
{
	bool bFound = false;
	for (int32 i = 0; i < CommandConversions.Num(); ++i)
	{
		if (CommandConversions[i].SourceCommand == SourceCommand && CommandConversions[i].TargetCommand == TargetCommand)
		{
			CommandConversions[i].bDelayExecute = DelayExc;
			CommandConversions[i].bCacheEveryExecute = CacheEveryExecute;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		CommandConversion NewConversion;
		NewConversion.SourceCommand = SourceCommand;
		NewConversion.TargetCommand = TargetCommand;
		NewConversion.bDelayExecute = DelayExc;
		NewConversion.bCacheEveryExecute = CacheEveryExecute;
		CommandConversions.Add(NewConversion);
	}
}

void AStateMachineCharacter::RemoveCommandConversion(FName SourceCommand, FName TargetCommand)
{
	for (int32 i = 0; i < CommandConversions.Num(); ++i)
	{
		if (CommandConversions[i].SourceCommand == SourceCommand && CommandConversions[i].TargetCommand == TargetCommand)
		{
			if (CommandConversions[i].bDelayExecute)
			{
				TArray<CommandInvokeCache>& InvokeCaches = CommandConversions[i].InvokeCaches;
				for (int32 InvokeIdx = 0; InvokeIdx < InvokeCaches.Num(); ++InvokeIdx)
				{
					DoCommandInternal(InvokeCaches[i].FuncSignature, InvokeCaches[i].ParamData);
				}
			}
			CommandConversions.RemoveAt(i);
			break;
		}
	}
}

bool AStateMachineCharacter::ApplyCommandConversion(const FName& CmdName, const FString& ParamSignature, const TArray<uint8>& Params)
{
	bool bFoundConversion = false;
	for (int32 ConvIdx = 0; ConvIdx < CommandConversions.Num(); ++ConvIdx)
	{
		if (CommandConversions[ConvIdx].SourceCommand == CmdName)
		{
			bFoundConversion = true;

			CommandConversion& CmdConv = CommandConversions[ConvIdx];
			FName& TargetCmd = CmdConv.TargetCommand;

			if (CmdConv.bDelayExecute)
			{
				CommandInvokeCache CacheInvokeInstance;
				CacheInvokeInstance.FuncSignature = TargetCmd.ToString() + ParamSignature;

				//���������ÿ�ε��õ��������Ҫ�Ѳ���������������ͬ�ĵ��úϲ�������������Command�滻�ڼ�һ��������10��SourceCommand����ô���ִ�л����TargetCommandֻ��һ��
				if (!CmdConv.bCacheEveryExecute)
				{
					bool AlreadyCached = false;
					for (int32 CacheIdx = 0; CacheIdx < CmdConv.InvokeCaches.Num(); ++CacheIdx)
					{
						if (CacheInvokeInstance.FuncSignature == CmdConv.InvokeCaches[CacheIdx].FuncSignature)
						{
							CmdConv.InvokeCaches[CacheIdx].ParamData = Params;
							AlreadyCached = true;
						}
					}

					if (AlreadyCached)
					{
						continue;
					}
				}

				CacheInvokeInstance.ParamData = Params;
				CmdConv.InvokeCaches.Add(CacheInvokeInstance);
			}
			else
			{
				FString FuncSignature = TargetCmd.ToString() + ParamSignature;
				DoCommandInternal(FuncSignature, Params);
			}
		}
	}

	return bFoundConversion;
}

bool AStateMachineCharacter::DoCommandInternal(const FString& FuncSignature, const TArray<uint8>& Params)
{
	TArray<CommandFunctionBase*>* SigFuncs = CommandFunctions.Find(FName(*FuncSignature));
	if (!SigFuncs)
	{
		return false;
	}

	for (int32 i = 0; i < SigFuncs->Num(); ++i)
	{
		((*SigFuncs)[i])->DoCommand(Params);
	}
	return true;
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "StateMachineCharacter.h"
#include "StateMachine.h"
#include "Engine/ActorChannel.h"
FName AStateMachineCharacter::CharacterAbilityStateSystemName(TEXT("CharacterAbilityStateSystem"));

// Sets default values
AStateMachineCharacter::AStateMachineCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AbilityStateSystem = ObjectInitializer.CreateDefaultSubobject<UAbilityStateSystem>(this, CharacterAbilityStateSystemName);
	AbilityStateSystem->BindOwnerCharacter(this);
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
	AbilityStateSystem->Tick(DeltaTime);
}

// Called to bind functionality to input
void AStateMachineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AStateMachineCharacter::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = false;

	WroteSomething |= Super::ReplicateSubobjects(Channel,Bunch,RepFlags);
	//TODO
	//const TArray<UActorComponent*>& RepComponents = GetReplicatedComponents();
	//for (UActorComponent* ActorComp : RepComponents)
	//{
	//	if (ActorComp && ActorComp->GetIsReplicated())
	//	{
	//		WroteSomething |= ActorComp->ReplicateSubobjects(Channel, Bunch, RepFlags);		// Lets the component add subobjects before replicating its own properties.
	//		UGameCharacterAbilityComponent* AbilityComp = Cast<UGameCharacterAbilityComponent>(ActorComp);
	//		if (AbilityComp)
	//		{
	//			AbilityComp->ReplicateSelf(Channel, Bunch, RepFlags);
	//		}
	//		else
	//		{
	//			WroteSomething |= Channel->ReplicateSubobject(ActorComp, *Bunch, *RepFlags);	// (this makes those subobjects 'supported', and from here on those objects may have reference replicated)		
	//		}
	//	}
	//}
	
	//WroteSomething |= AbilityStateSystem->ReplicateSelf(Channel, Bunch, RepFlags);   //TODO

	WroteSomething |= Channel->ReplicateSubobject(AbilityStateSystem, *Bunch, *RepFlags);

	return WroteSomething;
}

void AStateMachineCharacter::MarkComponentsAsPendingKill()
{
	Super::MarkComponentsAsPendingKill();
	AbilityStateSystem->MarkPendingKill();
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

bool AStateMachineCharacter::CheckStateMutex(int32 StateID)
{
	//TODO
	return false;
}

void AStateMachineCharacter::UpdateStateMutex()
{
	//TODO
}

void AStateMachineCharacter::UpdateCurrentStateIDs()
{
	//TODO

}

const TArray<ECharacterPlayerState>* AStateMachineCharacter::GetMutexFlagByStateID(int32 StateID)
{
	//TODO
	return nullptr;
}

const TArray<ECharStateTagType>* AStateMachineCharacter::GetTagsByStateID(int32 StateID)
{
	return nullptr;
	//TODO
}

void AStateMachineCharacter::UpdateStateTags()
{
	//TODO
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

				//如果不缓存每次调用的情况，需要把参数数量和类型相同的调用合并起来，比如在Command替换期间一共发起了10次SourceCommand，那么最后执行缓存的TargetCommand只有一次
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


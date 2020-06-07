#include "GameCharacterAbilityComp.h"
#include "StateMachine.h"
#include "SMMCommon.h"
#include "Engine/ActorChannel.h"


UGameCharacterAbilityComponent::UGameCharacterAbilityComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bActiveStateRegisterd(false)
	, CachedPrimitiveTickEnabled(false)
{
	bWantsInitializeComponent = true;
	bAutoActivate = false;

	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicated(true);
}

void UGameCharacterAbilityComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OwnerCharacter = Cast<AStateMachineCharacter>(GetOwner());
	RegisterStates();
	RegisterCommands();
	RegisterBPCommands(false);
}

void UGameCharacterAbilityComponent::UninitializeComponent()
{
	UnregisterStates();
	UnregisterCommands();
	UnregisterBPCommands(false);
	Super::UninitializeComponent();
}

void UGameCharacterAbilityComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (bActiveStateRegisterd)
	{
		UnregisterActiveStates();
		UnregisterActiveCommands();
		UnregisterBPCommands(true);
		bActiveStateRegisterd = false;
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
	OwnerCharacter = nullptr;
}

void UGameCharacterAbilityComponent::Activate(bool bReset)
{
	if (nullptr == OwnerCharacter)
		return;

	if (!bActiveStateRegisterd)
	{
		RegisterActiveStates();
		RegisterActiveCommands();
		RegisterBPCommands(true);
		bActiveStateRegisterd = true;
	}

	Super::Activate(bReset);
}

void UGameCharacterAbilityComponent::Deactivate()
{
	if (nullptr == OwnerCharacter)
		return;

	if (bActiveStateRegisterd)
	{
		UnregisterActiveStates();
		UnregisterActiveCommands();
		UnregisterBPCommands(true);
		bActiveStateRegisterd = false;
	}

	Super::Deactivate();
}

void UGameCharacterAbilityComponent::OnRep_IsActive()
{
	Super::OnRep_IsActive();

	if (bIsActive)
	{
		if (!bActiveStateRegisterd)
		{
			RegisterActiveStates();
			RegisterActiveCommands();
			RegisterBPCommands(true);
			bActiveStateRegisterd = true;
		}
	}
	else
	{
		if (bActiveStateRegisterd)
		{
			UnregisterActiveStates();
			UnregisterActiveCommands();
			UnregisterBPCommands(true);
			bActiveStateRegisterd = false;
		}
	}
}

void UGameCharacterAbilityComponent::AfterInitPlayer()
{

}

void UGameCharacterAbilityComponent::UpdateDisableTickState(bool bDisabled)
{
	if (bDisabled)
	{
		if (PrimaryComponentTick.IsTickFunctionRegistered())
		{
			CachedPrimitiveTickEnabled = PrimaryComponentTick.IsTickFunctionEnabled();
		}
		else
		{
			CachedPrimitiveTickEnabled = PrimaryComponentTick.bCanEverTick && PrimaryComponentTick.bStartWithTickEnabled && (GetNetMode() != NM_DedicatedServer || PrimaryComponentTick.bAllowTickOnDedicatedServer);
		}

		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
	else
	{
		PrimaryComponentTick.SetTickFunctionEnable(CachedPrimitiveTickEnabled);
	}
}

void UGameCharacterAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UGameCharacterAbilityComponent::GetStateTags(TSet<ECharStateTagType>& StateTags) const
{
	if (bIsActive)
	{
		StateTags.Append(ActiveStateTags);
	}
}

void UGameCharacterAbilityComponent::RegisterBlueprintFunctionCommand(const FName& Command, const FName& FuncName)
{
	//OwnerCharacter->RegisterBlueprintCommand(Command, this, FuncName);
}

void UGameCharacterAbilityComponent::UnregisterBlueprintFunctionCommand(const FName& Command, const FName& FuncName)
{
	//OwnerCharacter->UnregisterBlueprintCommand(Command, this, FuncName);
}

void UGameCharacterAbilityComponent::RegisterBPCommands(bool InNeedActive)
{
	for (int32 i = 0; i < BPCommandFuncs.Num(); ++i)
	{
		if (BPCommandFuncs[i].bNeedActive == InNeedActive)
		{
			//OwnerCharacter->RegisterBlueprintCommand(BPCommandFuncs[i].CmdName, this, BPCommandFuncs[i].FuncName);
		}
	}
}

void UGameCharacterAbilityComponent::UnregisterBPCommands(bool InNeedActive)
{
	for (int32 i = 0; i < BPCommandFuncs.Num(); ++i)
	{
		if (BPCommandFuncs[i].bNeedActive == InNeedActive)
		{
			//OwnerCharacter->UnregisterBlueprintCommand(BPCommandFuncs[i].CmdName, this, BPCommandFuncs[i].FuncName);
		}
	}
}

void UGameCharacterAbilityComponent::ClearRepPropertyDirty(UActorChannel* Channel)
{
	DirtyRepChannels.Add(MakeWeakObjectPtr(Channel));
}

bool UGameCharacterAbilityComponent::IsRepPropertyDirty(UActorChannel* Channel)
{
	if (nullptr == DirtyRepChannels.Find(MakeWeakObjectPtr(Channel)))
	{
		return true;
	}

	const TSharedRef<FObjectReplicator>* ObjReplicator = Channel->ReplicationMap.Find(this);
	if ((*ObjReplicator)->RemoteFuncInfo.Num() > 0)
	{
		return true;
	}

	return false;
}

bool UGameCharacterAbilityComponent::ReplicateSelf(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = false;
	if (IsRepPropertyDirty(Channel))
	{
		WroteSomething = Channel->ReplicateSubobject(this, *Bunch, *RepFlags);
		ClearRepPropertyDirty(Channel);
	}

	return WroteSomething;
}

void UGameCharacterAbilityComponent::ComponentRegistCommondInfo(FName CommandName, FString FunctionName)
{
	//OwnerCharacter->RegisterBlueprintCommand(CommandName, this, FName(*FunctionName));
}

void UGameCharacterAbilityComponent::ComponentUnRegistCommondInfo(FName CommandName, FString FunctionName)
{
	//OwnerCharacter->UnregisterBlueprintCommand(CommandName, this, FName(*FunctionName));
}

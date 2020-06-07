#include "GameCharacterBasicAbility.h"

//------------------------------------------------------------------------------------------------------------------
AbilityStateFalling::AbilityStateFalling(UGameCharacterBasicAbilityComponent* BasicAbility)
	: BasicAbilityComponent(BasicAbility)
{

}

void AbilityStateFalling::CalcPriority()
{
	StatePriority = EAbilityStatePriority_None;

	/*if (OwnerCharacter->GameCharacterMovement && OwnerCharacter->GameCharacterMovement->IsLogicFalling())
	{
		StatePriority = EAbilityStatePriority_6;
	}*/
}

void AbilityStateFalling::GetStateTags(TSet<ECharStateTagType>& StateTags) const
{
	/*UGameCharacterMovementComponent* GameCharacterMovement = Cast<UGameCharacterMovementComponent>(OwnerCharacter->GetMovementComponent());
	if (GameCharacterMovement &&
		(FVector::DotProduct(OwnerCharacter->GetActorForwardVector(), GameCharacterMovement->DesiredMoveVelocity) > 250.f
			|| FVector::DotProduct(OwnerCharacter->GetActorForwardVector(), GameCharacterMovement->Velocity) > 250.f))
	{
		StateTags.Add(ECharStateTagType::CharStateTag_FallingForward);
	}*/

	StateTags.Add(ECharStateTagType::CharStateTag_Falling);
	StateTags.Add(ECharStateTagType::CharStateTag_InAir);
}

void AbilityStateFalling::Enter(AbilityStateBase* LastState)
{

}

void AbilityStateFalling::SimulateEnter()
{

}

void AbilityStateFalling::Leave(AbilityStateBase* NewState)
{
	//先去掉着地动画，这样玩家落地可以直接跑动
	/*if (NewState && NewState->GetStateID() == (int32)ECharacterPlayerState::ECharPlayerState_Stand)
	{
		if (OwnerCharacter->HasControllAuthority())
		{
			BasicAbilityComponent->StartLandingTask();
		}
	}*/
}

void AbilityStateFalling::SimulateLeave()
{

}

void AbilityStateFalling::Tick(float DeltaTime)
{

}

void AbilityStateFalling::SimulateTick(float DeltaTime)
{

}


//------------------------------------------------------------------------------------------------------------------
UGameCharacterBasicAbilityComponent::UGameCharacterBasicAbilityComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetIsReplicated(true);
}

UGameCharacterBasicAbilityComponent::~UGameCharacterBasicAbilityComponent()
{
	for (int32 i = 0; i < SupportStates.Num(); ++i)
	{
		delete SupportStates[i];
	}

	SupportStates.Empty();
}

void UGameCharacterBasicAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UGameCharacterBasicAbilityComponent, FrozenRate);
}

void UGameCharacterBasicAbilityComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGameCharacterBasicAbilityComponent::RegisterStates()
{
	check(SupportStates.Num() == 0);

	//SupportStates.Push(new AbilityStateStand(this));
	SupportStates.Push(new AbilityStateFalling(this));
	/*SupportStates.Push(new AbilityStateHeavyFalling(this));
	SupportStates.Push(new AbilityStateFlying(this));
	SupportStates.Push(new AbilityStateDying(this));
	SupportStates.Push(new AbilityStateDead(this));
	SupportStates.Push(new AbilityStateFrozen(this));*/

	for (int32 i = 0; i < SupportStates.Num(); ++i)
	{
		OwnerCharacter->AbilityStateSystem->RegisterState(SupportStates[i]);
	}
}

void UGameCharacterBasicAbilityComponent::RegisterCommands()
{

}

void UGameCharacterBasicAbilityComponent::UnregisterStates()
{
	for (int32 i = 0; i < SupportStates.Num(); ++i)
	{
		OwnerCharacter->AbilityStateSystem->UnregisterState(SupportStates[i]);
		delete SupportStates[i];
	}

	SupportStates.Empty();
}

void UGameCharacterBasicAbilityComponent::UnregisterCommands()
{

}
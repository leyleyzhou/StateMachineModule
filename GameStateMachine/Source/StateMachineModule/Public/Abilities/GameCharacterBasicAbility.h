#pragma once
#include "SMMCommon.h"
#include "GameCharacterAbilityComp.h"
#include "StateMachine.h"
#include "GameCharacterBasicAbility.generated.h"

class AStateMachineCharacter;
class UGameCharacterBasicAbilityComponent;

class STATEMACHINEMODULE_API AbilityStateFalling : public AbilityStateBase
{
public:
	AbilityStateFalling(UGameCharacterBasicAbilityComponent* BasicAbility);

	virtual int32 GetStateID() override
	{
		return (int32)ECharacterPlayerState::ECharPlayerState_Falling;
	}

	virtual EAbilityStateGroup GetStateGroup() const
	{
		return EAbilityStateGroup_Basic;
	}

	virtual void CalcPriority() override;

	virtual void GetStateTags(TSet<ECharStateTagType>& StateTags) const;

	virtual void Enter(AbilityStateBase* LastState) override;

	virtual void SimulateEnter() override;

	virtual void Leave(AbilityStateBase* NewState) override;

	virtual void SimulateLeave() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SimulateTick(float DeltaTime) override;

protected:
	UGameCharacterBasicAbilityComponent* BasicAbilityComponent;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STATEMACHINEMODULE_API UGameCharacterBasicAbilityComponent : public UGameCharacterAbilityComponent
{
	GENERATED_BODY()
public:
	UGameCharacterBasicAbilityComponent(const FObjectInitializer& ObjectInitializer);
	~UGameCharacterBasicAbilityComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void RegisterStates();
	virtual void RegisterCommands();

	virtual void UnregisterStates();
	virtual void UnregisterCommands();

public:

protected:
	TArray<AbilityStateBase*> SupportStates;
};

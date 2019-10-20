#pragma once
#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include "StateMachineCharacter.h"
#include "CharacterTypeDefine.h"
#include "SMMReplicatedObject.h"
#include "StateMachine.generated.h"

enum EAbilityStatePriority
{
	EAbilityStatePriority_None,
	EAbilityStatePriority_1,
	EAbilityStatePriority_2,
	EAbilityStatePriority_3,
	EAbilityStatePriority_4,
	EAbilityStatePriority_Standard,
	EAbilityStatePriority_6,
	EAbilityStatePriority_7,
	EAbilityStatePriority_8,
	EAbilityStatePriority_9,
	EAbilityStatePriority_10,
	EAbilityStatePriority_Must,
};

enum EAbilityStateGroup
{
	EAbilityStateGroup_Basic,
	EAbilityStateGroup_Move,
	EAbilityStateGroup_Camera,
	EAbilityStateGroup_Weapon,
	EAbilityStateGroup_AttachState,  //(在现有状态机组基础上 + 附加物改变状态组):::用于做附加物增加状态互斥 标签 或者UI 互斥状态 等等 by_zhoulei 
};

class STATEMACHINEMODULE_API AbilityStateBase
{
public:

	AbilityStateBase()
		: StatePriority(EAbilityStatePriority_None)
	{}

	virtual ~AbilityStateBase() {}

	void SetOwnerCharacter(AStateMachineCharacter* InCharacter)
	{
		OwnerCharacter = InCharacter;
	}

	virtual int32 GetStateID() { return INDEX_NONE; }

	virtual EAbilityStateGroup GetStateGroup() const
	{
		return EAbilityStateGroup_Basic;
	}

	virtual void CalcPriority() { }

	EAbilityStatePriority GetPriority() const
	{
		return StatePriority;
	}

	virtual void GetStateTags(TSet<ECharStateTagType>& StateTags) const { }

	virtual bool CheckCanEnter() { return true; }

	virtual void Enter(AbilityStateBase* LastState) { }

	virtual void SimulateEnter() { }

	virtual void Leave(AbilityStateBase* NewState) { }

	virtual void SimulateLeave() { }

	virtual void Tick(float DeltaTime) { }

	virtual void SimulateTick(float DeltaTime) { }

protected:
	AStateMachineCharacter* OwnerCharacter;

	EAbilityStatePriority StatePriority;
};


class STATEMACHINEMODULE_API AbilityStateMachine
{
public:
	AbilityStateMachine(AStateMachineCharacter* InCharacter, EAbilityStateGroup InStateGroup);
	virtual ~AbilityStateMachine();

	virtual void RegisterState(AbilityStateBase* State);

	virtual void UnregisterState(AbilityStateBase* State);

	virtual void Tick(float DeltaTime);

	virtual void UpdateBestState();

	virtual void ChangeSimulatedState(int32 StateID);

	virtual void ResetState();

	EAbilityStateGroup GetStateGroup() const { return StateGroup; }

	int32 GetStateID();

	AbilityStateBase* GetCurrentState() const { return CurrentState; }

	bool IsValid() const { return (StateMap.Num() > 0); }

protected:
	AStateMachineCharacter* OwnerCharacter;

	EAbilityStateGroup StateGroup;

	AbilityStateBase* CurrentState;

	TMap<int32, AbilityStateBase*> StateMap;
};


UCLASS(BlueprintType)
class STATEMACHINEMODULE_API UAbilityStateSystem : public USMMRepliactedObject
{
	GENERATED_BODY()
public:

	UAbilityStateSystem(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbilityStateSystem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void RegisterState(AbilityStateBase* State);

	virtual void UnregisterState(AbilityStateBase* State);

	virtual void ResetAllState();

	virtual void Tick(float DeltaTime);

	void GetStateMutexFlags(TMap<int32, int8>& CharMutexFlags);

	void GetStateTags(TSet<ECharStateTagType>& StateTags) const;

	UFUNCTION(BlueprintCallable)
	virtual bool CheckInState(int32 StateID);

	UFUNCTION()
	void OnRep_CurrentStateIDs();

	TArray<AbilityStateMachine*> StateList;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentStateIDs)
	TArray<int32> CurrentStateIDs;

	TMap<int32, int8> MutexFlags;
};
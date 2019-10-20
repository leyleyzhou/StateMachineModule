#include "StateMachien.h"

AbilityStateMachine::AbilityStateMachine(AStateMachineCharacter* InCharacter, EAbilityStateGroup InStateGroup)
	: OwnerCharacter(InCharacter)
	, StateGroup(InStateGroup)
	, CurrentState(nullptr)
{

}

AbilityStateMachine::~AbilityStateMachine()
{

}
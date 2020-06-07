// Copyright 2016 P906, Perfect World, Inc.

#include "InputRawDataStructs.h"
#include "PWDelegateManager.h"
#include "PWLibrary.h"

FPlayerInputRaw::FPlayerInputRaw()
{
	Forward = 0;
	Right = 0;
	LookUp = 0;
	Turn = 0;
}

bool FPlayerInputRaw::_RegisterActionInput(const FName& ActionInputName)
{
	if (ActionInputName == NAME_None)
		return false;
	if (HasAction(ActionInputName))
		return false;

	ActionInputStates.Add(FActionInputState(ActionInputName, EActionInputState::Free));

	return true;
}

EActionInputState FPlayerInputRaw::_GetDefalutNextActionInputState(const FName& ActionInputName, EActionInputState InState, float ActiveTime)
{
	switch (InState)
	{
	case EActionInputState::Press:
		return EActionInputState::Hold;

	case EActionInputState::Click:
		return EActionInputState::Free;

	case EActionInputState::DoublePress:
		return EActionInputState::DoubleHold;

	case EActionInputState::ClickRelease:
	{
		float DoubleClickTime = ButtonDoubleClickDeltaTime;
		if (ActionInputName == InputGamePadRightUp)
		{
			DoubleClickTime = GamePadRightUpDoubleClickDeltaTime;
		}
		if (ActiveTime >= DoubleClickTime)
			return EActionInputState::Click;

		break;
	}
	case EActionInputState::DoubleClickRelease:
		return EActionInputState::Free;

	case EActionInputState::RepeatRelease:
		return EActionInputState::Free;

	case EActionInputState::DoubleRepeatRelease:
		return EActionInputState::Free;

	}

	return InState;
}

void FPlayerInputRaw::Update(float Deltatime)
{
	UpdateAllActionInputStateToDefaultNext();

	bool bNoInput = true;
	const int32 StateNum = ActionInputStates.Num();
	for (int32 StateIndex = 0; StateIndex < StateNum; StateIndex++)
	{
		if (!ActionInputStates.IsValidIndex(StateIndex))
		{
			continue;
		}
		auto& BS = ActionInputStates[StateIndex];
		if (BS.State == EActionInputState::Hold || BS.State == EActionInputState::DoubleHold)
		{
			float PreTime = BS.GetCurrentStateActiveTime();
			BS.UpdateActiveTime(Deltatime);
			float PostTime = BS.GetCurrentStateActiveTime();
			float RepeatStartTime = ButtonRepeatStartTime;
			if (PreTime < RepeatStartTime && PostTime >= RepeatStartTime)
			{
				if (BindgingActor.IsValid() && UPWDelegateManager::Get(BindgingActor.Get())->IsValidLowLevel())
				{
					if (BS.State == EActionInputState::Hold)
					{
						UPWDelegateManager::Get(BindgingActor.Get())->OnInputButtonRepeat.Broadcast(BS.ActionName);
					}
					else if (BS.State == EActionInputState::DoubleHold)
					{
						UPWDelegateManager::Get(BindgingActor.Get())->OnInputButtonDoubleRepeat.Broadcast(BS.ActionName);
					}
				}
			}
		}
		else
		{
			BS.UpdateActiveTime(Deltatime);
		}

		if (bNoInput && BS.State != EActionInputState::Free)
		{
			bNoInput = false;
		}
	}

	if (bNoInput && !(Forward == 0 && Right == 0 /*&& LookUp == 0 && Turn == 0 */))
	{
		bNoInput = false;
	}

	if (bNoInput)
	{
		NoInputTime += Deltatime;
	}
	else
	{
		NoInputTime = 0;
	}
}

void FPlayerInputRaw::UpdateAllActionInputStateToDefaultNext()
{
	for (auto& BS : ActionInputStates)
	{
		_UpdateActionInputStateToDefaultNext(BS);
	}
}

void FPlayerInputRaw::_UpdateActionInputStateToDefaultNext(const FActionInputState& ActionState)
{
	EActionInputState DefaultNextState = _GetDefalutNextActionInputState(ActionState.ActionName, ActionState.State, ActionState.GetCurrentStateActiveTime());

	if (DefaultNextState != EActionInputState::Unknow
		/*&& ActionInputStatesMatainFrameRecorder[ActionInputName] > 1*/)
	{
		SetActionInputState(ActionState.ActionName, DefaultNextState);
	}
}

void FPlayerInputRaw::UpdateActionInputState(const FName& ActionInputName, bool bPressed)
{
	if (!HasAction(ActionInputName))
		_RegisterActionInput(ActionInputName);

	EActionInputState CurrentState = GetActionInputState(ActionInputName);
	EActionInputState NewState = CurrentState;

	if (CurrentState == EActionInputState::Unknow)
	{
		NewState = bPressed ? EActionInputState::Press : EActionInputState::Free;
	}
	else
	{
		if (bPressed)
		{
			if (CurrentState == EActionInputState::Free || CurrentState == EActionInputState::Press
				|| CurrentState == EActionInputState::Hold || CurrentState == EActionInputState::DoubleHold
				|| CurrentState == EActionInputState::Click || CurrentState == EActionInputState::DoublePress
				|| CurrentState == EActionInputState::DoubleClickRelease || CurrentState == EActionInputState::RepeatRelease
				|| CurrentState == EActionInputState::DoubleRepeatRelease)
			{
				NewState = EActionInputState::Press;
			}
			else if (CurrentState == EActionInputState::ClickRelease)
			{
				NewState = EActionInputState::DoublePress;
			}
		}
		else
		{
			if (CurrentState == EActionInputState::Press)
			{
				NewState = EActionInputState::ClickRelease;
			}
			else if (CurrentState == EActionInputState::Hold)
			{
				float HoldTime = GetActionCurrentStateActiveTime(ActionInputName);
				float RepeatStartTime = ButtonRepeatStartTime;
				if (HoldTime < RepeatStartTime)
				{
					NewState = EActionInputState::ClickRelease;
				}
				else
				{
					NewState = EActionInputState::RepeatRelease;
				}
			}
			else if (CurrentState == EActionInputState::DoublePress)
			{
				NewState = EActionInputState::DoubleClickRelease;
			}
			else if (CurrentState == EActionInputState::DoubleHold)
			{
				float HoldTime = GetActionCurrentStateActiveTime(ActionInputName);
				float RepeatStartTime = ButtonRepeatStartTime;
				if (HoldTime < RepeatStartTime)
				{
					NewState = EActionInputState::DoubleClickRelease;
				}
				else
				{
					NewState = EActionInputState::DoubleRepeatRelease;
				}
			}
		}
	}
	SetActionInputState(ActionInputName, NewState);
}

EActionInputState FPlayerInputRaw::GetActionInputState(const FName& ActionInputName) const
{
	for (int i = 0; i < ActionInputStates.Num(); i++)
	{
		if (ActionInputStates[i].ActionName == ActionInputName)
		{
			return ActionInputStates[i].State;
		}
	}

	return EActionInputState::Unknow;
}

float FPlayerInputRaw::GetActionCurrentStateActiveTime(const FName& ActionInputName) const
{
	for (int i = 0; i < ActionInputStates.Num(); i++)
	{
		if (ActionInputStates[i].ActionName == ActionInputName)
		{
			return ActionInputStates[i].GetCurrentStateActiveTime();
		}
	}

	return 0;
}

void FPlayerInputRaw::SetActionInputState(const FName& ActionInputName, EActionInputState NewState)
{
	for (int i = 0; i < ActionInputStates.Num(); i++)
	{
		if (ActionInputStates[i].ActionName == ActionInputName)
		{
			ActionInputStates[i].SetCurrentState(NewState);
			if (NewState == EActionInputState::Click)
			{
				if (BindgingActor.IsValid() && UPWDelegateManager::Get(BindgingActor.Get())->IsValidLowLevel())
				{
					UPWDelegateManager::Get(BindgingActor.Get())->OnInputButtonClick.Broadcast(ActionInputName);
				}
			}
			else if (NewState == EActionInputState::DoublePress)
			{
				if (BindgingActor.IsValid() && UPWDelegateManager::Get(BindgingActor.Get())->IsValidLowLevel())
				{
					UPWDelegateManager::Get(BindgingActor.Get())->OnInputButtonDoubleClick.Broadcast(ActionInputName);
				}
			}
			return;
		}
	}
}

bool FPlayerInputRaw::IsActionInputStatePress(const FName& ActionInputName)
{
	if (HasAction(ActionInputName))
		return GetActionInputState(ActionInputName) == EActionInputState::Press;
	return false;
}

bool FPlayerInputRaw::IsActionInputStatePressOrHold(const FName& ActionInputName)
{
	if (HasAction(ActionInputName))
	{
		EActionInputState CurrentState = GetActionInputState(ActionInputName);
		return  CurrentState == EActionInputState::Hold || CurrentState == EActionInputState::Press;
	}
	return false;
}

bool FPlayerInputRaw::HasAction(const FName& ActionName)
{
	for (int i = 0; i < ActionInputStates.Num(); i++)
	{
		if (ActionInputStates[i].ActionName == ActionName)
		{
			return true;
		}
	}
	return false;
}

float FPlayerInputRaw::GetNoInputTime() const
{
	return NoInputTime;
}

void FPlayerInputRaw::Reset()
{
	Forward = 0;
	Right = 0;
	LookUp = 0;
	Turn = 0;

	ActionInputStates.Empty();
	// 	for (int i = 0; i < ActionInputStates.Num(); i++)
	// 	{
	// 		ActionInputStates[i].State = EActionInputState::Free;
	// 	}
}


// Copyright 2016 P906, Perfect World, Inc.

#include "WidgetBase.h"
#include "InputRawDataStructs.h"
#include "Kismet/GameplayStatics.h"

void UPWWidgetBase::ResetBindings()
{
	if (InputComponent)
	{
		InputComponent->ClearActionBindings();
		InputComponent->AxisBindings.Reset();
	}
}

void UPWWidgetBase::ApplyBindings(int32 IgnoreBindings, bool bUseRegionalButton/* = true*/)
{
	SetupInputComponent();

	if (!TEST_BIT(IgnoreBindings, EBindingTypes::Menu))				BindMenu();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::MenuSpecial))		BindMenuSpecial();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::Bumpers))			BindBumpers();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::Triggers))			BindTriggers();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::LeftStick))		BindLeftStick();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::LeftStickAxes))	BindLeftStickAxes();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::RightStick))		BindRightStick();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::RightStickAxes))	BindRightStickAxes();
	if (!TEST_BIT(IgnoreBindings, EBindingTypes::TouchPad))			BindTouchPad();
}

void UPWWidgetBase::BindMenu(bool bUseRegionalButton/* = true*/)
{
	check(InputComponent);

	// Menu direction - DPad
	InputComponent->BindAction(InputGamePadLeftUp, IE_Pressed, this, &UPWWidgetBase::MenuUpPressed);
	InputComponent->BindAction(InputGamePadLeftDown, IE_Pressed, this, &UPWWidgetBase::MenuDownPressed);
	InputComponent->BindAction(InputGamePadLeftLeft, IE_Pressed, this, &UPWWidgetBase::MenuLeftPressed);
	InputComponent->BindAction(InputGamePadLeftRight, IE_Pressed, this, &UPWWidgetBase::MenuRightPressed);

	InputComponent->BindAction(InputGamePadLeftUp, IE_Released, this, &UPWWidgetBase::MenuUpReleased);
	InputComponent->BindAction(InputGamePadLeftDown, IE_Released, this, &UPWWidgetBase::MenuDownReleased);
	InputComponent->BindAction(InputGamePadLeftLeft, IE_Released, this, &UPWWidgetBase::MenuLeftReleased);
	InputComponent->BindAction(InputGamePadLeftRight, IE_Released, this, &UPWWidgetBase::MenuRightReleased);

	InputComponent->BindAction(InputGamePadLeftUp, IE_Repeat, this, &UPWWidgetBase::MenuUpRepeat);
	InputComponent->BindAction(InputGamePadLeftDown, IE_Repeat, this, &UPWWidgetBase::MenuDownRepeat);
	InputComponent->BindAction(InputGamePadLeftLeft, IE_Repeat, this, &UPWWidgetBase::MenuLeftRepeat);
	InputComponent->BindAction(InputGamePadLeftRight, IE_Repeat, this, &UPWWidgetBase::MenuRightRepeat);

	// Menu Accept & Cancel
	//if (UPWBlueprintLibrary::IsEnterCrossNotCircle(this) && bUseRegionalButton)
	//{
	//	InputComponent->BindAction(InputGamePadRightDown, IE_Pressed, this, &UPWWidgetBase::MenuAcceptPressed);
	//	InputComponent->BindAction(InputGamePadRightRight, IE_Pressed, this, &UPWWidgetBase::MenuCancelPressed);

	//	InputComponent->BindAction(InputGamePadRightDown, IE_Released, this, &UPWWidgetBase::MenuAcceptReleased);
	//	InputComponent->BindAction(InputGamePadRightRight, IE_Released, this, &UPWWidgetBase::MenuCancelReleased);

	//	InputComponent->BindAction(InputGamePadRightDown, IE_Repeat, this, &UPWWidgetBase::MenuAcceptRepeat);
	//	InputComponent->BindAction(InputGamePadRightRight, IE_Repeat, this, &UPWWidgetBase::MenuCancelRepeat);
	//}
	//else
	{
		InputComponent->BindAction(InputGamePadRightRight, IE_Pressed, this, &UPWWidgetBase::MenuAcceptPressed);
		InputComponent->BindAction(InputGamePadRightDown, IE_Pressed, this, &UPWWidgetBase::MenuCancelPressed);

		InputComponent->BindAction(InputGamePadRightRight, IE_Released, this, &UPWWidgetBase::MenuAcceptReleased);
		InputComponent->BindAction(InputGamePadRightDown, IE_Released, this, &UPWWidgetBase::MenuCancelReleased);

		InputComponent->BindAction(InputGamePadRightRight, IE_Repeat, this, &UPWWidgetBase::MenuAcceptRepeat);
		InputComponent->BindAction(InputGamePadRightDown, IE_Repeat, this, &UPWWidgetBase::MenuCancelRepeat);
	}

	// Menu Face Button 1 & 2
	InputComponent->BindAction(InputGamePadRightLeft, IE_Pressed, this, &UPWWidgetBase::MenuAction1Pressed);
	InputComponent->BindAction(InputGamePadRightUp, IE_Pressed, this, &UPWWidgetBase::MenuAction2Pressed);

	InputComponent->BindAction(InputGamePadRightLeft, IE_Released, this, &UPWWidgetBase::MenuAction1Released);
	InputComponent->BindAction(InputGamePadRightUp, IE_Released, this, &UPWWidgetBase::MenuAction2Released);

	InputComponent->BindAction(InputGamePadRightLeft, IE_Repeat, this, &UPWWidgetBase::MenuAction1Repeat);
	InputComponent->BindAction(InputGamePadRightUp, IE_Repeat, this, &UPWWidgetBase::MenuAction2Repeat);
}

void UPWWidgetBase::BindMenuSpecial()
{
	// Menu Special Left & Right
	InputComponent->BindAction(InputGamePadSpecialLeft, IE_Pressed, this, &UPWWidgetBase::MenuSpecialLeftPressed);
	InputComponent->BindAction(InputGamePadSpecialRight, IE_Pressed, this, &UPWWidgetBase::MenuSpecialRightPressed);

	InputComponent->BindAction(InputGamePadSpecialLeft, IE_Released, this, &UPWWidgetBase::MenuSpecialLeftReleased);
	InputComponent->BindAction(InputGamePadSpecialRight, IE_Released, this, &UPWWidgetBase::MenuSpecialRightReleased);

	InputComponent->BindAction(InputGamePadSpecialLeft, IE_Repeat, this, &UPWWidgetBase::MenuSpecialLeftRepeat);
	InputComponent->BindAction(InputGamePadSpecialRight, IE_Repeat, this, &UPWWidgetBase::MenuSpecialRightRepeat);
}

void UPWWidgetBase::BindBumpers()
{
	check(InputComponent);

	// Bumpers
	InputComponent->BindAction(InputGamePadL1, IE_Pressed, this, &UPWWidgetBase::L1Pressed);
	InputComponent->BindAction(InputGamePadR1, IE_Pressed, this, &UPWWidgetBase::R1Pressed);

	InputComponent->BindAction(InputGamePadL1, IE_Released, this, &UPWWidgetBase::L1Released);
	InputComponent->BindAction(InputGamePadR1, IE_Released, this, &UPWWidgetBase::R1Released);

	InputComponent->BindAction(InputGamePadL1, IE_Repeat, this, &UPWWidgetBase::L1Repeat);
	InputComponent->BindAction(InputGamePadR1, IE_Repeat, this, &UPWWidgetBase::R1Repeat);
}

void UPWWidgetBase::BindTriggers()
{
	check(InputComponent);

	// Triggers
	InputComponent->BindAction(InputGamePadL2, IE_Pressed, this, &UPWWidgetBase::L2Pressed);
	InputComponent->BindAction(InputGamePadR2, IE_Pressed, this, &UPWWidgetBase::R2Pressed);

	InputComponent->BindAction(InputGamePadL2, IE_Released, this, &UPWWidgetBase::L2Released);
	InputComponent->BindAction(InputGamePadR2, IE_Released, this, &UPWWidgetBase::R2Released);

	InputComponent->BindAction(InputGamePadL2, IE_Repeat, this, &UPWWidgetBase::L2Repeat);
	InputComponent->BindAction(InputGamePadR2, IE_Repeat, this, &UPWWidgetBase::R2Repeat);

	// Axis
	InputComponent->BindAxis(InputGamePadL2, this, &UPWWidgetBase::L2Axis);
	InputComponent->BindAxis(InputGamePadR2, this, &UPWWidgetBase::R2Axis);
}

void UPWWidgetBase::BindLeftStick()
{
	// Menu direction - Left Stick
	InputComponent->BindAction(InputGamePadLeftStickUp, IE_Pressed, this, &UPWWidgetBase::MenuUpPressed);
	InputComponent->BindAction(InputGamePadLeftStickDown, IE_Pressed, this, &UPWWidgetBase::MenuDownPressed);
	InputComponent->BindAction(InputGamePadLeftStickLeft, IE_Pressed, this, &UPWWidgetBase::MenuLeftPressed);
	InputComponent->BindAction(InputGamePadLeftStickRight, IE_Pressed, this, &UPWWidgetBase::MenuRightPressed);

	InputComponent->BindAction(InputGamePadLeftStickUp, IE_Released, this, &UPWWidgetBase::MenuUpReleased);
	InputComponent->BindAction(InputGamePadLeftStickDown, IE_Released, this, &UPWWidgetBase::MenuDownReleased);
	InputComponent->BindAction(InputGamePadLeftStickLeft, IE_Released, this, &UPWWidgetBase::MenuLeftReleased);
	InputComponent->BindAction(InputGamePadLeftStickRight, IE_Released, this, &UPWWidgetBase::MenuRightReleased);

	InputComponent->BindAction(InputGamePadLeftStickUp, IE_Repeat, this, &UPWWidgetBase::MenuUpRepeat);
	InputComponent->BindAction(InputGamePadLeftStickDown, IE_Repeat, this, &UPWWidgetBase::MenuDownRepeat);
	InputComponent->BindAction(InputGamePadLeftStickLeft, IE_Repeat, this, &UPWWidgetBase::MenuLeftRepeat);
	InputComponent->BindAction(InputGamePadLeftStickRight, IE_Repeat, this, &UPWWidgetBase::MenuRightRepeat);
}

void UPWWidgetBase::BindLeftStickAxes()
{
	check(InputComponent);

	// Axis
	InputComponent->BindAxis(InputGamePadLeftAxisX, this, &UPWWidgetBase::LeftStickXAxis);
	InputComponent->BindAxis(InputGamePadLeftAxisY, this, &UPWWidgetBase::LeftStickYAxis);
}

void UPWWidgetBase::BindRightStick()
{
	check(InputComponent);

	// Directional
	InputComponent->BindAction(InputGamePadRightStickUp, IE_Pressed, this, &UPWWidgetBase::RightStickUpPressed);
	InputComponent->BindAction(InputGamePadRightStickDown, IE_Pressed, this, &UPWWidgetBase::RightStickDownPressed);
	InputComponent->BindAction(InputGamePadRightStickLeft, IE_Pressed, this, &UPWWidgetBase::RightStickLeftPressed);
	InputComponent->BindAction(InputGamePadRightStickRight, IE_Pressed, this, &UPWWidgetBase::RightStickRightPressed);

	InputComponent->BindAction(InputGamePadRightStickUp, IE_Released, this, &UPWWidgetBase::RightStickUpReleased);
	InputComponent->BindAction(InputGamePadRightStickDown, IE_Released, this, &UPWWidgetBase::RightStickDownReleased);
	InputComponent->BindAction(InputGamePadRightStickLeft, IE_Released, this, &UPWWidgetBase::RightStickLeftReleased);
	InputComponent->BindAction(InputGamePadRightStickRight, IE_Released, this, &UPWWidgetBase::RightStickRightReleased);

	InputComponent->BindAction(InputGamePadRightStickUp, IE_Repeat, this, &UPWWidgetBase::RightStickUpRepeat);
	InputComponent->BindAction(InputGamePadRightStickDown, IE_Repeat, this, &UPWWidgetBase::RightStickDownRepeat);
	InputComponent->BindAction(InputGamePadRightStickLeft, IE_Repeat, this, &UPWWidgetBase::RightStickLeftRepeat);
	InputComponent->BindAction(InputGamePadRightStickRight, IE_Repeat, this, &UPWWidgetBase::RightStickRightRepeat);

}

void UPWWidgetBase::BindRightStickAxes()
{
	// Axis
	InputComponent->BindAxis(InputGamePadRightAxisX, this, &UPWWidgetBase::RightStickXAxis);
	InputComponent->BindAxis(InputGamePadRightAxisY, this, &UPWWidgetBase::RightStickYAxis);
}

void UPWWidgetBase::BindTouchPad()
{
	check(InputComponent);

	// TouchPad
	InputComponent->BindAxis(InputGamePadTouchX, this, &UPWWidgetBase::TouchPadXAxis);
	InputComponent->BindAxis(InputGamePadTouchY, this, &UPWWidgetBase::TouchPadYAxis);
}

void UPWWidgetBase::OnHide_Implementation()
{
	UnregisterInputComponent();
}

void UPWWidgetBase::OnPreHide_Implementation()
{
}

void UPWWidgetBase::IncreaseNumByTime(const bool bStickIsArrow, const FKey Key, int32& Time, float& Delay, int32 Limit /*= 3*/, int32 Scale /*= 1*/)
{
	float fTime = UGameplayStatics::GetPlayerController(this, 0)->GetInputKeyTimeDown(Key) * Scale;
	if (bStickIsArrow)
	{
		if (Key == EKeys::Gamepad_DPad_Left && fTime == 0)
		{
			fTime = UGameplayStatics::GetPlayerController(this, 0)->GetInputKeyTimeDown(EKeys::Gamepad_LeftStick_Left) * Scale;
		}
		else if (Key == EKeys::Gamepad_DPad_Right && fTime == 0)
		{
			fTime = UGameplayStatics::GetPlayerController(this, 0)->GetInputKeyTimeDown(EKeys::Gamepad_LeftStick_Right) * Scale;
		}
		else if (Key == EKeys::Gamepad_DPad_Up && fTime == 0)
		{
			fTime = UGameplayStatics::GetPlayerController(this, 0)->GetInputKeyTimeDown(EKeys::Gamepad_LeftStick_Up) * Scale;
		}
		else if (Key == EKeys::Gamepad_DPad_Down && fTime == 0)
		{
			fTime = UGameplayStatics::GetPlayerController(this, 0)->GetInputKeyTimeDown(EKeys::Gamepad_LeftStick_Down) * Scale;
		}
	}

	Time = FMath::Clamp(FMath::TruncToInt(fTime) + 1, 1, Limit);
	Delay = 0.1f / Time;
	// Repeat µÄ¼ä¸ôÎª 0.1f
}

void UPWWidgetBase::ClearChildenWithOutFirst(UPanelWidget* ParentWidget)
{
	if (IsValid(ParentWidget))
	{
		int32 Children = ParentWidget->GetChildrenCount();
		for (int32 ChildIndex = 0; ChildIndex < Children; ChildIndex++)
		{
			if (ChildIndex == 0)
			{
				ParentWidget->GetChildAt(ChildIndex)->SetVisibility(ESlateVisibility::Collapsed);
			}
			else
			{
				ParentWidget->RemoveChildAt(1);
			}
		}
	}
}

void UPWWidgetBase::Close()
{
	//UPWUIManager::Get(this)->HideUI(WidgetID);
}

void UPWWidgetBase::SetupInputComponent()
{
	if (!InputComponent)
	{
		InitializeInputComponent();
	}
	else
	{
		ResetBindings();
	}

	RegisterInputComponent();
}

// Copyright 2016 P906, Perfect World, Inc.

#pragma once
#include "InputRawDataStructs.generated.h"


//
//按钮状态切换规则：
//1、第一下长按松开后马上再按不会触发双击，还是走单击逻辑
//2、双击在第二下按下之后触发，如果按下以后不松也不会触发长按
//3、双击以后马上再点击触发的是单击不是双击

UENUM(BlueprintType)
enum class EActionInputState :uint8
{
	Unknow,
	Free,
	Press,
	Hold,			//进入长按状态由FPlayerInputRaw进行，所以进入长按状态时会有OnInputButtonRepeat广播
	DoubleHold,		//进入长按状态由FPlayerInputRaw进行，所以进入长按状态时会有OnInputButtonDoubleRepeat广播
	Click,			//该状态在FPlayerInputRaw的Tick中进行，进入该状态时会有OnInputButtonClick广播
	DoublePress,	//双击在这里，该状态在FPlayerInputRaw中切入，进入该状态时会有OnInputButtonDoubleClick广播
	ClickRelease,
	DoubleClickRelease,
	RepeatRelease,
	DoubleRepeatRelease,
};

const float ButtonRepeatStartTime = 0.3f;
const float ButtonDoubleClickDeltaTime = 0.1f;
const float GamePadRightUpDoubleClickDeltaTime = 0.3f;

USTRUCT(BlueprintType)
struct FActionInputState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
		FName ActionName;

	UPROPERTY(BlueprintReadOnly)
		EActionInputState State;

	UPROPERTY(BlueprintReadOnly)
		float CurrentStateActiveTime = 0.0f;

	FActionInputState()
	{
		ActionName = NAME_None;
		State = EActionInputState::Free;
	}

	FActionInputState(FName InName, EActionInputState InState)
	{
		ActionName = InName;
		State = InState;
	}

	void SetCurrentState(EActionInputState NewState)
	{
		if (State != NewState)
		{
			CurrentStateActiveTime = 0;
			State = NewState;
		}
	}

	float GetCurrentStateActiveTime() const {
		return CurrentStateActiveTime;
	}

	void UpdateActiveTime(float DeltaTime)
	{
		CurrentStateActiveTime += DeltaTime;
	}

};

USTRUCT(BlueprintType)
struct FPlayerInputRaw
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
		float Forward;
	UPROPERTY(BlueprintReadOnly)
		float Right;
	UPROPERTY(BlueprintReadOnly)
		float LookUp;
	UPROPERTY(BlueprintReadOnly)
		float Turn;

	// For Action Mapping
	UPROPERTY(BlueprintReadOnly)
		TArray<FActionInputState> ActionInputStates;

private:

	float NoInputTime;

	TWeakObjectPtr<AActor> BindgingActor;

public:

	FPlayerInputRaw();

	// 注册 ActionInput
	bool _RegisterActionInput(const FName& ActionInputName);

	void SetBindingActor(AActor* Actor) { BindgingActor = Actor; }

	// 获得 按键状态 的 默认后续状态
	EActionInputState _GetDefalutNextActionInputState(const FName& ActionInputName, EActionInputState InState, float ActiveTime);

	void Update(float Deltatime);

	// 将 单点状态 转入 持续状态
	// 1.将 Press 设定为 Hold
	// 2.将 Release 设定为 Free
	void UpdateAllActionInputStateToDefaultNext();
	void _UpdateActionInputStateToDefaultNext(const FActionInputState& ActionState);

	// 设定 指定输入 的 输入状态
	void UpdateActionInputState(const FName& ActionInputName, bool bPressed);

	// 获取 指定输入 的 输入状态
	EActionInputState GetActionInputState(const FName& ActionInputName) const;

	float GetActionCurrentStateActiveTime(const FName& ActionInputName) const;

	void SetActionInputState(const FName& ActionInputName, EActionInputState NewState);

	// 获取 指定输入状态 是否为 Press
	bool IsActionInputStatePress(const FName& ActionInputName);

	// 获取 指定输入状态 是否为 Press 或 Hold
	bool IsActionInputStatePressOrHold(const FName& ActionInputName);

	bool HasAction(const FName& ActionName);

	float GetNoInputTime() const;

	void Reset();
};


//#pragma region InputNames
const FName InputNameESC = "ESC";
const FName InputNameEnter = "Enter";

const FName InputNameForward = "Forward";
const FName InputNameRight = "Right";
const FName InputNameLookUp = "LookUp";
const FName InputNameTurn = "Turn";

const FName InputNameJump = "Jump";
const FName InputNameCrouch = "Crouch";
const FName InputNameProne = "Prone";
const FName InputNameSprint = "Sprint";
const FName InputNameWalk = "Walk";
const FName InputNameInteract = "Interact";
const FName InputNameFire = "Fire";
const FName InputNameAim = "Aim";
const FName InputNameReload = "Reload";
const FName InputNameEquipWeapon = "EquipWeapon";
const FName InputNameSwitchWeapon = "SwitchWeapon";
const FName InputNamePrimaryWeapon1 = "PrimaryWeapon1";
const FName InputNamePrimaryWeapon2 = "PrimaryWeapon2";
const FName InputNameSideArmWeapon3 = "SideArmWeapon3";
const FName InputNameThrowables = "Throwables";
const FName InputNameSwitchFrag = "SwitchFrag";
const FName InputNameSwitchSmoke = "SwitchSmoke";
const FName InputNameSwitchStun = "SwitchStun";
const FName InputNameSwitchBait = "SwitchBait";
const FName InputNameTeleScope = "TeleScope";
const FName InputNameUnarm = "Unarm";
const FName InputNameTeleFocus = "MouseWheel";
const FName InputNameMeleeAttack = "MeleeAttack";

const FName InputNameIncreaseHealth = "IncreaseHealth";
const FName InputNameFreeView = "FreeView";
const FName InputNameSwitchView = "SwitchView";
const FName InputNameVehicleExit = "VehicleExit";
const FName InputNameParachute = "Parachute";
const FName InputNameSwitcSeat1 = "SwitcSeat1";
const FName InputNameSwitcSeat2 = "SwitcSeat2";
const FName InputNameSwitcSeat3 = "SwitcSeat3";
const FName InputNameSwitcSeat4 = "SwitcSeat4";
const FName InputNameSwitcSeat5 = "SwitcSeat5";
const FName InputNameSwitcSeat6 = "SwitcSeat6";
const FName InputNameSwitcSeat7 = "SwitcSeat7";
const FName InputNameSwitcSeat8 = "SwitcSeat8";
const FName InputNameSwitcSeat9 = "SwitcSeat9";
const FName InputNameVehicleHorn = "VehicleHorn";
const FName InputVehicleSprint = "VehicleSprint";
const FName InputNameVehiclePitchForward = "VehiclePitchForward";
const FName InputNameVehiclePitchBack = "VehiclePitchBack";
const FName InputIncreaseZeroRange = "IncreaseZeroRange";
const FName InputDecreaseZeroRange = "DecreaseZeroRange";
const FName InputSwitchFiringMode = "SwitchFiringMode";

const FName InputNameVehicleRoll_L = "VehicleRoll_L";
const FName InputNameVehicleRoll_R = "VehicleRoll_R";

const FName InputNameLeanL = "Lean_L";
const FName InputNameLeanR = "Lean_R";

const FName InputUseF7Reward = "UseF7Reward";
const FName InputUseF6Reward = "UseF6Reward";
const FName InputUseF5Reward = "UseF5Reward";

const FName InputChooseMsg1 = "ChooseQuickMsg1";
const FName InputChooseMsg2 = "ChooseQuickMsg2";
const FName InputChooseMsg3 = "ChooseQuickMsg3";
const FName InputChooseMsg4 = "ChooseQuickMsg4";
const FName InputChooseMsg5 = "ChooseQuickMsg5";
const FName InputChooseMsg6 = "ChooseQuickMsg6";

const FName InputNameHoldBreath = "HoldBreath";

//---use item shortcut---start---zly----
const FName InputUseBandage = "UseBandage";
const FName InputUseInjection = "UseInjection";
const FName InputUseAidPacket = "UseAidPacket";
const FName InputUseMedicalKit = "UseMedicalKit";
const FName InputUsePills = "UsePills";
const FName InputUseDrinks = "UseDrinks";
const FName InputUseTranquillizer = "UseTranquillizer";
//---use item shortcut---end---zly----

const FName InputLeanL = "Lean_L";
const FName InputLeanR = "Lean_R";

const FName InputMultiKillReward = "MultiKillReward";

//#if WITH_EDITOR
const FName InputTestShowUI = "TestShowUI";
const FName InputTestHiddenUI = "TestHiddenUI";
//#endif

const FName InputToggleChatChannel = "ToggleChatChannel";
const FName InputChat = "Chat";
const FName InputToggleMuteMode = "ToggleChatMuteMode";
//#pragma endregion

const FName InputPreWeapon = "PreWeapon";
const FName InputNextWeapon = "NextWeapon";
const FName InputTelescopeZoomIn = "TelescopeZoomIn";
const FName InputTelescopeZoomOut = "TelescopeZoomOut";

const FName InputSetMarker = "SetMarker";
const FName InputDeleteMarker = "DeleteMarker";
const FName InputShowSettings = "ShowSettings";
const FName InputShowMainUI = "ShowMainUI";

//GamePad
const FName InputGamePadLeftUp = "GP_LeftUp";
const FName InputGamePadLeftDown = "GP_LeftDown";
const FName InputGamePadLeftLeft = "GP_LeftLeft";
const FName InputGamePadLeftRight = "GP_LeftRight";
const FName InputGamePadRightUp = "GP_RightUp";
const FName InputGamePadRightDown = "GP_RightDown";
const FName InputGamePadRightLeft = "GP_RightLeft";
const FName InputGamePadRightRight = "GP_RightRight";

const FName InputGamePadL1 = "GP_L1";
const FName InputGamePadL2 = "GP_L2";
const FName InputGamePadR1 = "GP_R1";
const FName InputGamePadR2 = "GP_R2";


const FName InputGamePadOption = "GP_Option";
const FName InputGamePadTouchX = "GP_TouchPadX";
const FName InputGamePadTouchY = "GP_TouchPadY";
const FName InputGamePadLeftAxisX = "GP_LeftAxisX";
const FName InputGamePadLeftAxisY = "GP_LeftAxisY";
const FName InputGamePadRightAxisX = "GP_RightAxisX";
const FName InputGamePadRightAxisY = "GP_RightAxisY";

const FName InputGamePadSpecialLeft = "GP_SpecialLeft";
const FName InputGamePadSpecialRight = "GP_SpecialRight";

const FName InputGamePadLeftStick = "GP_LeftStickButton";
const FName InputGamePadRightStick = "GP_RightStickButton";

const FName InputGamePadLeftStickLeft = "GP_LeftStickLeft";
const FName InputGamePadLeftStickRight = "GP_LeftStickRight";
const FName InputGamePadLeftStickUp = "GP_LeftStickUp";
const FName InputGamePadLeftStickDown = "GP_LeftStickDown";

const FName InputGamePadRightStickLeft = "GP_RightStickLeft";
const FName InputGamePadRightStickRight = "GP_RightStickRight";
const FName InputGamePadRightStickUp = "GP_RightStickUp";
const FName InputGamePadRightStickDown = "GP_RightStickDown";

const FName InputName_AutoSprint = "AutoSprint";

#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECharacterPlayerState : uint8
{
	ECharPlayerState_Invalid = 0,

	ECharPlayerState_Stand = 1,
	ECharPlayerState_Crouch,
	ECharPlayerState_Lie,
	ECharPlayerState_Falling,

	ECharPlayerState_GoundMove_Idle = 20,
	ECharPlayerState_GoundMove_SlowMove,
	ECharPlayerState_GoundMove_Run,
	ECharPlayerState_GoundMove_FastRun,

	ECharPlayerState_Weapon_NoWeapon = 30,
	ECharPlayerState_Weapon_Gun,
	ECharPlayerState_Weapon_SubGun,
	ECharPlayerState_Weapon_Grenade,
	ECharPlayerState_Weapon_Medicine,

	ECharPlayerState_Camera_ThirdPerson = 40,
	ECharPlayerState_Camera_FreeCamera,
	ECharPlayerState_Camera_ThirdPersonAiming,
	ECharPlayerState_Camera_FarAiming,
	ECharPlayerState_Camera_FirstPerson,

	ECharPlayerState_Sideways_None = 50,
	ECharPlayerState_Sideways_Left,
	ECharPlayerState_Sideways_Right,

	ECharState_Skill_Null = 60,
	ECharState_Skill_FullBody,
	ECharState_Skill_UpperBody,
	ECharState_Skill_Special			UMETA(DisplayName = "Special"),    
};

USTRUCT(BlueprintType)
struct FCharacterStateMutexFlags
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<ECharacterPlayerState> MutexList;
};
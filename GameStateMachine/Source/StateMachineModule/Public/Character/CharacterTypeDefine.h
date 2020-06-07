#pragma once
#include "CoreMinimal.h"
#include "CharacterTypeDefine.generated.h"
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


UENUM(BlueprintType)
enum class ECharStateTagType : uint8
{
	CharStateTag_None,
	CharStateTag_Stand = 1,
	CharStateTag_Dead = 2,
	CharStateTag_Dying = 3,
	CharStateTag_Crouch = 4,
	CharStateTag_PGDown = 5,
	CharStateTag_Falling = 6,
	CharStateTag_FallingForward = 7,
	CharStateTag_HeavyFalling = 8,
	CharStateTag_Frozen = 9,
	CharStateTag_Flying = 16,
	CharStateTag_InAir = 17,	//在空中，Flying Falling HeavyFalling都会设置这个Tag

	//攀爬状态定义
	CharStateTag_Climbing = 10,
	CharStateTag_ClimbOnLower = 11,
	CharStateTag_ClimbOverLower = 12,
	CharStateTag_ClimbOnHigher = 13,
	CharStateTag_ClimbOverHigher = 14,
	CharStateTag_DoubleJump = 15,

	CharStateTag_SlowWalk = 20,
	CharStateTag_Run,
	CharStateTag_FastRun,
	CharStateTag_NoMove,

	CharStateTag_CameraDefault = 30,
	CharStateTag_CameraAiming,
	CharStateTag_CameraFarAiming,
	CharStateTag_CameraFirstPerson,
	CharStateTag_CameraFree,

	CharStateTag_EquipMainWeapon = 40,
	CharStateTag_EquipSubWeapon,
	CharStateTag_EquipGrenade,
	CharStateTag_EquipMedicine,
	CharStateTag_NoHandWeaponIK,
	//CharStateTag_PreGunFire,
	CharStateTag_Guarding,
	CharStateTag_Skill1 = 50						 UMETA(DisplayName = "BigStrongJump"),
	CharStateTag_Skill2							 UMETA(DisplayName = "TuJinZhanDimianxuli"),
	CharStateTag_Skill3							 UMETA(DisplayName = "SkillMaskFire"),
	CharStateTag_Skill4							 UMETA(DisplayName = "TuJinZhanKongzhongxuli"),
	CharStateTag_Skill5,
	CharStateTag_Skill6,
	CharStateTag_Skill7 = 56						 UMETA(DisplayName = "SkillShootComponentTag"),
	CharStateTag_Skill8,
	CharStateTag_Skill9,
	CharStateTag_Skill10					     UMETA(DisplayName = "SkillNoHandIK"),
	CharStateTag_HideToShow,
	//CharStateTag_BigStrongAiming,
	CharStateTag_HitFly_f,
	CharStateTag_HitFly_b,
	CharStateTag_HitBack_f,
	CharStateTag_HitBack_b,
	CharStateTag_HitNormal,
	CharStateTag_HitRecover,	// 硬直
	CharStateTag_SkillOneSection,
	CharStateTag_SkillTwoSection,
	CharStateTag_MeleeAttack = 70,
	CharStateTag_Block,
	CharStateTag_Unbeatable,
	CharStateTag_HitBack_l,
	CharStateTag_HitBack_r,

	CharStateTag_Medicine1 = 90,
	CharStateTag_Medicine2,
	CharStateTag_Medicine3,
	CharStateTag_Medicine4,
	CharStateTag_Medicine5,
};


USTRUCT(BlueprintType)
struct FCharacterStateMutexFlags
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<ECharacterPlayerState> MutexList;
};


USTRUCT(BlueprintType)
struct FCharacterStateTags
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<ECharStateTagType> TagList;
};

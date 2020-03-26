// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "ObjectMacros.h"
#include "Engine/DataTable.h"
#include "BaseDataTable.h"
#include "UnrealString.h"
#include "PWPresenceData.generated.h"

const FString PWDTDir_Root = TEXT("/Game/DataTables");
const FString PWDTDir_StandaloneRoot = TEXT("/Game/StandalonePackage/DataTables");
const FString PWDTDir_Common = PWDTDir_Root + TEXT("/Tables");
const FString PWDTDir_DHDT = PWDTDir_Root + TEXT("/DHDT");

//config DataTable name
const FString PWDTName_TeamInfo = TEXT("TeamInfo");
const FString PWDTName_InteractConfig = TEXT("InteractConfig");

//--zly--Start--2018.04.28,For Temporary,it will be deprecated few days later
const FString PWDTDir_Equipment = PWDTDir_DHDT + TEXT("/Equipment");
//--zly--End--2018.04.28,For Temporary,it will be deprecated few days later.

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	/**Indiscrimination.*/
	CT_NoDifference = 0,
	/**Human.*/
	CT_Human = 1,
	/**Wolf.*/
	CT_Werewolf = 2,
	/**leopard.*/
	CT_Leopard = 3,
	/**bear.*/
	CT_Bear = 4,
	CT_Max UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EPWPlayerCareerType : uint8
{
	CPPCT_None = 0 UMETA(DisplayName = "无"),
	/**人.*/
	CPPCT_Human UMETA(DisplayName = "人类"),
	/**狼人*/
	CPPCT_Werewolf UMETA(DisplayName = "狼人-人形态"),
	/**狼Wolf.*/
	CPPCT_Wolf UMETA(DisplayName = "狼人-狼形态"),
	/**豹人*/
	CPPCT_DemiLeopard UMETA(DisplayName = "豹人-人形态"),
	/**豹*/
	CPPCT_Leopard UMETA(DisplayName = "豹人-豹形态"),
};

UENUM(BlueprintType)
enum class EPWOccupation : uint8
{
	EPWOccupation_None = 0,
	EPWOccupation_Human UMETA(DisplayName = "人类"),
	EPWOccupation_Werewolf UMETA(DisplayName = "狼人-人形态"),
	EPWOccupation_Wolf UMETA(DisplayName = "狼人-狼形态"),
	EPWOccupation_DemiLeopard UMETA(DisplayName = "豹人-人形态"),
	EPWOccupation_Leopard UMETA(DisplayName = "豹人-豹形态"),
	EPWOccupation_Max UMETA(Hidden),
};


UENUM(BlueprintType)
enum class EGameStatus : uint8
{
	EGameStatus_None = 0,
	EGameStatus_Offline,
	EGameStatus_Online,
	EGameStatus_Lobby,
	EGameStatus_InTeam,
	EGameStatus_Matching,
	EGameStatus_Fighting,

	EGameStatus_Max,
};


USTRUCT(BlueprintType)
struct FPWGameStatus
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWGameStatus")
		FString Language;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWGameStatus")
		FString GameStatus;
};

USTRUCT(BlueprintType)
struct FPWPresenceData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWPresenceData")
		int32 ItemID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWPresenceData")
		EGameStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWPresenceData")
		TArray<FPWGameStatus>	Display;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPWPresenceData")
		FString		DefaultDisplay;
};

MAKE_DH_DATATABLE_WITH_INT32(PWPresenceDataTable, FPWPresenceData, PWDTDir_DHDT + TEXT("/Presence/PWPresenceData.PWPresenceData"));
/**
* Copyright 2004-2017 Perfect World Co.,Ltd. All Rights Reserved.
* Created by ZP On 6.6.2017
*/

#pragma once
#include "CoreMinimal.h"
#include "Public/Misc/Paths.h"
//~=============================================================================
// store Json data
//  
//~=============================================================================

namespace NSJsonTable
{
	static FString GetRootPath()
	{
		return FPaths::ProjectContentDir();
	}
#define  RootPath GetRootPath()
#define  EquipPath  RootPath + TEXT("Json/Equipment.json")
#define  AnimaPath  RootPath + TEXT("Json/Animation.json")
#define  StringPath RootPath + TEXT("Json/String.json")



}





class PWGAME_API FPWJsonTable
{
public:
	FPWJsonTable();
	static FPWJsonTable& Get();

	template<typename StructType>
	bool GetItemById(int32 id, StructType& Out);

protected:

	TMap<int32, uint8*> Container;

};

template<typename StructType>
FORCEINLINE bool FPWJsonTable::GetItemById(int32 id, StructType& Out)
{
	bool ret = false;


	auto* value = reinterpret_cast<StructType*>(Container.Find(id));
	if (value)
	{
		//if (((UStruct*)value)->GetClass() != reinterpret_cast<UStruct>(Out).GetClass())
		//{
		//	ret = false;
		//}
		//else
		{
			Out = *value;
			ret = true;
		}

	}


	
	return false;
}
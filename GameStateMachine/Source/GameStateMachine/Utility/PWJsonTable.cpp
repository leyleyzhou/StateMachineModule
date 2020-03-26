// Fill out your copyright notice in the Description page of Project Settings.

#include "PWJsonTable.h"
#include "PWLibrary.h"

FPWJsonTable::FPWJsonTable()
{
	//PWLIB::ParseJsonArray2TMap<FEquipment>(NSJsonTable:: ,&Container);

}

FPWJsonTable & FPWJsonTable::Get()
{
	static FPWJsonTable* JsonInst = nullptr;

	if (JsonInst == nullptr)
	{
		JsonInst = new FPWJsonTable();
	}

	return *JsonInst;
}



//bool UMWJsonTable::GetEquipmentById(int32 id, FEquipment& outData)
//{
//	bool  ret = false;
//
//	FEquipment* pBoneInfo = EquipMent.FindByPredicate(
//		[&](const FEquipment Element)
//	{
//		return Element.Id == id;
//	});
//
//	if (pBoneInfo)
//	{
//		ret = true;
//		outData = *pBoneInfo;
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Can not find Equipment By id %d"), id);
//	}
//
//	return ret;
//}



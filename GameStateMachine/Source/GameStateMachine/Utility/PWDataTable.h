/**
* Copyright 2004-2017 Perfect World Co.,Ltd. All Rights Reserved.
* Created by ZP On 6.28.2017
*/

#pragma once
#include "CoreMinimal.h"
#include "PWInventoryDataStructs.h"
#include "PWWeaponDataStructs.h"
#include "Runtime/CoreUObject/Public/UObject/Class.h"
#include "PWDataTableDef.h"


class UDataTable;
struct FPWBattleEquipmentItem;

#define TYPETAG_DESIN 0
#define TYPETAG_ART 1
#define TYPETAG_PROGRAM 2

struct PWGAME_API FPWDataTable
{
	friend class FPWCheatingSystem;
public:
	FPWDataTable();
	~FPWDataTable();
	static FPWDataTable& Get();
	static void Release();
	TMap<int32, uint8*>& GetIDMap();

protected:
	void InitializeGameDatas();

public:

	template<class T>
	bool GetItemByName(const FString& ItemName, T& Out, uint8 TypeTag = TYPETAG_DESIN)
	{
		bool ret = false;
		UStruct* TargetStruct = T::StaticStruct();
		uint8** value = GetDataTablePtrByName(ItemName, TypeTag);//NameContainer.Find(ItemName);

		if (TargetStruct == nullptr || value == nullptr || !IsChildOf(TargetStruct, FPWItemBase::StaticStruct()))
		{
			ret = false;
			return ret;
		}
		UStruct* SourceStruct = ((T*)(*value))->GetUStructPtr();
		//先判断要取的类型等于找到的类型
		//如果要找的是PWItem 找到的不是继承自PWITem 不返回 兼容PickupActor里的查找
		//其他类型要求类型匹配才能返回
		if (TargetStruct == SourceStruct ||
			(TargetStruct == FPWItem::StaticStruct() && IsChildOf(SourceStruct, FPWItem::StaticStruct()))
			)
		{
			Out = *((T*)*value);
			ret = true;
		}
		return ret;
	}

	template<class T>
	T* GetItemByName(const FString& ItemName, uint8 TypeTag)
	{
		UStruct* TargetStruct = T::StaticStruct();
		uint8** value = GetDataTablePtrByName(ItemName, TypeTag);//NameContainer.Find(ItemName);

		if (TargetStruct == nullptr || value == nullptr || !IsChildOf(TargetStruct, FPWItemBase::StaticStruct()))
		{
			return nullptr;
		}
		UStruct* SourceStruct = ((T*)(*value))->GetUStructPtr();
		//先判断要取的类型等于找到的类型
		//如果要找的是PWItem 找到的不是继承自PWITem 不返回 兼容PickupActor里的查找
		//其他类型要求类型匹配才能返回
		if (TargetStruct == SourceStruct ||
			(TargetStruct == FPWItem::StaticStruct() && IsChildOf(SourceStruct, FPWItem::StaticStruct()))
			)
		{
			return (T*)*value;
		}
		return nullptr;
	}


	template<class T>
	bool GetItemById(int32 id, T& Out, uint8 TypeTag = TYPETAG_DESIN)
	{
		bool ret = false;

		UStruct* TargetStruct = T::StaticStruct();
		uint8** value = GetDataTablePtrByID(id, TypeTag); //IDContainer.Find(id);
		
		if(TargetStruct == nullptr || value == nullptr || !IsChildOf(TargetStruct, FPWItemBase::StaticStruct()))
		{
			ret = false;
			return ret;
		}
		UStruct* SourceStruct = ((T*)(*value))->GetUStructPtr();
		//先判断要取的类型等于找到的类型
		//如果要找的是PWItem 找到的不是继承自PWITem 不返回 兼容PickupActor里的查找
		//其他类型要求类型匹配才能返回
		if (TargetStruct == SourceStruct ||
			(IsChildOf(SourceStruct, TargetStruct))) // SourceStruct是TargetStruct的子类也可以返回
		{
			Out = *((T*)*value);
			ret = true;
		}

		return ret;
	}

	template<class T>
	T* GetItemById(int32 id, T& Out, uint8 TypeTag)
	{
		UStruct* TargetStruct = T::StaticStruct();
		uint8** value = GetDataTablePtrByID(id, TypeTag); //IDContainer.Find(id);

		if (TargetStruct == nullptr || value == nullptr || !IsChildOf(TargetStruct, FPWItemBase::StaticStruct()))
		{
			return nullptr;
		}
		UStruct* SourceStruct = ((T*)(*value))->GetUStructPtr();
		//先判断要取的类型等于找到的类型
		//如果要找的是PWItem 找到的不是继承自PWITem 不返回 兼容PickupActor里的查找
		//其他类型要求类型匹配才能返回
		if (TargetStruct == SourceStruct ||
			(IsChildOf(SourceStruct, TargetStruct))) // SourceStruct是TargetStruct的子类也可以返回
		{
			return (T*)*value;
		}
		return nullptr;
	}

	const FPWUseableItem* GetUseableItemPtrById(int32 id) const;

	const FPWStorageItem* GetStorageItemPtrById(int32 id) const;

	const FPWItem* GetItemPtr(int32 ID) const;

	template<typename TableRowType>
	const TableRowType* GetItemPtr(int32 ID) const {
		return Cast<TableRowType>(GetItemPtr(ID));
	}


	/**
	* 搜索 datatable 中，指定 Data 类型的所有数据。
	* 注意：要搜索的data类型必须拥有独有 ItemID，否则将返回不完整的 Array
	*
	* @typename DataClass 返回data的类型（即datatable类型）
	* @param OutData 搜索出的data列表
	*/
	template<class DataClass>
	const void GetDataByDataType(TArray<DataClass> & OutData)
	{
		for (auto& Elem : IDContainer)
		{
			const UStruct* TargetStruct = ((DataClass*)(Elem.Value))->GetUStructPtr();
			if (TargetStruct && IsChildOf(TargetStruct, DataClass::StaticStruct()))
			{
				DataClass* data = reinterpret_cast<DataClass*>(Elem.Value);
				OutData.Add(*data);
			}
		}
	}

	/**
	* 搜索 datatable 中，属性值为某值的所有数据。
	* 注意：Property 必须对 == 符号有重载，否则编译不过。此时建议使用 GetDataByDataType 函数
	* 注意：要搜索的 data 类型必须拥有独有 ItemID，否则将返回空Array
	*
	* @typename DataClass 返回data的类型（即datatable类型）
	* @typename PropertyClass 属性类型
	* @param OutData 搜索出的data列表
	* @param Propertyname 属性名
	* @param PropertyValue 属性值
	*/
	template<class DataClass, class PropertyClass>
	const void GetDataByProperty(TArray<DataClass> & OutData, const FString& Propertyname, PropertyClass PropertyValue)
	{
		for (auto& Elem : IDContainer)
		{
			const UStruct* TargetStruct = ((DataClass*)(Elem.Value))->GetUStructPtr();
			if (TargetStruct && IsChildOf(TargetStruct, DataClass::StaticStruct()))
			{
				DataClass* data = reinterpret_cast<DataClass*>(Elem.Value);

				for (TFieldIterator<UProperty> It(TargetStruct); It; ++It)
				{
					UProperty* Property = *It;

					if (Property && Propertyname == Property->GetName())
					{
						const PropertyClass* value = Property->ContainerPtrToValuePtr<PropertyClass>(data);
						if (value && *value == PropertyValue)
						{
							OutData.Add(*data);
						}
					}
				}
			}
		}
	}

	template<class T>
	bool IsA(int32 ID) const
	{
		bool ret = false;

		UStruct* TargetStruct = T::StaticStruct();
		const uint8* const* value = IDContainer.Find(ID);

		if (TargetStruct == nullptr || value == nullptr || !IsChildOf(TargetStruct, FPWItemBase::StaticStruct()))
		{
			return false;
		}

		UStruct* SourceStruct = ((T*)(*value))->GetUStructPtr();
		if (TargetStruct == SourceStruct 
			|| IsChildOf(SourceStruct, TargetStruct))
		{
			return true;
		}

		return false;
	}

	void LoadAllAsync();

	void UpdateDatatable(TMap<FString, FString> NewTBMap);

	/**修改了表结构，添加新表转换旧表的显示接口。*/
	bool GetPWBattleEquipmentItem(int32 ItemID, FPWBattleEquipmentItem& Out);
	/**与GetPWBattleEquipmentItem不同表。*/
	bool GetPWEquipmentItem(int32 ItemID, FPWEquipmentItem& Out);
	/**获取武器皮肤，新代码使用新的读表方式。*/
	bool GetPWWeaponSkin(int32 ItemID, FPWWeaponSkin& Out);

protected:

	void AddTo(UDataTable* DT, uint8 TypeTag = TYPETAG_DESIN);

	bool IsChildOf(const UStruct* SomeChild, const UStruct* SomeBase) const;

	uint8** GetDataTablePtrByID(int32 ID, uint8 TypeTag = TYPETAG_DESIN);
	uint8** GetDataTablePtrByName(const FString& Name, uint8 TypeTag = TYPETAG_DESIN);

private:

	void LoadAllDataTable(const FString& DataTableDir, TArray<UDataTable*>& OutTables);
	UDataTable* LoadDataTableByName(const FString& TableName);
	
	void RemoveTableForPlatform(TArray<FAssetData> &List);

	void AddItemToContainer(FPWItemBase* Item, uint8 TypeTag = TYPETAG_DESIN);


protected:
	TMap<int32, uint8*> IDContainer;	
	TMap<FString, uint8*> NameContainer;
	TMap<int32, uint8*> IDContainer_Art;
	TMap<FString, uint8*> NameContainer_Art;
	TMap<int32, uint8*> IDContainer_Des;
	TMap<FString, uint8*> NameContainer_Des;
	TMap<int32, uint8*> IDContainer_Pro;
	TMap<FString, uint8*> NameContainer_Pro;

	TArray<UDataTable*> DataTables;
	TArray<UDataTable*> DataTables_Art;
	TArray<UDataTable*> DataTables_Des;
	TArray<UDataTable*> DataTables_Pro;

public:
	static FPWDataTable* JsonInst;

};

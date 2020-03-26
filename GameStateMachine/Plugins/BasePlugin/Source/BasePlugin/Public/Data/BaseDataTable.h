// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseFile.h"

/*
数据配置表实现，目前支持 DataTable 和 CSV 两种格式，整数和浮点类型的Key，支持BinarySearchByKey()查找

结构体需继承自 FTableRowBase ，如下：

USTRUCT(BlueprintType)
struct FSampleTableRow : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;
};

1结构体 VS 1配置表，可以使用如下宏快捷，快速定义：
MAKE_DH_DATATABLE_WITH_STRING_KEY(FSampleTable, FSampleTableRow, TEXT("/Game/DataTables/SampleTable.SampleTable"), ID);

1结构体 VS N配置表，可以通过手动定义来解决：
class FMultiSampleTable : public DHDataTable<FMultiSampleTable, FSampleTableRow, FString, true>
{
public:
    FMultiSampleTable(int32 TagID)
    {
        switch (TagID) {
            case 0:
                Initialize(TEXT("/Game/DataTables/SampleTable0.SampleTable0"));
                return;
            case 1:
                Initialize(TEXT("/Game/DataTables/SampleTable1.SampleTable1"));
                return;
        }
    }
    static const FString &GenerateKey(TableRowTypePtr Row)
    {
        return Row->KeyField;
    }
};

如何使用：
    auto SampleTable = FSampleTable::GetInstance();
    auto Row = SampleTable->GetRowByKey(TEXT("123"));

    auto MultiSampleTable = FMultiSampleTable::GetInstance(0);
    auto Row = MultiSampleTable->GetRowByKey(TEXT("123"));
*/

#include "BaseCommon.h"
#include "Engine/DataTable.h"

/*
 TableClassName         表对象
 TableRowStructType     表行结构体，继承自FTableRowBase
 FilePath               数据表路径
 KeyType                唯一Key字段类型，如果是 integer 或者 float ，将支持BinarySearch查找
 KeyField               TableRowStructType中作为唯一Key的字段
 */
#define MAKE_DH_DATATABLE(TableClassName, TableRowStructType, FilePath, KeyType, KeyField)                      \
class TableClassName : public DHDataTable<TableClassName, TableRowStructType, KeyType>	                        \
{																								                \
public:																							                \
	TableClassName()																		                    \
	{																							                \
		Initialize(FilePath);								                                                    \
	}																							                \
	static const KeyType &GenerateKey(const TableRowTypePtr Row)							                    \
	{																							                \
		return Row->KeyField;																                    \
	}																							                \
};

#define MAKE_DH_DATATABLE_WITH_INT32_KEY(TableClassName, TableRowStructType, FilePath, KeyField)                \
    MAKE_DH_DATATABLE(TableClassName, TableRowStructType, FilePath, int32, KeyField)

#define MAKE_DH_DATATABLE_WITH_STRING_KEY(TableClassName, TableRowStructType, FilePath, KeyField)               \
    MAKE_DH_DATATABLE(TableClassName, TableRowStructType, FilePath, FString, KeyField)

#define MAKE_DH_DATATABLE_WITH_INT32(TableClassName, TableRowStructType, FilePath)                              \
    MAKE_DH_DATATABLE_WITH_INT32_KEY(TableClassName, TableRowStructType, FilePath, ItemID)

#define MAKE_DH_DATATABLE_WITH_STRING(TableClassName, TableRowStructType, FilePath)                             \
    MAKE_DH_DATATABLE_WITH_STRING_KEY(TableClassName, TableRowStructType, FilePath, ItemID)

#define MAKE_DH_DATATABLE_WITH_FLOAT_KEY(TableClassName, TableRowStructType, FilePath, KeyField)                \
    MAKE_DH_DATATABLE(TableClassName, TableRowStructType, FilePath, float, KeyField)

enum class EDHFileType : int8
{
    Asset,      // 文件名 `*/(name).(name)`
    Text,       // 文件名 `*/(name).txt`
    TextCSV,    // 文件名 `*/(name).csv`
    TextJson    // 文件名 `*/(name).json`
};

template <
    typename TableRowType,
    typename KeyType
>
class DHDataTable_DataEntity
{
public:
    typedef const TableRowType* TableRowTypePtr;
protected:
    TMap<KeyType, TableRowTypePtr> _Table;
    TArray<TableRowTypePtr> _TableRows;
    UDataTable* _RawTable = nullptr;
    bool _Sorted = false;
};

template <
    typename TableRowType,
    typename KeyType,
    typename InternalDataType = DHDataTable_DataEntity<TableRowType, KeyType>
>
class DHDataTable_Operation : public DHDataTable_DataEntity<TableRowType, KeyType>
{
    typedef DHDataTable_DataEntity<TableRowType, KeyType> Super;
public:
    // Nullable
    typename Super::TableRowTypePtr GetRowByKey(const KeyType Key)
    {
        return_null_if_null(Super::_RawTable);

        return Super::_Table.FindRef(Key);
    }
};

/*
 * FString，特例化
 * 支持 GetRowByKey(const FString &Key) 和 GetRowByKey(const TCHAR *Key) 方法重载
 */
template <
    typename TableRowType
>
class DHDataTable_Operation<TableRowType, FString> : public DHDataTable_DataEntity<TableRowType, FString>
{
    typedef DHDataTable_DataEntity<TableRowType, FString> Super;
public:
    // Nullable
    typename Super::TableRowTypePtr GetRowByKey(const FString& Key)
    {
        return_null_if_null(Super::_RawTable);

        return Super::_Table.FindRef(Key);
    }

    // Nullable
    typename Super::TableRowTypePtr GetRowByKey(const TCHAR* Key)
    {
        return_null_if_null(Super::_RawTable);

        return Super::_Table.FindRef(Key);
    }
};

/*
 * 行数据比较器，依赖 TableType::GenerateKey 静态方法
 */
template <
    typename TableType,
    typename TableRowType
>
struct DHDataTable_DataRowComparer
{
    bool operator() (const TableRowType& Left, const TableRowType& Right) const
    {
        return TableType::GenerateKey(&Left) < TableType::GenerateKey(&Right);
    }
};

/*
 * 支持 Key 值 二元搜索
 */
template <
    typename TableType,
    typename TableRowType,
    typename KeyType
>
class DHDataTable_BinarySearch : public DHDataTable_Operation<TableRowType, KeyType>
{
    typedef DHDataTable_DataEntity<TableRowType, KeyType> Super;

    void SortOnce()
    {
        return_if_true(Super::_Sorted);
        Super::_Sorted = true;
        Super::_TableRows.Sort(DHDataTable_DataRowComparer<TableType, TableRowType>());
    }
public:
    /*
     * 二元搜索
     * @param ClosedKey 临近 Key 的值
     * @param UseClosedRange 是否使用闭合区间，效果如：在 [1, 3, 6, 7] 中（查找 -2 返回 1，查找 10 返回 7）
     * @return TableRowType*  Nullable
     */
    typename Super::TableRowTypePtr BinarySearchByClosedKey(KeyType ClosedKey, bool UseClosedRange = true)
    {
        auto& Rows = Super::_TableRows;

        int32 Num = Rows.Num();
        return_null_if_true(Num == 0);

        SortOnce();

        int32 Index = 0;
        SIZE_T Size = Num;

        while (Size > 0) {
            const SIZE_T LeftoverSize = Size % 2;
            Size = Size / 2;

            const SIZE_T CheckIndex = Index + Size;
            const SIZE_T StartIfLess = CheckIndex + LeftoverSize;

            auto&& CheckValue = TableType::GenerateKey(Rows[CheckIndex]);
            Index = CheckValue < ClosedKey ? StartIfLess : Index;
        }

        if (Index == INDEX_NONE || Index >= Num) {
            return_null_if_true(!UseClosedRange);

            Index = (ClosedKey < TableType::GenerateKey(Super::_TableRows[0])) ? 0 : (Num - 1);
        }

        return Super::_TableRows[Index];
    }
};

/*
 * 数据表单例
 * @TableType 类
 * @TableRowType 表行结构体，继承自FTableRowBase
 * @KeyType 表中充当 Key 的数据类型
 * @Multiple 是否要支持多表，表的加载及实例获取，通过 TagID 来区分
 */
template <
    typename TableType,
    typename TableRowType,
    typename KeyType,
    bool Multiple = false,
    typename = typename std::enable_if<std::is_base_of<FTableRowBase, TableRowType>::value>::type
>
class DHDataTable :
    public std::conditional<
    (std::is_integral<KeyType>::value || std::is_floating_point<KeyType>::value),
    DHDataTable_BinarySearch<TableType, TableRowType, KeyType>,
    DHDataTable_Operation<TableRowType, KeyType>
    >::type,
    public DHInstance<TableType, Multiple>
{
    typedef DHDataTable_DataEntity<TableRowType, KeyType> Super;
    int32 CurVersion = 0;
    FString FilePath;
public:
    DHDataTable() = default;

    virtual ~DHDataTable()
    {
        Reset();
    }

    void Initialize(const FString& InFilePath)
    {
        return_if_true(Super::_RawTable != nullptr && CurVersion == DHFiles::GetGlobalCfgVersion());

        return_if_true(!DHLoadDataTable(InFilePath, TableRowType::StaticStruct(), Super::_RawTable));

        Super::_RawTable->AddToRoot();

        FString ContextString;

        Super::_TableRows.Empty(Super::_RawTable->GetRowMap().Num());
        Super::_RawTable->template ForeachRow<TableRowType>(ContextString, [this](const FName&, const TableRowType& Row) {
            Super::_Table.Add(TableType::GenerateKey(&Row), &Row);
            Super::_TableRows.Add(&Row);
            });

        FilePath = InFilePath;
        CurVersion = DHFiles::GetGlobalCfgVersion();
    }

    const TArray<typename Super::TableRowTypePtr>& GetRows()
    {
        Initialize(FilePath); // 确保最新，下同
        return Super::_TableRows;
    }

    FORCEINLINE bool IsEmpty()
    {
        Initialize(FilePath);
        return Super::_TableRows.Num() == 0;
    }

    FORCEINLINE int32 RowsNum()
    {
        Initialize(FilePath);
        return Super::_TableRows.Num();
    }

    void Reset()
    {
        return_if_true(!IsValid(Super::_RawTable));

        Super::_RawTable->RemoveFromRoot();
        Super::_RawTable = nullptr;
        Super::_Table.Empty();
        Super::_TableRows.Empty();
        Super::_Sorted = false;

        CurVersion = 0;
    }
};

bool BASEPLUGIN_API DHLoadDataTable(const FString& FilePath, UScriptStruct* ScriptStruct, UDataTable*& Table);

void BASEPLUGIN_API DHScanAssetPaths(const TArray<FString>& Paths);
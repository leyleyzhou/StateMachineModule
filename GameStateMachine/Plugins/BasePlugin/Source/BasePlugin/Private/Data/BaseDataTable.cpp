// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDataTable.h"
#include "Engine/AssetManager.h"
#include "IAssetRegistry.h"
#include "Paths.h"
#include "FileHelper.h"
#include "PlatformFilemanager.h"
#include "Regex.h"
#include "BaseFile.h"

static bool LoadDataTable(const BaseFiles::FileDescription& Description, UScriptStruct* ScriptStruct, UDataTable*& Table);
static bool LoadCSV(const BaseFiles::FileDescription& Description, UScriptStruct* ScriptStruct, UDataTable*& Table);
static void TestOrAddColumnName(FString& Content);
static bool LoadTableFromPatch(const BaseFiles::FileDescription& Description, UScriptStruct* ScriptStruct, UDataTable*& Table);

void DHScanAssetPaths(const TArray<FString>& Paths)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.ScanPathsSynchronous(Paths);
}

bool DHLoadDataTable(const FString& FilePath, UScriptStruct* ScriptStruct, UDataTable*& Table)
{
	BaseFiles::FileDescription Description;
	return_false_if_false(BaseFiles::ParseFilePath(FilePath, Description));

	if (Description.GuessType == BaseFiles::EFileType::TextCSV || BaseFiles::GetNewConfig(FilePath)) // 如果是CSV格式或者有更新的配置表
		return LoadCSV(Description, ScriptStruct, Table);
	else if (Description.GuessType == BaseFiles::EFileType::Asset)
		return LoadDataTable(Description, ScriptStruct, Table);

	return false;
}

static bool LoadDataTable(const BaseFiles::FileDescription& Description, UScriptStruct* ScriptStruct, UDataTable*& Table)
{
	static bool ScanTables = false;

	return_true_if_true(LoadTableFromPatch(Description, ScriptStruct, Table));
	if (!IsInGameThread()) {
		UE_LOG(LogTemp, Error, TEXT("Not In Game Thread"));
	}
	FString Path = FString::Printf(TEXT("/Game/%s/%s.%s"), *Description.Directory, *Description.Name, *Description.Name);

	FName PathOfName(*Path);
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	if (!ScanTables) {
		ScanTables = true;
		TArray<FString> PathsToScan;
		PathsToScan.Add(TEXT("/Game/DataTables/DHDT"));
		AssetRegistry.ScanPathsSynchronous(PathsToScan);
	}
	FAssetData Data = AssetRegistry.GetAssetByObjectPath(PathOfName);

	return_false_if_false(Data.IsValid());

	Table = Cast<UDataTable>(Data.GetAsset());
	Table->RowStruct = ScriptStruct;

	return Table != nullptr;
}

static bool LoadCSV(const BaseFiles::FileDescription& Description, UScriptStruct* ScriptStruct, UDataTable*& Table)
{
	FString CSVPath = FString::Printf(TEXT("%s/%s.csv"), *Description.Directory, *Description.Name);
	FString Content;
	return_false_if_false(BaseFiles::LoadStringContent(CSVPath, Content));

	auto DataTable = NewObject<UDataTable>();
	DataTable->RowStruct = ScriptStruct;

	TestOrAddColumnName(Content);
	TArray<FString> Errors = DataTable->CreateTableFromCSVString(Content);

	if (Errors.Num() > 0) {
		DataTable->ConditionalBeginDestroy();
		return false;
	}

	Table = DataTable;

	return true;
}

static bool LoadTableFromPatch(const BaseFiles::FileDescription& Description, UScriptStruct* ScriptStruct, UDataTable*& Table)
{
	FString CSVPath = FString::Printf(TEXT("%s/%s.csv"), *Description.Directory, *Description.Name);
	FString Content;
	return_false_if_false(BaseFiles::LoadStringContentFromPatchs(CSVPath, Content));

	auto DataTable = NewObject<UDataTable>();
	DataTable->RowStruct = ScriptStruct;

	TestOrAddColumnName(Content);
	if (DataTable->CreateTableFromCSVString(Content).Num() > 0) {
		DataTable->ConditionalBeginDestroy();
		return false;
	}

	Table = DataTable;

	return true;
}

static void TestOrAddColumnName(FString& Content)
{
	return_if_true(Content.StartsWith(TEXT("--")));

	FRegexPattern Pattern("[\\r\\n]+");
	FRegexMatcher Matcher(Pattern, Content);
	auto Ptr = *Content;

	FString Value;
	Value.Empty(Content.Len() * 1.1);

	int32 Index = 0, RowIndex = 0;
	Value += TEXT("---,");

	if (Matcher.FindNext()) {
		Value.AppendChars(Ptr + Index, Matcher.GetMatchBeginning() - Index);
		Value += TEXT('\n');
		Index = Matcher.GetMatchEnding();
	}

	while (Matcher.FindNext()) {
		Value += TEXT("Row");
		Value.AppendInt(++RowIndex);
		Value += TEXT(',');
		Value.AppendChars(Ptr + Index, Matcher.GetMatchBeginning() - Index);
		Value += TEXT('\n');
		Index = Matcher.GetMatchEnding();
	}

	Content = Value;
}

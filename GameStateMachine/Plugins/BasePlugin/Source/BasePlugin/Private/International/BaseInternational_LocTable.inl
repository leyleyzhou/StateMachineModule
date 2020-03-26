#include "International/BaseInternational.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "StringTableRegistry.h"
#include "StringTable.h"
#include "Misc/FileHelper.h"
#include "Serialization/Csv/CsvParser.h"
#include "Internationalization/StringTableRegistry.h"

void DHInternational::Impl::DHInternal_LocTableFromPath(const FName InTableId, const FString& InNamespace, const FString& InFilePath, const FString& InRootPath)
{
	//1. New Loc.
	FStringTableRegistry::Get().Internal_NewLocTable(InTableId, InNamespace);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const FString FullPath = InRootPath / InFilePath;
	const FString FileExtension = "csv";
	TArray<FString> FindFiles;
	PlatformFile.FindFiles(FindFiles, *FullPath, *FileExtension);
	//
	for (auto& CVSFile : FindFiles)
	{
		//2. load file as string.
		FString ImportedStrings;
		if (!FFileHelper::LoadFileToString(ImportedStrings, *CVSFile))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to import string table from '%s'. Could not open file."), *CVSFile);
			continue;
		}
		// 3.Add String.
		const FCsvParser ImportedStringsParser(ImportedStrings);
		const FCsvParser::FRows& Rows = ImportedStringsParser.GetRows();
		// Must have at least 2 rows (header and content)
		if (Rows.Num() <= 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to import string table from '%s'. Incorrect number of rows (must be at least 2)."), *CVSFile);
			continue;
		}

		int32 KeyColumn = INDEX_NONE;
		int32 SourceStringColumn = INDEX_NONE;
		TMap<FName, int32> MetaDataColumns;

		// Validate header
		{
			const TArray<const TCHAR*>& Cells = Rows[0];

			for (int32 CellIdx = 0; CellIdx < Cells.Num(); ++CellIdx)
			{
				const TCHAR* Cell = Cells[CellIdx];
				if (FCString::Stricmp(Cell, TEXT("Key")) == 0 && KeyColumn == INDEX_NONE)
				{
					KeyColumn = CellIdx;
				}
				else if (FCString::Stricmp(Cell, TEXT("SourceString")) == 0 && SourceStringColumn == INDEX_NONE)
				{
					SourceStringColumn = CellIdx;
				}
				else
				{
					const FName MetaDataName = Cell;
					if (!MetaDataName.IsNone())
					{
						MetaDataColumns.Add(MetaDataName, CellIdx);
					}
				}
			}

			bool bValidHeader = true;
			if (KeyColumn == INDEX_NONE)
			{
				bValidHeader = false;
				UE_LOG(LogTemp, Warning, TEXT("Failed to import string table from '%s'. Failed to find required column 'Key'."), *CVSFile);
			}
			if (SourceStringColumn == INDEX_NONE)
			{
				bValidHeader = false;
				UE_LOG(LogTemp, Warning, TEXT("Failed to import string table from '%s'. Failed to find required column 'SourceString'."), *CVSFile);
			}
			if (!bValidHeader)
			{
				continue;
			}
		}

		// Import rows
		{
			//FScopeLock KeyMappingLock(&KeyMappingCS);
			//FScopeLock MetaDataLock(&KeysToMetaDataCS);
			//@zpj
			//ClearSourceStrings(Rows.Num() - 1);
			for (int32 RowIdx = 1; RowIdx < Rows.Num(); ++RowIdx)
			{
				const TArray<const TCHAR*>& Cells = Rows[RowIdx];

				// Must have at least an entry for the Key and SourceString columns
				if (Cells.IsValidIndex(KeyColumn) && Cells.IsValidIndex(SourceStringColumn))
				{
					FString Key = Cells[KeyColumn];
					Key = Key.ReplaceEscapedCharWithChar();

					FString SourceString = Cells[SourceStringColumn];
					SourceString = SourceString.ReplaceEscapedCharWithChar();
					FStringTableRegistry::Get().Internal_SetLocTableEntry(InTableId, Key, SourceString);

					//FStringTableEntryRef TableEntry = FStringTableEntry::NewStringTableEntry(AsShared(), SourceString, FTextLocalizationManager::Get().GetDisplayString(TableNamespace, Key, &SourceString));
					//KeysToEntries.Emplace(Key, TableEntry);
					//DisplayStringsToKeys.Emplace(TableEntry->GetDisplayString(), Key);

					for (const auto& MetaDataColumnPair : MetaDataColumns)
					{
						if (Cells.IsValidIndex(MetaDataColumnPair.Value))
						{
							FString MetaData = Cells[MetaDataColumnPair.Value];
							MetaData = MetaData.ReplaceEscapedCharWithChar();

							if (!MetaData.IsEmpty())
							{
								// 4. add meta.
								FStringTableRegistry::Get().Internal_SetLocTableEntryMetaData(InTableId, Key, MetaDataColumnPair.Key, MetaData);
								//FMetaDataMap& MetaDataMap = KeysToMetaData.FindOrAdd(Key);
								//MetaDataMap.Add(MetaDataColumnPair.Key, MetaData);
							}
						}
					}
				}
			}
		}

#if WITH_EDITOR
		UE_LOG(LogTemp, Log, TEXT(" DHInternational::Internal_LocTableFromPath import filepath '%s'"), *CVSFile);
#endif // WITH_EDITOR
	}
}
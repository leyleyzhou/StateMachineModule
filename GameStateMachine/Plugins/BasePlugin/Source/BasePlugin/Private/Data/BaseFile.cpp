// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseFile.h"
#include "FileHelper.h"
#include "Paths.h"
#define IS_DEVELOPMENT_MODE (!(UE_BUILD_SHIPPING || UE_BUILD_TEST) || WITH_EDITOR)

namespace BaseFiles
{
	static int32 CurrentCfgVersion = 0;

	BASEPLUGIN_API int32 IncGlobalCfgVersion()
	{
		CurrentCfgVersion++;
		return CurrentCfgVersion;
	}


	BASEPLUGIN_API int32 GetGlobalCfgVersion()
	{
		return CurrentCfgVersion;
	}

	BASEPLUGIN_API void ResetGlobalCfgVersion()
	{
		FPlatformAtomics::AtomicStore(&CurrentCfgVersion, 0);
	}


	BASEPLUGIN_API bool ParseFilePath(const FString& FilePath, FileDescription& OutDescription)
	{
		const FString Game = TEXT("/Game/");
		const FString Content = TEXT("/Content/");

		int32 GameIndex = FilePath.Find(Game, ESearchCase::CaseSensitive);
		int32 ContentIndex = FilePath.Find(Content, ESearchCase::CaseSensitive);
		int32 LastIndex = FilePath.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);

		FString FullName = FilePath.Mid(LastIndex + 1);
		int32 DotOfName = FullName.Find(TEXT("."));

		if (DotOfName < 0) { // 没有找到 "." ，默认 Asset 资源
			OutDescription.GuessType = EFileType::Asset;
			OutDescription.Name = FullName;
		}
		else {
			FString Name = FullName.Mid(0, DotOfName);
			FString Ext = FullName.Mid(DotOfName + 1);
			OutDescription.Name = Name;
			OutDescription.ExtensionName = Ext;
			if (Name == Ext) {
				OutDescription.GuessType = EFileType::Asset;
			}
			else if (Ext == TEXT("csv")) {
				OutDescription.GuessType = EFileType::TextCSV;
			}
			else if (Ext == TEXT("json")) {
				OutDescription.GuessType = EFileType::TextJson;
			}
			else if (Ext == TEXT("txt")) {
				OutDescription.GuessType = EFileType::Text;
			}
			else {
				UE_LOG(LogLoad, Warning, TEXT("Unknown file type, file = %s"), *FilePath);
				return false;
			}
		}

		OutDescription.AssetType = OutDescription.GuessType == EFileType::Asset ? EFileAssetType::Asset : EFileAssetType::Package;
		if (GameIndex == 0) {
			OutDescription.Directory = FilePath.Mid(Game.Len(), LastIndex - Game.Len());
		}
		else if (ContentIndex == 0) {
			OutDescription.Directory = FilePath.Mid(Game.Len(), LastIndex - Game.Len());
		}
		else {
			OutDescription.Directory = FilePath.Mid(0, LastIndex);
		}

		return true;
	}

	static TMap<FString, FString> NewConfigMap;

	BASEPLUGIN_API void AddNewConfig(FString Path, FString Content)
	{
		FString BaseFileName = FPaths::GetBaseFilename(Path);
		NewConfigMap.FindOrAdd(BaseFileName) = Content;
	}

	BASEPLUGIN_API void RestNewConfigMap()
	{
		NewConfigMap.Empty();
	}

	BASEPLUGIN_API FString* GetNewConfig(FString Path)
	{
		FString BaseFileName = FPaths::GetBaseFilename(Path);
		auto Content = NewConfigMap.Find(BaseFileName);

		return Content;
	}

	BASEPLUGIN_API bool LoadStringContentFromPatchs(const FString& Path, FString& OutString)
	{
		auto Content = GetNewConfig(Path);
		if (Content)
		{
			OutString = *Content;
			return true;
		}

#if PLATFORM_PS4
		FString Name;
#else
#   if PLATFORM_WINDOWS
		FString FilePath;
#       if IS_DEVELOPMENT_MODE
		FilePath = TEXT("d:/darkhorse-content/") + Path;
#       endif
#   else
		FString FilePath = TEXT("~/darkhorse-content/") + Path;
#   endif
		if (FFileHelper::LoadFileToString(OutString, *FilePath, FFileHelper::EHashOptions::EnableVerify))
		{
			return true;
		}
#endif
		return false;
	}

	BASEPLUGIN_API bool LoadStringContent(const FString& Path, FString& OutString)
	{
		if(LoadStringContentFromPatchs(Path, OutString))
		{
			return true;
		}

		FString FullPath = FPaths::ProjectContentDir() / Path;

		return FFileHelper::LoadFileToString(OutString, *FullPath, FFileHelper::EHashOptions::EnableVerify);
	}

}
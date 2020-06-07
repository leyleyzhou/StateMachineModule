// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace BaseFiles
{
	BASEPLUGIN_API int32 IncGlobalCfgVersion();
	BASEPLUGIN_API int32 GetGlobalCfgVersion();
	BASEPLUGIN_API void ResetGlobalCfgVersion();

	enum class EFileType : int8
	{
		UnKnown,
		Asset,           //文件名 '*/(name).(name)'
		Text,            //文件名 '*/(name).text'
		TextCSV,         //文件名 '*/(name).csv'
		TextJson,        //文件名 '*/(name).json'
	};

	enum class EFileAssetType :int8
	{
		UnKnown,
		Asset,            //UE 资源
		Package,        //包内其他资源
	};
	
	struct FileDescription 
	{
		EFileAssetType AssetType{};
		EFileType      GuessType{};
		FString Directory;
		FString Name;
		FString ExtensionName;
	};

	BASEPLUGIN_API bool ParseFilePath(const FString& FilePath, FileDescription& OutDescription);

	/*
	 *从Patch资源中加载文本
	 */
	BASEPLUGIN_API bool LoadStringContentFromPatchs(const FString &FilePath,FString &OutString);

	/*
		加载文本
		加载顺序：Patch-> Asset Package
	*/
	BASEPLUGIN_API bool LoadStringContent(const FString &FilePath,FString &OutString);

	FORCEINLINE static bool LoadStringContent(const FileDescription& Description, FString& OutString)
	{
		FString Path = FString::Printf(TEXT("%s/%s.%s"),*Description.Directory,*Description.Name,*Description.ExtensionName);

		return LoadStringContent(Path, OutString);
	}

	//保存更新的配置表信息
	BASEPLUGIN_API void AddNewConfig(FString Path,FString Content);

	//重置保存的配置表信息
	BASEPLUGIN_API  void RestNewConfigMap();

	//根据路径/名称 获取更新的配置表内容
	BASEPLUGIN_API  FString* GetNewConfig(FString Path);
}

#pragma once
#include "CoreMinimal.h"
#include "Public/AssetData.h"

class FPWFileUtils {
public:
	static void ScanAndGetAssetDatasInPath(const FName& Path, TArray<FAssetData>& AssetDatas);

	// 得到文件夹里边所有文件名
	static TArray<FString> GetAllFileNames(const FString& Directory);

	// 得到文件夹里边所有文件夹名
	static TArray<FString> GetAllDirNames(const FString& Directory);

	// 存在文件夹
	static bool ExistDirectory(const FString& Directory);

	// 存在文件
	static bool ExistFile(const FString& Path);

	// 创建文件夹
	static bool CreateDirectory(const FString& Directory);

	// 存储文件内容
	static bool SaveArrayToFile(const TArray<uint8>& Array, const FString& File);

	// 读取文件内容
	static bool LoadFileToArray(TArray<uint8>& Array, const FString& File);

	// 复制目录
	static bool CopyDirectory(const FString& FromPath, const FString& ToPath);

	// 删除目录（目录路径）下的子目录，除了某子目录（子目录名）
	static bool RemoveSubDirectoriesExceptSubDirectories(const FString& Directory, const TArray<FString>& ExceptSubDirectories);

	// 删除目录（目录路径）下的子文件，除了某子文件（子文件名）
	static bool RemoveSubFilesExceptSubFiles(const FString& Directory, const TArray<FString>& ExceptFiles);

	// 删除目录（目录路径）下的子文件，除了某子文件（子文件名）
	static bool RemoveSubFiles(const FString& Directory, const TArray<FString>& Files);

	// 删除目录
	static bool DeleteDirectory(const FString& Directory);

	// 删除文件
	static bool DeleteFile(const FString& File);
};


#include "PWFileUtils.h"
#include "AssetRegistryModule.h"
#include "ARFilter.h"
#include "PlatformFilemanager.h"
#include "Public/GenericPlatform/GenericPlatformFile.h"
#include "Public/Misc/Paths.h"
#include "Public/Misc/FileHelper.h"

void FPWFileUtils::ScanAndGetAssetDatasInPath(const FName& Path, TArray<FAssetData>& AssetDatas) {
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.ScanPathsSynchronous(TArray<FString>{Path.ToString()}, false);
	AssetRegistry.GetAssetsByPath(Path, AssetDatas, true);
}

TArray<FString> FPWFileUtils::GetAllFileNames(const FString& Directory){
	class FMarvelFileVisitor : public IPlatformFile::FDirectoryVisitor {
	public:
		FMarvelFileVisitor(TArray<FString>& InFoundFiles)
			: FoundFiles(InFoundFiles) {
		}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override {
			if (!bIsDirectory) {
				FString FullDirectoryName(FilenameOrDirectory);
				FoundFiles.Push(FPaths::GetCleanFilename(FullDirectoryName));
			}
			return true;
		}

		TArray<FString>& FoundFiles;
	};

	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> Files;
	FMarvelFileVisitor Visitor(Files);    
	PlatformFile.IterateDirectory(*Directory, Visitor);
	return Files;
} 

TArray<FString> FPWFileUtils::GetAllDirNames(const FString& Directory) {
	class FMarvelDirectoryVisitor : public IPlatformFile::FDirectoryVisitor {
	public:
		FMarvelDirectoryVisitor(TArray<FString>& InFoundDirectory)
			: FoundDirectory(InFoundDirectory) {
		}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override {
			if (bIsDirectory) {
				FString FullDirectoryName(FilenameOrDirectory);
				FoundDirectory.Push(FPaths::GetCleanFilename(FullDirectoryName));
			}
			return true;
		}

		TArray<FString>& FoundDirectory;
	};

     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	 TArray<FString> Directories;
     FMarvelDirectoryVisitor Visitor(Directories);    
     PlatformFile.IterateDirectory(*Directory, Visitor);
	 return Directories;
}

bool FPWFileUtils::ExistDirectory(const FString& Directory) {
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	 return PlatformFile.DirectoryExists(*Directory);
}

bool FPWFileUtils::ExistFile(const FString& Path) {
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	 return PlatformFile.FileExists(*Path);
}

bool FPWFileUtils::CreateDirectory(const FString& Directory) {
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	 return PlatformFile.CreateDirectory(*Directory);
}

bool FPWFileUtils::SaveArrayToFile(const TArray<uint8>& Array, const FString& File) {
	return FFileHelper::SaveArrayToFile(Array, *File);
}

bool FPWFileUtils::LoadFileToArray(TArray<uint8>& Array, const FString& File) {
	return FFileHelper::LoadFileToArray(Array, *File);
}

bool FPWFileUtils::CopyDirectory(const FString& FromPath, const FString& ToPath) {
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	 return PlatformFile.CopyDirectoryTree(*ToPath, *FromPath, false);
}

bool FPWFileUtils::RemoveSubDirectoriesExceptSubDirectories(const FString& Directory, const TArray<FString>& ExceptDirectories) {
	class FMarvelSubDirectoryVisitor : public IPlatformFile::FDirectoryVisitor {
	public:
		FMarvelSubDirectoryVisitor(const TArray<FString>& InExceptDirectories)
		:ExceptDirectories ( InExceptDirectories){
		}
		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override {
			 IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			if (bIsDirectory && !ExceptDirectories.Contains(FPaths::GetCleanFilename(FilenameOrDirectory))) {
				 return PlatformFile.DeleteDirectoryRecursively(FilenameOrDirectory);
			}
			return true;
		}
	private:
		const TArray<FString>& ExceptDirectories;
	};
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
     FMarvelSubDirectoryVisitor Visitor(ExceptDirectories);    
     PlatformFile.IterateDirectory(*Directory, Visitor);
	 return true;
}

bool FPWFileUtils::RemoveSubFilesExceptSubFiles(const FString& Directory, const TArray<FString>& ExceptFiles) {
	class FMarvelSubFileVisitor : public IPlatformFile::FDirectoryVisitor {
	public:
		FMarvelSubFileVisitor(const TArray<FString>& InExceptFiles) 
		:ExceptFiles (InExceptFiles){
		}
		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override {
			 IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			if (!bIsDirectory && !ExceptFiles.Contains(FPaths::GetCleanFilename(FilenameOrDirectory))) {
				 return PlatformFile.DeleteFile(FilenameOrDirectory);
			}
			return true;
		}
		
	private:
		const TArray<FString>& ExceptFiles;
	};
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
     FMarvelSubFileVisitor Visitor(ExceptFiles);    
     PlatformFile.IterateDirectory(*Directory, Visitor);
	 return true;
}

bool FPWFileUtils::RemoveSubFiles(const FString& Directory, const TArray<FString>& Files)
{
	class FMarvelSubFileVisitor : public IPlatformFile::FDirectoryVisitor {
	public:
		FMarvelSubFileVisitor(const TArray<FString>& InFiles) 
		:Files (InFiles){
		}
		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override {
			 IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			if (!bIsDirectory && Files.Contains(FPaths::GetCleanFilename(FilenameOrDirectory))) {
				 return PlatformFile.DeleteFile(FilenameOrDirectory);
			}
			return true;
		}
		
	private:
		const TArray<FString>& Files;
	};
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
     FMarvelSubFileVisitor Visitor(Files);    
     PlatformFile.IterateDirectory(*Directory, Visitor);
	 return true;
}

bool FPWFileUtils::DeleteDirectory(const FString& Directory) {
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	 return PlatformFile.DeleteDirectoryRecursively(*Directory);
}

bool FPWFileUtils::DeleteFile(const FString& File)
{
     IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	 return PlatformFile.DeleteFile(*File);
}
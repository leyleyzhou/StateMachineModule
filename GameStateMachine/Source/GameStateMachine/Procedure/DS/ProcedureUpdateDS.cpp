// Copyright 2016 P906, Perfect World, Inc.

#include "PWProcedureUpdateDS.h"
#include "Engine/World.h"
#include "CommandLine.h"
#include "Parse.h"
#include "PWGameDataConfig.h"
#include "DHHttpClient.h"
#include "PWProcedureManager.h"
#include "PWUpdateInfo.h"
#include "PWVivoxConfig.h"


void UPWProcedureUpdateDS::Enter()
{
	StartUpdate();
}

void UPWProcedureUpdateDS::StartUpdate()
{
	if (GetWorld()->IsPlayInEditor())
	{
		OnUpdateFinished();
		return;
	}
	if (FParse::Value(FCommandLine::Get(), TEXT("UpdateAddr="), UpdateRootAddr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UPWProcedureUpdateDS::StartUpdate failed, failed to get UpdateAddr"));
		OnUpdateFinished();
		return;
	}
	auto Config = GetInstanceOfJson<FPWGameDataConfig>();
	if (Config == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UPWProcedureUpdateDS::StartUpdate failed, failed to get FPWGameDataConfig"));
		OnUpdateFinished();
		return;
	}

	DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Get, UpdateRootAddr + Config->UpdateListFileName, {}, [this, Config](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
		if (ResultCode != EDHHttpResultCode::Ok)
		{
			UE_LOG(LogTemp, Error, TEXT("UPWProcedureUpdateDS::StartUpdate failed, request UpdateListFile failed, ResultCode = %d"), (int32)ResultCode);
			OnUpdateFinished();
			return;
		}
		FPWUpdateInfo UpdateInfo;
		if (!DHJsonUtils::FromJson(Content, &UpdateInfo))
		{
			UE_LOG(LogTemp, Error, TEXT("UPWProcedureUpdateDS::StartUpdate failed, failed to get UpdateFileInfo"));
			OnUpdateFinished();
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("UPWProcedureUpdateDS::StartUpdate LocalVerSion = %s RemoteVersion = %s, FileListSize = %d"), *Config->Version, *UpdateInfo.Version, UpdateInfo.FileList.Num());
		if (UpdateInfo.Version.Equals(Config->Version) || UpdateInfo.FileList.Num() == 0)
		{
			OnUpdateFinished();
			return;
		}
		//todo download and update all files
		for (auto FileName : UpdateInfo.FileList)
		{
			FString TableUrl = UpdateRootAddr + FileName;
			UnfinishedHttpRequest++;
			DHHttpClient::GetInstance()->RequestStringContent(EDHHttpMethod::Get, TableUrl, {}, [this, FileName](EDHHttpResultCode ResultCode, int32 HttpCode, const FString& Content) {
				if (ResultCode == EDHHttpResultCode::Ok)
				{
					DHFiles::AddNewConfig(FileName, Content);
				}
				else
				{
					PW_LOG(LogTemp, Error, TEXT("UPWProcedureLoginPS4::StartRequestUpdateServer Error, DataTable HttpRequest failed, FileName = %s, ResultCode = %d"), *FileName, (int32)ResultCode);
				}
				UnfinishedHttpRequest--;
				if (UnfinishedHttpRequest == 0)
				{
					DHFiles::IncGlobalCfgVersion();
					OnUpdateFinished();
				}
				});
		}
		});
}

void UPWProcedureUpdateDS::OnUpdateFinished()
{
	UE_LOG(LogTemp, Log, TEXT("UPWProcedureUpdateDS::OnUpdateFinished exec"));
	UPWProcedureManager::GetInstance(this)->ChangeCurState(ProcedureState::ProcedureState_BattleDS);
}
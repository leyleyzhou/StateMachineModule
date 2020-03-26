#include "Client/PWProcedureLoginBase.h"
#include "PWUIManager.h"
#include "UIWidgetHead.h"
#include "DHInternational.h"
#include "DETNet/DETNetClient.h"
#include "PWSaveGameManager.h"
#include "Lobby/PWBulletinWidget.h"
#include "Engine/World.h"
#include "PWLogManager.h"
#include "TimerManager.h"

void UPWProcedureLoginBase::CloseBulletinsWidget()
{
	UPWUIManager* UIMgr = UPWUIManager::Get(this);
	check(UIMgr);
	UIMgr->HideUI(EUMGID::EUMGID_LoginBulletinWidget);
}

void UPWProcedureLoginBase::PullBulletin()
{
	const FString CurLang = DHInternational::GetInstance()->GetDisplayLanguage();
	DETNetClient::Instance().PullBulletin(CurLang);
}


void UPWProcedureLoginBase::CheckBulletinMsg()
{
	UPWUIManager* UIManager = UPWUIManager::Get(this);
	if (!UIManager)
	{
		return;
	}

	int32 LastBulletinID = -1;
	UPWSaveGameManager* SaveGameManager = UPWSaveGameManager::Get(this);
	if (SaveGameManager)
	{
		LastBulletinID = SaveGameManager->GetOtherData()->GetLastBulletinID();
	}

	DETNetClient& Net = DETNetClient::Instance();
	FString CurVersion = Net.GetVer();
	int32 BulletinIndexToShow = -1;
	TArray<FClientBulletinInfo>& BulletinList = Net.BulletinList.List;
	for (int32 BulletinIndex = 0; BulletinIndex < BulletinList.Num(); BulletinIndex++)
	{
		if (BulletinList[BulletinIndex].Id <= LastBulletinID)
		{
			continue;
		}
		if (BulletinIndexToShow >= 0 && BulletinList[BulletinIndex].Id <= BulletinList[BulletinIndexToShow].Id)
		{
			continue;
		}
		if (CurVersion != BulletinList[BulletinIndex].Ver)
		{
			continue;
		}
		BulletinIndexToShow = BulletinIndex;
	}
	PW_LOG(LogTemp, Log, TEXT("CheckBulletinMsg"));
	TWeakObjectPtr<UPWProcedureLoginBase>  WeakThis(this);
	if (BulletinIndexToShow >= 0)
	{
		FClientBulletinInfo& BulletinInfo = BulletinList[BulletinIndexToShow];
		AddBulletin(BulletinInfo.Title, BulletinInfo.Text, BulletinInfo.Id);
		UIManager->ForceShowUI(EUMGID::EUMGID_LoginBulletinWidget);
	}
}

void UPWProcedureLoginBase::OnCloseBulletinWidget()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::CheckBulletinMsg);
}

void UPWProcedureLoginBase::AddBulletin(FString Title, FString Content, int32 ID)
{
	FBulletinMsg NewMsg;
	NewMsg.Title = Title;
	NewMsg.Content = Content;
	NewMsg.ID = ID;
	NewMsg.IsBulletin = true;
	NewMsg.CanSkip = false;
	BulletinMsges.Insert(NewMsg, 0);
}
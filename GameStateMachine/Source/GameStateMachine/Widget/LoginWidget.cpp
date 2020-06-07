// Copyright 2016 P906, Perfect World, Inc.

#include "PWLoginWidget.h"
#include "PWDelegateManager.h"
#include "PWProcedureManager.h"
#include "Paths.h"
#include "PWClientNet.h"
#include "DETNet/DETNetClient.h"
#include "TextBlock.h"
#include "Client/PC/PWProcedureLogin.h"
#include "BaseInternational.h"

void UPWLoginWidget::OnLoginClicked()
{
#if PLATFORM_WINDOWS && !UE_BUILD_SHIPPING
	if (account.IsEmpty())
	{
		const int32 MaxSize = 32767;
		TCHAR ComputerName[MaxSize];
		DWORD BuffCount = MaxSize;
		GetComputerName(ComputerName, &BuffCount);
		account = ComputerName;
	}
#endif

	/*if (!UPWUIManager::Get(this)->IsInViewport(EUMGID::EUMGID_Connecting))
	{
		TWeakObjectPtr<UPWUIManager> WeakThis(UPWUIManager::Get(this));
		FPWUIOpenDelegate UIOpenDelegate = FPWUIOpenDelegate::CreateLambda([WeakThis]() {
			if (WeakThis.IsValid())
			{
				UPWLoadConnectingWidget* ConnectingWidget = Cast<UPWLoadConnectingWidget>(WeakThis.Get()->GetWidget(EUMGID::EUMGID_Connecting));
				if (ConnectingWidget)
				{
					ConnectingWidget->InitType(eLoadConnectType::eLoadConnectType_Connecting);
				}
			}
			});
		UPWUIManager::Get(this)->ShowUI(EUMGID::EUMGID_Connecting, UIOpenDelegate);
	}*/


#if PLATFORM_WINDOWS
	_LoginNormally();
#endif

	/*if (UPWSaveGameManager::Get(this) && UPWSaveGameManager::Get(this)->GetOtherData())
	{
		UPWSaveGameManager::Get(this)->GetOtherData()->SetSavedName(account);
	}*/
}

void UPWLoginWidget::OnSelectNextServer()
{
	AreaIndex++;
	if (AreaIndex >= DETNetClient::Instance().GetAreaList().Num())
	{
		AreaIndex = 0;
	}
	_UpdateServerName();
}

void UPWLoginWidget::OnSelectPreServer()
{
	if (AreaIndex == 0)
	{
		AreaIndex = DETNetClient::Instance().GetAreaList().Num() - 1;
	}
	else
	{
		AreaIndex--;
	}
	_UpdateServerName();
}

void UPWLoginWidget::_UpdateServerName()
{
	if (TextBlock_ServerName == nullptr)
		return;

	TArray<FAddrObj>& AreaList = DETNetClient::Instance().GetAreaList();
	check(AreaList.IsValidIndex(AreaIndex));
	FAddrObj& AreaInfo = AreaList[AreaIndex];
	TextBlock_ServerName->SetText(FText::FromString(AreaInfo.Name));
	DETNetClient::Instance().SetLoginUrl(AreaInfo.Addr);
}

void UPWLoginWidget::_LoginNormally()
{
#if !PLATFORM_PS4
	if (account.IsEmpty())
		return;

	UPWProcedureLogin* LoginProcedure = Cast<UPWProcedureLogin>(UPWProcedureManager::GetInstance(this)->GetProcedure(ProcedureState::ProcedureState_Login));
	if (LoginProcedure)
	{
		LoginProcedure->SetAccount(account);
	}
	TArray<FAddrObj>& AreaList = DETNetClient::Instance().GetAreaList();
	if (AreaList.IsValidIndex(AreaIndex) == false)
		return;
	bool PlusAccess = false;
	const FString CurLang = DHInternational::GetInstance()->GetDisplayLanguage();
	UPWClientNet::GetInstance(this)->LoginReq(account, TEXT(""), PlusAccess, CurLang);
#endif
}

void UPWLoginWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	//throw std::logic_error("The method or operation is not implemented.");
}

void UPWLoginWidget::MenuLeftPressed_Implementation()
{
	OnSelectPreServer();
	//UPWBlueprintLibrary::PlaySoundByUIAudioType(this, EWwiseUIAudioType::EWUI_MenuLeft_Login);
}

void UPWLoginWidget::MenuRightPressed_Implementation()
{
	OnSelectNextServer();
	//UPWBlueprintLibrary::PlaySoundByUIAudioType(this, EWwiseUIAudioType::EWUI_MenuRight_Login);
}

void UPWLoginWidget::MenuAcceptPressed_Implementation()
{
	OnLoginClicked();
	//UPWBlueprintLibrary::PlayConfirmSound(this);
}

void UPWLoginWidget::_InitAreaInfo()
{
}

void UPWLoginWidget::NativeConstruct()
{
	_InitAreaInfo();


	/*{
		if (UPWSaveGameManager::Get(this) && UPWSaveGameManager::Get(this)->GetOtherData())
		{
			account = UPWSaveGameManager::Get(this)->GetOtherData()->GetSavedName();
		}
	}*/

	ApplyBindings();
	Super::NativeConstruct();

	_UpdateServerName();
}

void UPWLoginWidget::L1Pressed_Implementation() {

	L1 = true;
}
void UPWLoginWidget::L2Pressed_Implementation() {

	L2 = true;
}
void UPWLoginWidget::R1Pressed_Implementation() {

	R1 = true;
}
void UPWLoginWidget::R2Pressed_Implementation() {

	R2 = true;
}

void UPWLoginWidget::L1Released_Implementation() {
	TryOpenLogWidget();
	L1 = false;
}

void UPWLoginWidget::L2Released_Implementation() {
	TryOpenLogWidget();
	L2 = false;
}

void UPWLoginWidget::R1Released_Implementation() {
	TryOpenLogWidget();
	R1 = false;
}

void UPWLoginWidget::R2Released_Implementation() {
	TryOpenLogWidget();
	R2 = false;
}

void UPWLoginWidget::TryOpenLogWidget() {
	if (L1 && L2 && R1 && R2)
	{
		//UPWLogManager::Get()->ShowLogWidget();
	}
}

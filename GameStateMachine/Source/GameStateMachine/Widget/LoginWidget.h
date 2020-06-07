// Copyright 2016 P906, Perfect World, Inc.

#pragma once

#include "CoreMinimal.h"
#include "Widget/PWWidgetBase.h"
#include "Procedure/PWProcedureBase.h"
#include "PWLoginWidget.generated.h"

/**
 *
 */
UCLASS()
class PWGAME_API UPWLoginWidget : public UPWWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LoginWidget")
		void OnLoginClicked();

	UFUNCTION(BlueprintCallable, Category = "LoginWidget")
		void OnSelectNextServer();

	UFUNCTION(BlueprintCallable, Category = "LoginWidget")
		void OnSelectPreServer();

protected:
	void _LoginNormally();

	void _InitAreaInfo();
	void _UpdateServerName();


protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void MenuLeftPressed_Implementation();
	virtual void MenuRightPressed_Implementation();
	virtual void MenuAcceptPressed_Implementation();

	UPROPERTY(BlueprintReadWrite, Category = "LoginWidget")
		FString		account;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UPWLoginWidget", meta = (BindWidgetOptional))
		class UTextBlock* TextBlock_ServerName;

	int32		AreaIndex = 0;
public:
	virtual void L1Pressed_Implementation();
	virtual void L2Pressed_Implementation();
	virtual void R1Pressed_Implementation();
	virtual void R2Pressed_Implementation();

	virtual void L1Released_Implementation();
	virtual void L2Released_Implementation();
	virtual void R1Released_Implementation();
	virtual void R2Released_Implementation();
private:
	void TryOpenLogWidget();
	bool L1 = false;
	bool L2 = false;
	bool R1 = false;
	bool R2 = false;
};

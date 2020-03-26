#pragma once

#include "BaseCommon.h"

class BASEPLUGIN_API DHInternational : public DHInstance<DHInternational, false, false, false>
{
	struct Impl;
	Impl* _Impl;
public:
	DHInternational();

	~DHInternational();

	DHInternational* SetDefaultLanguage(const FString& Language);

	DHInternational* AddLanguageFile(const std::initializer_list<FString>& Language, const FString& LanguageFile);

	void SetDisplayLanguage(const FString& Language);

	const FString GetDisplayLanguage();
	const FString GetISOLanguage();

	const FString GetSupportedLanguage(const FString& UserLanguage = {});

	void ResetAll();

	void End();

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDisplayLanguageUpdated, const FString&);
	FOnDisplayLanguageUpdated& GetOnDisplayLanguageUpdated();
};
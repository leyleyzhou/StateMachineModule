#include "International/BaseInternational.h"
#include "Internationalization.h"
#include "Internationalization/Culture.h"
#include "StringTableRegistry.h"
#include "StringTableCore.h"
#include "StringTable.h"
#include "Paths.h"

struct DHInternational::Impl
{
	TMap<FString, FString> LanguageFiles;
	FString DefaultLanguage;
	FString CurrentLanguage;
	FString CurrentLanguageISO;
	FDelegateHandle Handle;

	FOnDisplayLanguageUpdated OnDisplayLanguageUpdatedEvent;

	void ChangeLanguage(const FString& Language);

	const FString GetSupportedLanguage(const FString& UserLanguage);

	/**@zpj Internal function called by LOCTABLE_FROMFILE_X to create and register a FStringTable instance that has been populated from a folder */
	void DHInternal_LocTableFromPath(const FName InTableId, const FString& InNamespace, const FString& InFilePath, const FString& InRootPath);
};

DHInternational::DHInternational()
	:_Impl(new Impl)
{
	_Impl->DefaultLanguage = TEXT("zh-Hans");

	_Impl->Handle = FInternationalization::Get().OnCultureChanged().AddLambda([this] {
		_Impl->ChangeLanguage(FInternationalization::Get().GetCurrentCulture()->GetName());
		});
}

DHInternational::~DHInternational()
{
	FInternationalization::Get().OnCultureChanged().Remove(_Impl->Handle);

	safe_delete(_Impl);
}

DHInternational* DHInternational::SetDefaultLanguage(const FString& Language)
{
	_Impl->DefaultLanguage = Language;

	return this;
}

DHInternational* DHInternational::AddLanguageFile(const std::initializer_list<FString>& Languages, const FString& LanguageFile)
{
	for (const FString& Language : Languages) {
		_Impl->LanguageFiles.Add(Language, LanguageFile);
	}

	return this;
}

void DHInternational::End()
{
	_Impl->ChangeLanguage(FInternationalization::Get().GetCurrentCulture()->GetName());
}

DHInternational::FOnDisplayLanguageUpdated& DHInternational::GetOnDisplayLanguageUpdated()
{
	return _Impl->OnDisplayLanguageUpdatedEvent;
}

void DHInternational::ResetAll()
{
	_Impl->CurrentLanguage.Empty();
	FStringTableRegistry::Get().UnregisterStringTable(TEXT("Lang"));
}

void DHInternational::SetDisplayLanguage(const FString& Language)
{
	FInternationalization::Get().SetCurrentCulture(Language);
}

const FString DHInternational::GetSupportedLanguage(const FString& UserLanguage)
{
	return _Impl->GetSupportedLanguage(UserLanguage);
}

const FString DHInternational::Impl::GetSupportedLanguage(const FString& UserLanguage)
{
	return LanguageFiles.Contains(UserLanguage) ? UserLanguage : DefaultLanguage;
}

void DHInternational::Impl::ChangeLanguage(const FString& WantLanguage)
{
	FString Language = GetSupportedLanguage(WantLanguage);
	return_if_true(Language == CurrentLanguage);
	CurrentLanguage = Language;

	CurrentLanguageISO = FInternationalization::Get().GetCulture(Language)->GetTwoLetterISOLanguageName();


	FStringTableRegistry::Get().UnregisterStringTable(TEXT("Lang"));
	DHInternal_LocTableFromPath(TEXT("Lang"), TEXT("Lang"), LanguageFiles[Language], FPaths::ProjectContentDir());
	//FStringTableRegistry::Get().Internal_LocTableFromFile(TEXT("Lang"), TEXT("Lang"), LanguageFiles[Language], FPaths::ProjectContentDir());
	if (OnDisplayLanguageUpdatedEvent.IsBound())
	{
		OnDisplayLanguageUpdatedEvent.Broadcast(CurrentLanguage);
	}
}

const FString DHInternational::GetDisplayLanguage()
{
	return _Impl->CurrentLanguage;
}

const FString DHInternational::GetISOLanguage()
{
	if (_Impl->CurrentLanguage.Equals(TEXT("zh-Hans"), ESearchCase::IgnoreCase))
	{
		return TEXT("zh-CN");
	}
	if (_Impl->CurrentLanguage.Equals(TEXT("zh-Hant"), ESearchCase::IgnoreCase))
	{
		return TEXT("zh-TW");
	}
	return _Impl->CurrentLanguage;
}
// include DH LocTable.
#include "BaseInternational_LocTable.inl"
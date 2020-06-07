// Copyright 2004-2019 Perfect World Co.,Ltd. All Rights Reserved.
// @author Yongquan Fan, create on 2019/5/14.

#pragma once

#include "DHCommon.h"

namespace ELicenseAreaPS4
{
	enum class Type : uint8
	{
		Unknown,
		SIEJA = 1,          // 亚太
		SIEA,               // 美区
		SIEE                // 欧区
	};

	inline const TCHAR* ToString(Type Ty)
	{
		switch (Ty) {
		case Type::SIEA:
			return TEXT("SIEA");
		case Type::SIEE:
			return TEXT("SIEE");
		case Type::SIEJA:
			return TEXT("SIEJA");
		default:
			return TEXT("Unknown");
		}
	}
}

struct FCountryPS4
{
	const FString CountryCode;

	const FString CountryName;

	const FString Language;

	const FString Currency;

	const FString CurrencyCode;

	const ELicenseAreaPS4::Type LicenseArea = ELicenseAreaPS4::Type::Unknown;

	FCountryPS4(const FString& CountryCode_, const FString& CountryName_, const FString& Language_, const FString& Currency_, const FString& CurrencyCode_, ELicenseAreaPS4::Type LicenseArea_)
		:CountryCode(CountryCode_), CountryName(CountryName_), Language(Language_), Currency(Currency_), CurrencyCode(CurrencyCode_), LicenseArea(LicenseArea_)
	{}
};

PWGAME_API FCountryPS4 GetCountryByCodePS4(const FString& CountryCode);
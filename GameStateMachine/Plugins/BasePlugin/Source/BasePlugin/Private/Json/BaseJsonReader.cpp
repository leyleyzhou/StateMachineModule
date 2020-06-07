#include "Json/BaseJsonUtils.h"
#include "TextProperty.h"
#include "PropertyPortFlags.h"
#include "json.hpp"

namespace DHJsonUtils
{
	using JSON = nlohmann::json;

	enum class EReadResult : int8
	{
		Ok,
		Ignore,
		Error
	};

	static EReadResult ReadStructJsonValue(UScriptStruct* InStruct, JSON& Json, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult ReadJsonValue(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadEnum(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadNumeric(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadBool(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadString(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadArray(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadMap(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadSet(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadText(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadStruct(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);
	static EReadResult TryReadJsonObject(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias);

	bool ReadFromJson(UScriptStruct* InStruct, const FString& JsonString, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		JSON Json = JSON::parse(TCHAR_TO_UTF8(*JsonString), nullptr, false);
		if (Json.is_discarded()) {
			return false;
		}
		return ReadStructJsonValue(InStruct, Json, OutStruct, FieldAlias) == EReadResult::Ok;
	}

	static EReadResult ReadStructJsonValue(UScriptStruct* InStruct, JSON& Json, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		for (TFieldIterator<UProperty> Iter(InStruct); Iter; ++Iter) {
			UProperty* Property = *Iter;

			const FString& FieldName = Property->GetName();
			auto LowerName = FieldAlias.Find(FieldName.ToLower());

			auto IterValue = Json.find(LowerName == nullptr ? TCHAR_TO_UTF8(*FieldName) : TCHAR_TO_UTF8(*(*LowerName)));
			if (IterValue == Json.end()) {
				continue;
			}

			void* Ptr = Property->ContainerPtrToValuePtr<uint8>(OutStruct);
			if (ReadJsonValue(Property, IterValue.value(), Ptr, FieldAlias) == EReadResult::Error) {
				return EReadResult::Error;
			}
		}

		return EReadResult::Ok;
	}

	static EReadResult ReadJsonValue(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		EReadResult Result = TryReadEnum(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadNumeric(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadBool(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadString(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadArray(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadMap(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadSet(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadText(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadStruct(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		Result = TryReadJsonObject(Property, Value, OutStruct, FieldAlias);
		if (Result != EReadResult::Ignore) {
			return Result;
		}

		if (Value.is_string() && Property->ImportText(UTF8_TO_TCHAR(Value.get<std::string>().data()), OutStruct, 0, nullptr) != nullptr) {
			return EReadResult::Ok;
		}

		return EReadResult::Error;
	}

	static EReadResult TryReadJsonObject(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UObjectProperty* ObjectProperty = Cast<UObjectProperty>(Property);
		if (ObjectProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (!Value.is_string()) {
			return EReadResult::Error;
		}

		if (Property->ImportText(UTF8_TO_TCHAR(Value.get<std::string>().data()), OutStruct, 0, nullptr) != nullptr) {
			return EReadResult::Ok;
		}

		return EReadResult::Error;
	}

	static EReadResult TryReadEnum(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UEnumProperty* EnumProperty = Cast<UEnumProperty>(Property);
		if (EnumProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (Value.is_number_integer()) {
			EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutStruct, Value.get<int64>());
			return EReadResult::Ok;
		}

		if (Value.is_string()) {
			const UEnum* Enum = EnumProperty->GetEnum();
			check(Enum);
			std::string StrValue = Value.get<std::string>();
			int64 IntValue = Enum->GetValueByName(FName(UTF8_TO_TCHAR(StrValue.data())));
			if (IntValue == INDEX_NONE) {
				return EReadResult::Error;
			}

			EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutStruct, IntValue);
			return EReadResult::Ok;
		}

		return EReadResult::Ignore;
	}

	static EReadResult TryReadNumeric(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UNumericProperty* NumericProperty = Cast<UNumericProperty>(Property);
		if (NumericProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (NumericProperty->IsEnum() && Value.is_string()) {
			const UEnum* Enum = NumericProperty->GetIntPropertyEnum();
			auto StrValue = Value.get<std::string>();
			int64 IntValue = Enum->GetValueByName(FName(UTF8_TO_TCHAR(StrValue.data())));
			if (IntValue == INDEX_NONE) {
				return EReadResult::Error;
			}

			NumericProperty->SetIntPropertyValue(OutStruct, IntValue);
			return EReadResult::Ok;
		}

		if (NumericProperty->IsFloatingPoint() && Value.is_number()) {
			NumericProperty->SetFloatingPointPropertyValue(OutStruct, Value.get<double>());
			return EReadResult::Ok;
		}

		if (NumericProperty->IsInteger() && Value.is_number_integer()) {
			NumericProperty->SetIntPropertyValue(OutStruct, Value.get<int64>());
			return EReadResult::Ok;
		}

		return EReadResult::Ignore;
	}

	static EReadResult TryReadBool(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UBoolProperty* BoolProperty = Cast<UBoolProperty>(Property);
		if (BoolProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (!Value.is_boolean()) {
			return EReadResult::Error;
		}

		BoolProperty->SetPropertyValue(OutStruct, Value.get<bool>());

		return EReadResult::Ok;
	}

	static EReadResult TryReadString(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UStrProperty* StringProperty = Cast<UStrProperty>(Property);
		if (StringProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (!Value.is_string()) {
			return EReadResult::Error;
		}

		StringProperty->SetPropertyValue(OutStruct, UTF8_TO_TCHAR(Value.get<std::string>().data()));
		return EReadResult::Ok;
	}

	static EReadResult TryReadString(UProperty* Property, const std::string& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UStrProperty* StringProperty = Cast<UStrProperty>(Property);
		if (StringProperty == nullptr) {
			return EReadResult::Ignore;
		}

		StringProperty->SetPropertyValue(OutStruct, UTF8_TO_TCHAR(Value.data()));
		return EReadResult::Ok;
	}

	static EReadResult TryReadArray(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Property);
		if (ArrayProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (!Value.is_array()) {
			return EReadResult::Error;
		}

		size_t Size = Value.size();
		FScriptArrayHelper Helper(ArrayProperty, OutStruct);
		Helper.Resize(Size);

		for (int32 i = 0; i < Size; ++i) {
			JSON& Element = Value.at(i);
			if (Element.is_null() || ReadJsonValue(ArrayProperty->Inner, Element, Helper.GetRawPtr(i), FieldAlias) != EReadResult::Ok) {
				return EReadResult::Error;
			}
		}

		return EReadResult::Ok;
	}

	static EReadResult TryReadMap(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UMapProperty* MapProperty = Cast<UMapProperty>(Property);
		if (MapProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (!Value.is_object()) {
			return EReadResult::Error;
		}

		FScriptMapHelper Helper(MapProperty, OutStruct);

		for (auto& Pair : Value.items()) {
			if (Pair.value().is_null()) {
				continue;
			}

			int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();

			if (TryReadString(MapProperty->KeyProp, Pair.key(), Helper.GetKeyPtr(NewIndex), FieldAlias) != EReadResult::Ok) {
				return EReadResult::Error;
			}

			if (ReadJsonValue(MapProperty->ValueProp, Pair.value(), Helper.GetValuePtr(NewIndex), FieldAlias) != EReadResult::Ok) {
				return EReadResult::Error;
			}
		}

		Helper.Rehash();

		return EReadResult::Ok;
	}

	static EReadResult TryReadSet(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		USetProperty* SetProperty = Cast<USetProperty>(Property);
		if (SetProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (!Value.is_array()) {
			return EReadResult::Error;
		}

		size_t Size = Value.size();
		FScriptSetHelper Helper(SetProperty, OutStruct);

		for (int32 i = 0; i < Size; ++i) {
			JSON& Element = Value.at(i);
			int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();

			if (Element.is_null() || ReadJsonValue(SetProperty->ElementProp, Element, Helper.GetElementPtr(i), FieldAlias) != EReadResult::Ok) {
				return EReadResult::Error;
			}
		}

		Helper.Rehash();

		return EReadResult::Ok;
	}

	static EReadResult TryReadText(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UTextProperty* TextProperty = Cast<UTextProperty>(Property);
		if (TextProperty == nullptr) {
			return EReadResult::Ignore;
		}

		if (!Value.is_string()) {
			return EReadResult::Error;
		}

		TextProperty->SetPropertyValue(OutStruct, FText::FromString(UTF8_TO_TCHAR(Value.get<std::string>().data())));

		return EReadResult::Ok;
	}

	static EReadResult TryReadStruct(UProperty* Property, JSON& Value, void* OutStruct, const TMap<const FString, const FString>& FieldAlias)
	{
		UStructProperty* StructProperty = Cast<UStructProperty>(Property);
		if (StructProperty == nullptr) {
			return EReadResult::Ignore;
		}

		static const FName NAME_DateTime(TEXT("DateTime"));
		static const FName NAME_Color(TEXT("Color"));
		static const FName NAME_LinearColor(TEXT("LinearColor"));

		if (Value.is_object()) {
			return ReadStructJsonValue(StructProperty->Struct, Value, OutStruct, FieldAlias);
		}

		if (!Value.is_string()) {
			return EReadResult::Error;
		}

		if (StructProperty->Struct->GetFName() == NAME_LinearColor) {
			FLinearColor& ColorOut = *(FLinearColor*)OutStruct;
			std::string ColorString = Value.get<std::string>();

			FColor IntermediateColor;
			IntermediateColor = FColor::FromHex(UTF8_TO_TCHAR(Value.get<std::string>().data()));
			ColorOut = IntermediateColor;

			return EReadResult::Ok;
		}

		if (StructProperty->Struct->GetFName() == NAME_Color) {
			*(FColor*)OutStruct = FColor::FromHex(UTF8_TO_TCHAR(Value.get<std::string>().data()));
			return EReadResult::Ok;
		}

		if (StructProperty->Struct->GetFName() == NAME_DateTime) {
			FString DateString = UTF8_TO_TCHAR(Value.get<std::string>().data());
			FDateTime& DateTimeOut = *(FDateTime*)OutStruct;
			if (DateString == TEXT("min")) {
				DateTimeOut = FDateTime::MinValue();
				return EReadResult::Ok;
			}
			if (DateString == TEXT("max")) {
				DateTimeOut = FDateTime::MaxValue();
				return EReadResult::Ok;
			}
			if (DateString == TEXT("now")) {
				DateTimeOut = FDateTime::UtcNow();
				return EReadResult::Ok;
			}
			if (FDateTime::ParseIso8601(*DateString, DateTimeOut) || (FDateTime::Parse(DateString, DateTimeOut))) {
				return EReadResult::Ok;
			}
			else {
				return EReadResult::Error;
			}
		}

		if (StructProperty->Struct->GetCppStructOps() && StructProperty->Struct->GetCppStructOps()->HasImportTextItem()) {
			UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
			const TCHAR* ImportTextPtr = UTF8_TO_TCHAR(Value.get<std::string>().data());
			if (!TheCppStructOps->ImportTextItem(ImportTextPtr, OutStruct, PPF_None, nullptr, (FOutputDevice*)GWarn)) {
				Property->ImportText(ImportTextPtr, OutStruct, PPF_None, nullptr);
			}
			return EReadResult::Ok;
		}

		const TCHAR* ImportTextPtr = UTF8_TO_TCHAR(Value.get<std::string>().data());
		Property->ImportText(ImportTextPtr, OutStruct, PPF_None, nullptr);

		return EReadResult::Ok;
	}
}
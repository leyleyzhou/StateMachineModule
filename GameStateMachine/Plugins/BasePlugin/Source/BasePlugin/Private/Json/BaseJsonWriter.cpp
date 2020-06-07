#include "Json/BaseJsonUtils.h"
#include "JsonObjectWrapper.h"
#include "TextProperty.h"
#include "PropertyPortFlags.h"
#include "json.hpp"

namespace DHJsonUtils
{
	using JSON = nlohmann::json;

	static void WriteToJson(UScriptStruct* InStruct, void* Object, JSON& Json, const TMap<const FString, const FString>& FieldAlias);
	static bool ToJsonValue(UProperty* Property, void* Value, bool IgnoreDefaultValue, JSON& Json, const TMap<const FString, const FString>& FieldAlias);

	void WriteToJson(UScriptStruct* InStruct, void* Object, std::string& OutJsonString, const TMap<const FString, const FString>& FieldAlias)
	{
		JSON Json;
		WriteToJson(InStruct, Object, Json, FieldAlias);
		OutJsonString = Json.dump();
	}

	static void WriteToJson(UScriptStruct* InStruct, void* Object, JSON& Json, const TMap<const FString, const FString>& FieldAlias)
	{
		for (TFieldIterator<UProperty> Iter(InStruct); Iter; ++Iter) {
			UProperty* Property = *Iter;

			const FString& FieldName = Property->GetName();
			auto LowerName = FieldAlias.Find(FieldName.ToLower());

			auto FieldValue = Property->ContainerPtrToValuePtr<uint8*>(Object);
			JSON Value;
			if (ToJsonValue(Property, FieldValue, true, Value, FieldAlias)) {
				Json[LowerName == nullptr ? TCHAR_TO_UTF8(*FieldName) : TCHAR_TO_UTF8(*(*LowerName))] = MoveTemp(Value);
			}
		}
	}

	static bool ToJsonValue(UProperty* Property, void* RawValue, bool IgnoreDefaultValue, JSON& Json, const TMap<const FString, const FString>& FieldAlias)
	{
		if (UStrProperty* StringProperty = Cast<UStrProperty>(Property)) {
			auto& StringValue = StringProperty->GetPropertyValue(RawValue);
			return_false_if_true(IgnoreDefaultValue && StringValue.IsEmpty());

			Json = TCHAR_TO_UTF8(*StringValue);

			return true;
		}

		if (UNumericProperty* NumericProperty = Cast<UNumericProperty>(Property)) {
			if (UEnum* EnumDef = NumericProperty->GetIntPropertyEnum()) {
				int64 Value = NumericProperty->GetSignedIntPropertyValue(RawValue);
				FString StringValue = EnumDef->GetNameStringByValue(Value);
				if (StringValue.IsEmpty()) {
					Json = Value;
				}
				else {
					Json = TCHAR_TO_UTF8(*StringValue);
				}
				return true;
			}

			if (NumericProperty->IsFloatingPoint()) {
				Json = NumericProperty->GetFloatingPointPropertyValue(RawValue);
			}
			else {
				int64 Value = NumericProperty->GetSignedIntPropertyValue(RawValue);
				return_false_if_true(IgnoreDefaultValue && Value == 0);

				Json = Value;
			}

			return true;
		}

		if (UBoolProperty* BoolProperty = Cast<UBoolProperty>(Property)) {
			bool Value = BoolProperty->GetPropertyValue(RawValue);
			return_false_if_false(IgnoreDefaultValue && Value);

			Json = Value;
			return true;
		}

		if (UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Property)) {
			FScriptArrayHelper Helper(ArrayProperty, RawValue);
			return_false_if_true(IgnoreDefaultValue && Helper.Num() == 0);

			Json = JSON::array();
			for (int32 i = 0, n = Helper.Num(); i < n; ++i) {
				JSON Value;
				if (ToJsonValue(ArrayProperty->Inner, Helper.GetRawPtr(i), false, Value, FieldAlias)) {
					Json.emplace_back(Value);
				}
			}

			return true;
		}

		if (UMapProperty* MapProperty = Cast<UMapProperty>(Property)) {
			FScriptMapHelper Helper(MapProperty, RawValue);
			return_false_if_true(IgnoreDefaultValue && Helper.Num() == 0);

			Json = JSON::object();
			for (int32 i = 0, n = Helper.Num(); n; ++i) {
				if (Helper.IsValidIndex(i)) {
					if (UStrProperty* KeyPropery = Cast<UStrProperty>(MapProperty->KeyProp)) {
						JSON Value;
						if (ToJsonValue(MapProperty->ValueProp, Helper.GetValuePtr(i), false, Value, FieldAlias)) {
							auto PairKey = KeyPropery->GetPropertyValue(Helper.GetKeyPtr(i));
							Json[TCHAR_TO_UTF8(*PairKey)] = MoveTemp(Value);
						}
					}
					--n;
				}
			}

			return true;
		}

		if (UStructProperty* StructProperty = Cast<UStructProperty>(Property)) {
			UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
			if (!(StructProperty->Struct != FJsonObjectWrapper::StaticStruct() && TheCppStructOps && TheCppStructOps->HasExportTextItem())) {
				WriteToJson(StructProperty->Struct, RawValue, Json, FieldAlias);
				return true;
			}
		}

		if (UTextProperty* TextProperty = Cast<UTextProperty>(Property)) {
			auto& StringValue = TextProperty->GetPropertyValue(RawValue).ToString();
			return_false_if_true(IgnoreDefaultValue && StringValue.IsEmpty());
			Json = TCHAR_TO_UTF8(*StringValue);
			return true;
		}

		if (USetProperty* SetProperty = Cast<USetProperty>(Property)) {
			FScriptSetHelper Helper(SetProperty, RawValue);
			int32 Size = Helper.Num();
			return_false_if_true(IgnoreDefaultValue && Size == 0);

			Json = JSON::array();
			for (int32 i = 0; i < Size; ++i) {
				if (Helper.IsValidIndex(i)) {
					JSON Value;
					if (ToJsonValue(SetProperty->ElementProp, Helper.GetElementPtr(i), false, Value, FieldAlias)) {
						Json.emplace_back(Value);
					}
				}
			}

			return true;
		}

		if (UEnumProperty* EnumProperty = Cast<UEnumProperty>(Property)) {
			UEnum* EnumDef = EnumProperty->GetEnum();
			int64 Value = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(RawValue);
			FString StringValue = EnumDef->GetNameStringByValue(Value);
			if (StringValue.IsEmpty()) {
				Json = Value;
			}
			else {
				Json = TCHAR_TO_UTF8(*StringValue);
			}
			return true;
		}

		FString OutValueStr;
		Property->ExportTextItem(OutValueStr, RawValue, nullptr, nullptr, PPF_None, nullptr);
		return_false_if_true(IgnoreDefaultValue && OutValueStr.IsEmpty());

		Json = TCHAR_TO_UTF8(*OutValueStr);
		return true;
	}
};
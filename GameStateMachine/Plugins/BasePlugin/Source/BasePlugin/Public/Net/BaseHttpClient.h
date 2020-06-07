#pragma once

#include "BaseCommon.h"

enum class EDHHttpResultCode : int32
{
	Ok = 0,
	HttpRequestError = 100,
	HttpLostContextError,
	HttpResponseError
};

enum class EDHHttpMethod : int8
{
	Get,
	Post
};

struct DHHttpOptions
{
	int32 RetryTimes{ 3 };

	int32 TimeOutInSeconds{ 30 };
};

struct DHHttpField
{
	const FString Name;

	const FString Value;

	DHHttpField(const FString& name, const int value)
		: Name(name), Value(LexToString(value))
	{}

	DHHttpField(const FString& name, const float value)
		: Name(name), Value(FString::SanitizeFloat(value))
	{}

	DHHttpField(const FString& name, const FString& value)
		: Name(name), Value(value)
	{}
};

typedef TFunction<void(EDHHttpResultCode, int32, const TArray<uint8>&)> DHHttpBinaryCallback;

typedef TFunction<void(EDHHttpResultCode, int32, const FString&)> DHHttpStringCallback;

static const FString EDHHttpMethodName(const EDHHttpMethod httpMethod)
{
	switch (httpMethod) {
	case EDHHttpMethod::Post:
		return "POST";
	default:
		return "GET";
	}
}

class BASEPLUGIN_API DHHttpClient : public DHInstance<DHHttpClient>
{
	struct Impl;
	Impl* _Impl;
public:
	DHHttpClient();

	virtual ~DHHttpClient();

	DHHttpClient* SetOptions(const DHHttpOptions& options);

	DHHttpClient* SetDefaultHeader(const FString& name, const FString& value);

	DHHttpClient* RemoveDefaultHeader(const FString& name);

	DHHttpClient* EmptyHeaders();

	DHHttpClient* RequestBinaryContent(const EDHHttpMethod httpMethod, const FString& url, const std::initializer_list<DHHttpField>& fields, DHHttpBinaryCallback callback);

	DHHttpClient* RequestStringContent(const EDHHttpMethod httpMethod, const FString& url, const std::initializer_list<DHHttpField>& fields, DHHttpStringCallback callback);

	inline DHHttpClient* RequestBinaryContent(const EDHHttpMethod httpMethod, const FString& url, DHHttpBinaryCallback callback)
	{
		return RequestBinaryContent(httpMethod, url, {}, MoveTemp(callback));
	}

	inline DHHttpClient* RequestStringContent(const EDHHttpMethod httpMethod, const FString& url, DHHttpStringCallback callback)
	{
		return RequestStringContent(httpMethod, url, {}, MoveTemp(callback));
	}

	inline DHHttpClient* Request(const EDHHttpMethod httpMethod, const FString& url)
	{
		return RequestBinaryContent(httpMethod, url, {}, nullptr);
	}
};

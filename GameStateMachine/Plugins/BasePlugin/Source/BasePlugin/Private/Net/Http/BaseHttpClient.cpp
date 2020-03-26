#include "BaseHttpClient.h"
#include "HttpModule.h"
#include "HttpRetrySystem.h"
#include "IHttpRequest.h"
#include "IHttpResponse.h"
#include "PlatformHttp.h"
#include "Ticker.h"

struct DHHttpClient::Impl
{
	FHttpRetrySystem::FManager* HttpManager = nullptr;

	TArray<uint8> EmptyArray;

	FString EmptyString;

	TMap<FString, FString> Headers;

	TSet<FHttpRequestPtr> Requests;

	FDelegateHandle TickHandle;

	Impl()
	{
		DHHttpOptions options;

		InitOrCreateManager(options);
	}

	~Impl()
	{
		if (TickHandle.IsValid()) {
			FTicker::GetCoreTicker().RemoveTicker(TickHandle);
			TickHandle.Reset();
		}

		safe_delete(HttpManager);
	}

	void InitOrCreateManager(const DHHttpOptions& options);

	void Request(const EDHHttpMethod httpMethod,
		const FString& url,
		const std::initializer_list<DHHttpField>& fields,
		DHHttpBinaryCallback binaryCb,
		DHHttpStringCallback stringCb);

	void OnRequestCompleted(FHttpRequestPtr request, FHttpResponsePtr response, bool successful, DHHttpBinaryCallback binaryCb, DHHttpStringCallback stringCb);
};

DHHttpClient::DHHttpClient() : _Impl(new Impl) {}

DHHttpClient::~DHHttpClient()
{
	safe_delete(_Impl);
}

void DHHttpClient::Impl::InitOrCreateManager(const DHHttpOptions& options)
{
	if (HttpManager) {
		DHHttpOptions defaultOptions;
		if (defaultOptions.TimeOutInSeconds == options.TimeOutInSeconds) {
			HttpManager->SetDefaultRetryLimit(options.RetryTimes);
			return;
		}

		safe_delete(HttpManager);
	}

	HttpManager = new FHttpRetrySystem::FManager(FHttpRetrySystem::RetryLimitCount(options.RetryTimes),
		FHttpRetrySystem::RetryTimeoutRelativeSeconds(options.TimeOutInSeconds));

	if (!TickHandle.IsValid()) {
		TickHandle = FTicker::GetCoreTicker().AddTicker(TEXT("HttpManagerTick"), 0.5f, [this](float DeltaTime) -> bool {
			if (HttpManager) {
				HttpManager->Update();
			}
			return true;
			});
	}
}

DHHttpClient* DHHttpClient::SetOptions(const DHHttpOptions& options)
{
	_Impl->InitOrCreateManager(options);

	return this;
}

DHHttpClient* DHHttpClient::SetDefaultHeader(const FString& name, const FString& value)
{
	_Impl->Headers[name] = value;

	return this;
}

DHHttpClient* DHHttpClient::RemoveDefaultHeader(const FString& name)
{
	_Impl->Headers.Remove(name);

	return this;
}

DHHttpClient* DHHttpClient::EmptyHeaders()
{
	_Impl->Headers.Empty();

	return this;
}

DHHttpClient* DHHttpClient::RequestBinaryContent(const EDHHttpMethod httpMethod,
	const FString& url,
	const std::initializer_list<DHHttpField>& fields,
	DHHttpBinaryCallback callback)
{
	_Impl->Request(httpMethod, url, fields, MoveTemp(callback), nullptr);

	return this;
}

DHHttpClient* DHHttpClient::RequestStringContent(const EDHHttpMethod httpMethod,
	const FString& url,
	const std::initializer_list<DHHttpField>& fields,
	DHHttpStringCallback callback)
{
	_Impl->Request(httpMethod, url, fields, nullptr, MoveTemp(callback));

	return this;
}

void DHHttpClient::Impl::Request(const EDHHttpMethod HttpMethod,
	const FString& Url,
	const std::initializer_list<DHHttpField>& Fields,
	DHHttpBinaryCallback BinaryCb,
	DHHttpStringCallback StringCb)
{
	auto Request = HttpManager->CreateRequest();

	Request->SetVerb(EDHHttpMethodName(HttpMethod));

	if (Headers.Num() > 0) {
		for (auto& iter : Headers) {
			Request->SetHeader(iter.Key, iter.Value);
		}
	}

	if (BinaryCb != nullptr || StringCb != nullptr) {
		Requests.Add(Request);
		Request->OnProcessRequestComplete().BindRaw(this, &Impl::OnRequestCompleted, MoveTemp(BinaryCb), MoveTemp(StringCb));
	}

	if (Fields.size() == 0) {
		Request->SetURL(Url);
		UE_LOG(LogTemp, Display, TEXT("HttpClient [%s], Url=%s"), *Request->GetVerb(), *Url);
	}
	else {
		FString Content;

		for (auto& Field : Fields) {
			Content.Append(FPlatformHttp::UrlEncode(Field.Name));
			Content.Append(TEXT("="));
			Content.Append(FPlatformHttp::UrlEncode(Field.Value));
			Content.Append(TEXT("&"));
		}

		if (HttpMethod == EDHHttpMethod::Post) {
			Request->SetURL(Url);
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
			Request->SetContentAsString(Content);
		}
		else {
			int32 Index = INDEX_NONE;
			Request->SetURL(Url + (Url.FindChar('?', Index) ? "&" : "?") + Content);
		}

		UE_LOG(LogTemp, Display, TEXT("HttpClient [%s], Url=%s, Content=%s"), *Request->GetVerb(), *Request->GetURL(), *Content);
	}


	return_if_true(Request->ProcessRequest());

	if (BinaryCb != nullptr) {
		BinaryCb(EDHHttpResultCode::HttpRequestError, 0, EmptyArray);
		return;
	}

	if (StringCb != nullptr) {
		StringCb(EDHHttpResultCode::HttpRequestError, 0, EmptyString);
		return;
	}
}

void DHHttpClient::Impl::OnRequestCompleted(FHttpRequestPtr Request,
	FHttpResponsePtr Response,
	bool Successful,
	DHHttpBinaryCallback BinaryCb,
	DHHttpStringCallback StringCb)
{
	return_if_true(Requests.Remove(Request) == 0);

	auto ResultCode = EDHHttpResultCode::Ok;
	int32 ResponseCode = 0; // default code zero.

	do {
		if (!Response.IsValid()) {
			ResultCode = EDHHttpResultCode::HttpLostContextError;
			break;
		}

		ResponseCode = Response->GetResponseCode();

		if (!Successful || ResponseCode < EHttpResponseCodes::Ok || ResponseCode >= EHttpResponseCodes::BadRequest) {
			ResultCode = EDHHttpResultCode::HttpResponseError;
			break;
		}
	} while (false);

	if (BinaryCb != nullptr) {
		BinaryCb(ResultCode, ResponseCode, ResultCode == EDHHttpResultCode::Ok ? Response->GetContent() : EmptyArray);
	}

	if (StringCb != nullptr) {
		StringCb(ResultCode, ResponseCode, ResultCode == EDHHttpResultCode::Ok ? Response->GetContentAsString() : EmptyString);
	}
}
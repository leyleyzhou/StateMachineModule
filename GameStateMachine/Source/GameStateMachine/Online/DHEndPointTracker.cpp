#include "DHEndpointTracker.h"
#include "PlatformHttp.h"
#include "HttpModule.h"
#include "Ticker.h"

struct DHEndpointTracker::Impl
{
	FString Url;
	FString AdverID;
	FString UserID;

	double LastSendAt = 0;

	void HttpTrack(const FString&);
};

DHEndpointTracker::DHEndpointTracker()
	:_Impl(new Impl)
{
	FHttpModule::Get().SetHttpTimeout(10);
	_Impl->LastSendAt = FPlatformTime::Seconds();
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime) -> bool {
		if (FPlatformTime::Seconds() - _Impl->LastSendAt > 10) {
			TrackEvent(EEndpointTrackEvent::TE_Tick, {});
		}
		return true;
		}), 5);
}

DHEndpointTracker::~DHEndpointTracker()
{
	safe_delete(_Impl);
}

void DHEndpointTracker::Init(const FString& Url, FString& AdverID)
{
	_Impl->Url = Url;
	_Impl->AdverID = AdverID;
}

void DHEndpointTracker::SetUserID(const FString& UserID)
{
	_Impl->UserID = UserID;
}

void DHEndpointTracker::TrackEvent(enum EEndpointTrackEvent Event, const std::initializer_list<DHTrackParam>& Fields)
{
	_Impl->LastSendAt = FPlatformTime::Seconds();
#if UE_BUILD_SHIPPING
	FString Data = FString::Printf(TEXT("t=%u"), (uint32)Event);
#else
	FString Data = FString::Printf(TEXT("mode=debug&t=%u"), (uint32)Event);
#endif

	for (auto& Field : Fields) {
		Data.Append(TEXT("&"));
		Data.Append(Field.Value);
	}

#if PLATFORM_PS4
	_Impl->HttpTrack(Data);
#else
	if (Event != EEndpointTrackEvent::TE_Tick) {
		UE_LOG(LogTemp, Display, TEXT("EndpointTracker, Url=%s, Data=%s"), *_Impl->Url, *Data);
	}
#endif
}

void DHEndpointTracker::Impl::HttpTrack(const FString& Data)
{
	return_if_true(Url.IsEmpty());

	auto Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded; charset=utf-8"));
	Request->SetHeader(TEXT("AdverID"), AdverID);
	Request->SetHeader(TEXT("UserID"), UserID);
	Request->SetVerb(TEXT("POST"));
	Request->SetContentAsString(Data);
	Request->ProcessRequest();
}
#pragma once

#include "CoreMinimal.h"
#include "BaseCommon.h"
#include "PlatformHttp.h"

enum EEndpointTrackEvent : uint32
{
	TE_StartSession = 1,
	TE_HasError = 2,
	TE_Tick = 3,
	TE_EndVideo = 4,
	TE_StartLogin = 5,
	TE_EndLogin = 6,
	TE_StartGetServerList = 7,
	TE_EndGetServerList = 8,
	TE_StartUpdate = 9,
	TE_EndUpdate = 10,
	TE_StartEntry = 11,
	TE_StartLobbyVideo = 12,
	TE_EndLobbyVideo = 13,
	TE_StartCreateCharacter = 14,
	TE_EndCreateCharacter = 15,
	TE_InLobby = 16,
	TE_TrackEvent = 17
};

enum ETrackFieldName : uint32
{
	Device = 1,
	IsNewUser = 2,
	IsPlus = 3,
	Code = 4,
	UserID = 5,
	Url = 6,
	Name = 7,
	Sex = 8,
	Skip
};

struct DHTrackParam
{
	const FString Value;

	DHTrackParam(enum ETrackFieldName FieldName, const int FieldValue)
		: Value(FString::Printf(TEXT("%u=%u"), (uint32)FieldName, FieldValue))
	{}

	DHTrackParam(enum ETrackFieldName FieldName, const bool FieldValue)
		: Value(FString::Printf(TEXT("%u=%d"), (uint32)FieldName, FieldValue ? 1 : 0))
	{}

	DHTrackParam(enum ETrackFieldName FieldName, const float FieldValue)
		: Value(FString::Printf(TEXT("%u=%.2f"), (uint32)FieldName, FieldValue))
	{}

	DHTrackParam(enum ETrackFieldName FieldName, const FString& FieldValue)
		: Value(FString::Printf(TEXT("%u=%s"), (uint32)FieldName, *FPlatformHttp::UrlEncode(FieldValue)))
	{}

	DHTrackParam(const FString& FieldName, const FString& FieldValue)
		: Value(FString::Printf(TEXT("%s=%s"), *FPlatformHttp::UrlEncode(FieldName), *FPlatformHttp::UrlEncode(FieldValue)))
	{}
};

class PWGAME_API DHEndpointTracker : public DHInstance<DHEndpointTracker, false, false, false>
{
	struct Impl;
	Impl* _Impl;
	void TrackEvent(enum EEndpointTrackEvent Event, const std::initializer_list<DHTrackParam>& Fields);
public:
	DHEndpointTracker();
	virtual ~DHEndpointTracker();

	void Init(const FString& Url, FString& AdverID);

	void SetUserID(const FString& UserID);

	FORCEINLINE void StartSession(bool IsNewUser, const FString& Device)
	{
		Track(EEndpointTrackEvent::TE_StartSession, { ETrackFieldName::IsNewUser, IsNewUser }, { ETrackFieldName::Device, Device });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event)
	{
		TrackEvent(Event, {});
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1)
	{
		TrackEvent(Event, { Field1 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2)
	{
		TrackEvent(Event, { Field1, Field2 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3)
	{
		TrackEvent(Event, { Field1, Field2, Field3 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3, const DHTrackParam& Field4)
	{
		TrackEvent(Event, { Field1, Field2, Field3, Field4 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3, const DHTrackParam& Field4, const DHTrackParam& Field5)
	{
		TrackEvent(Event, { Field1, Field2, Field3, Field4, Field5 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3, const DHTrackParam& Field4, const DHTrackParam& Field5, const DHTrackParam& Field6)
	{
		TrackEvent(Event, { Field1, Field2, Field3, Field4, Field5, Field6 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3, const DHTrackParam& Field4, const DHTrackParam& Field5, const DHTrackParam& Field6, const DHTrackParam& Field7)
	{
		TrackEvent(Event, { Field1, Field2, Field3, Field4, Field5, Field6, Field7 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3, const DHTrackParam& Field4, const DHTrackParam& Field5, const DHTrackParam& Field6, const DHTrackParam& Field7, const DHTrackParam& Field8)
	{
		TrackEvent(Event, { Field1, Field2, Field3, Field4, Field5, Field6, Field7, Field8 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3, const DHTrackParam& Field4, const DHTrackParam& Field5, const DHTrackParam& Field6, const DHTrackParam& Field7, const DHTrackParam& Field8, const DHTrackParam& Field9)
	{
		TrackEvent(Event, { Field1, Field2, Field3, Field4, Field5, Field6, Field7, Field8, Field9 });
	}

	FORCEINLINE void Track(enum EEndpointTrackEvent Event, const DHTrackParam& Field1, const DHTrackParam& Field2, const DHTrackParam& Field3, const DHTrackParam& Field4, const DHTrackParam& Field5, const DHTrackParam& Field6, const DHTrackParam& Field7, const DHTrackParam& Field8, const DHTrackParam& Field9, const DHTrackParam& Field10)
	{
		TrackEvent(Event, { Field1, Field2, Field3, Field4, Field5, Field6, Field7, Field8, Field9, Field10 });
	}
};
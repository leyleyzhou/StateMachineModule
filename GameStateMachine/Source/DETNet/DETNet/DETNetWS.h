#pragma once
#include "DETNetCmn.h"
#include "DETNetCode.h"
#include "DETNetStruct.h"
class IWebSocket;

class DETNET_EXPORT DETNetWS
{
public:
	DETNetWS();
	virtual ~DETNetWS();
protected:
	FString Url;
private:
	TSharedPtr<IWebSocket> Websocket;

	TArray<uint8> ReceiveBuffer;
	TArray<uint8> FrameBuffer;
	FTimespan PingInterval, PongInterval;
	FDateTime LastSent;
	FDateTime LastReceivedPacket;
	FDateTime LastReceivedFrame;

	/* Handlers for websocket events */
	virtual void OnConnected();
	virtual void OnConnectionError(const FString& Error);
	virtual void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	virtual void OnMessage(const FString& Data)final;
	virtual void OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)final;
	virtual void OnTick(float DeltaTime);

	void HandleIncomingFrame(const uint8* Data, int32 Length);
protected:
	void Clear();
public:
	int Connect(const FString& _Url);
	void Close();
	void Tick(float DeltaTime);
	template <typename InStructType>
	int Send(const InStructType& Data)
	{
		auto cmd = dynamic_cast<const FICmd*>(&Data);
		if (cmd == nullptr || !Websocket.IsValid())
		{
			return -1;
		}
		FString JsonString;
		DHJsonUtils::ToJson(&Data, JsonString);
		std::string str(TCHAR_TO_UTF8(*JsonString));
		TArray<uint8> Buf((const unsigned char*)str.c_str(), (int32)str.size());
		DETNetEncode encode(uint32(sizeof(cmd->GetID()) + str.size()));
		encode.Encode(cmd->GetID());
		encode.Encode(Buf);
		Websocket->Send(encode.GetBuffer().GetData(), encode.GetBuffer().Num(), true);
		LastSent = FDateTime::UtcNow();
#if IS_DEVELOPMENT_MODE
		//if this header file be included by outer cpp, will result a link error
		//UE_LOG(LogDETNet, Display, TEXT("Send CmdID=%d, Data=%s"), cmd->GetID(), *JsonString);
#endif
		return 0;
	}
};
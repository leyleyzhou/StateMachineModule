#include "DETNetWS.h"
#include "DETNetMsgEvent.h"
#include "DETNetCode.h"

DETNetWS::DETNetWS()
{
}
DETNetWS::~DETNetWS()
{
}

int DETNetWS::Connect(const FString& _Url)
{
	Url = _Url;
	if (!Url.Contains(TEXT("ws://")) && !Url.Contains(TEXT("wss://")))
	{
		Url = TEXT("ws://") + Url;
	}
	Url += TEXT("/ws");
	UE_LOG(LogTemp, Display, TEXT("Connect: %s"), *Url);
	Websocket = FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets").CreateWebSocket(Url, TEXT("ws"));

	Websocket->OnConnected().AddLambda([this]()
		{
			this->OnConnected();
		});
	Websocket->OnConnectionError().AddLambda([this](const FString& Error)
		{
			this->OnConnectionError(Error);
		});
	Websocket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean) {
		this->OnClosed(StatusCode, Reason, bWasClean);
		});
	Websocket->OnRawMessage().AddRaw(this, &DETNetWS::OnRawMessage);
	Websocket->OnMessage().AddRaw(this, &DETNetWS::OnMessage);

	Websocket->Connect();
	return 0;
}
void DETNetWS::Close()
{
	Websocket->Close();
}
void DETNetWS::Clear()
{
	Url.Empty();
	Websocket = nullptr;
	ReceiveBuffer.Empty();
	FrameBuffer.Empty();
}
void DETNetWS::OnTick(float DeltaTime)
{
}

void DETNetWS::Tick(float DeltaTime)
{
	OnTick(DeltaTime);
}

void DETNetWS::OnConnected()
{
}
void DETNetWS::OnConnectionError(const FString& Error)
{
}
void DETNetWS::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
}
void DETNetWS::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
	LastReceivedPacket = FDateTime::UtcNow();
	if (BytesRemaining == 0 && ReceiveBuffer.Num() == 0)
	{
		// Skip the temporary buffer when the entire frame arrives in a single message. (common case)
		HandleIncomingFrame((const uint8*)Data, Size);
	}
	else
	{
		ReceiveBuffer.Append((const uint8*)Data, Size);
		if (BytesRemaining == 0)
		{
			HandleIncomingFrame(ReceiveBuffer.GetData(), ReceiveBuffer.Num());
			ReceiveBuffer.Empty();
		}
	}
}
void DETNetWS::OnMessage(const FString& Data)
{//close关闭时会先发这条消息过来
	UE_LOG(LogTemp, Display, TEXT("OnMessage: %s"), *Data);
}

void DETNetWS::HandleIncomingFrame(const uint8* Data, int32 Length)
{
	LastReceivedFrame = FDateTime::UtcNow();
	DETNetDecode decoder(Data, Length);
	int ID;
	decoder.Decode(ID);
	FrameBuffer.SetNum(Length - sizeof(ID));
	decoder.Decode(FrameBuffer);
	FrameBuffer.Add('\0');
	DETNetMsgEvent::Emit(ID, FrameBuffer.GetData());

	//FNboSerializeFromBuffer buff(Data, Length);
	//int ID;
	//buff >> ID;
	//FrameBuffer.Empty();
	//FrameBuffer.Append(Data + buff.Tell(), buff.GetBufferSize() - buff.Tell());
	//FrameBuffer.Add('\0');
	//PWDETNetMsgEvent::Emit(ID, FrameBuffer.GetData());
}

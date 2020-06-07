#include "DETNetTcp.h"
#include "DETNetStruct.h"
#include "DETNetMsgEvent.h"
#include "DETNetCode.h"
//#include "DHCommon.h"

DETTcpStreamBuffer::DETTcpStreamBuffer()
	: MaxSize(0)
	, Size(0)
	, Begin(0)
	, ScrollSize(0)
	, BaseMaxSize(0)
	, MaxExtendCount(8)
	, CurrentExtend(0)
{

}
DETTcpStreamBuffer::~DETTcpStreamBuffer()
{
	Release();
}
uint8* DETTcpStreamBuffer::GetBuf() 					// 目前数据的开始指针，在下一个变更操作(Free)前有效
{
	return	Buffer.GetData() + Begin;
}
int	DETTcpStreamBuffer::GetMaxSize()
{
	return	MaxSize;
}
int	DETTcpStreamBuffer::GetSize() 					// 获得已经存在的数据长度
{
	return	Size;
}
uint8* DETTcpStreamBuffer::GetTail() 					// 获得尾指针
{
	return	Buffer.GetData() + Begin + Size;
}
int	DETTcpStreamBuffer::GetSizeLeft()				// 获得还可以添加的数据长度
{
	return	MaxSize - Begin - Size;
}
int		DETTcpStreamBuffer::Init(int nMaxSize, int nScrollSize)
{
	MaxSize = nMaxSize;
	ScrollSize = nScrollSize;
	BaseMaxSize = nMaxSize;
	Buffer.Reserve(nMaxSize);
	Clean();
	return	0;
}
int		DETTcpStreamBuffer::Release()
{
	MaxSize = 0;
	Size = 0;
	Begin = 0;
	ScrollSize = 0;
	return	0;
}
void	DETTcpStreamBuffer::Clean()
{
	Size = 0;
	Begin = 0;
}
bool	DETTcpStreamBuffer::Resize(int nSize)
{
	int nSizeLeft = GetSizeLeft();
	//无法扩展数据了
	int nMissedSize = nSize * 3 - nSizeLeft * 2;
	int nLeftExtendCount = MaxExtendCount - CurrentExtend;
	if (nMissedSize >= (nLeftExtendCount - 1) * BaseMaxSize)
		return false;

	//扩展内存
	int nExtendCount = (nMissedSize / BaseMaxSize + 1);

	//内存大小不够了
	if (CurrentExtend + nExtendCount >= MaxExtendCount)
	{
		return false;
	}

	int nNewMaxSize = (CurrentExtend + nExtendCount + 1) * BaseMaxSize;
	//char* pNewData = new char[nNewMaxSize];
	TArray<uint8> oldBuff = Buffer;
	Buffer.Reserve(nNewMaxSize);
	FMemory::Memcpy(Buffer.GetData(), oldBuff.GetData(), oldBuff.GetSlack());

	MaxSize = nNewMaxSize;
	ScrollSize = MaxSize / 4;

	CurrentExtend += nExtendCount;
	return true;
}
uint8* DETTcpStreamBuffer::InAlloc(int nSize)
{
	int nSizeLeft = GetSizeLeft();
	if (nSizeLeft * 2 < nSize * 3)
	{
		if (!Resize(nSize))
			return nullptr;
	}
	//assert(m_pBuf && (Size + nSize <= MaxSize));

	auto pBuf = GetBuf() + Size;
	Size += nSize;
	return	pBuf;
}
bool DETTcpStreamBuffer::In(const uint8* pData, int nSize)
{
	auto pBuf = InAlloc(nSize);
	if (!pBuf)
	{
		return	false;
	}
	FMemory::Memcpy(pBuf, pData, nSize);
	return		true;
}
int		DETTcpStreamBuffer::Out(uint8* pData, int* pnSize)
{
	if (Size == 0)
	{
		*pnSize = 0;
		return	-1;
	}

	// 最大不能超过已有的数据长度
	if ((*pnSize) >= Size)
	{
		(*pnSize) = Size;
	}
	memcpy(pData, GetBuf(), *pnSize);

	Out(*pnSize);

	return		0;
}
void	DETTcpStreamBuffer::Out(int nSize)
{
	if (nSize >= Size)
	{
		Clean();
		return;
	}
	Size -= nSize;
	Begin += nSize;
	if (Begin >= ScrollSize && ScrollSize >= nSize)
	{
		// 超过规定长度
		FMemory::Memmove(Buffer.GetData(), GetBuf(), Size);
		Begin = 0;
	}
}


DETNetTcp::DETNetTcp()
	:StreamBuffer(new DETTcpStreamBuffer())
{
	SocketSubsystem = nullptr;
	Socket = nullptr;
	ConnectState = Closed;
	MatchServerAddr = TEXT("");

}
DETNetTcp::~DETNetTcp()
{
}
void DETNetTcp::Init()
{
	UE_LOG(LogDETNet, Log, TEXT("DETNetTcp Init"));
	ReceiveBuffer.Reserve(1024 * 10);
	StreamBuffer->Init(1024 * 1024 * 10, 1024 * 1024);
}
int DETNetTcp::Connect(const FString& _Host)
{
	UE_LOG(LogDETNet, Log, TEXT("DETNetTcp Connect %s"), *_Host);
	if (_Host.Len() != 0)
	{
		MatchServerAddr = _Host;
	}
	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (SocketSubsystem == nullptr)
	{
		UE_LOG(LogDETNet, Error, TEXT("DETNetTcp unable to get socket subsystem"));
		return -1;
	}
	Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("BattleTcp"));
	if (Socket == nullptr)
	{
		UE_LOG(LogDETNet, Error, TEXT("DETNetTcp unable to allocate stream socket "));
		return -2;
	}
	FString _ip, _port;
	if (!MatchServerAddr.Split(":", &_ip, &_port))
	{
		UE_LOG(LogDETNet, Error, TEXT("DETNetTcp host split failed %s"), *MatchServerAddr);
		return -3;
	}
	Socket->SetNonBlocking(true);
	FIPv4Address ip;
	FIPv4Address::Parse(_ip, ip);
	TSharedRef<FInternetAddr> addr = SocketSubsystem->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(FCString::Atoi(*_port));
	if (!Socket->Connect(*addr))
	{
		UE_LOG(LogDETNet, Error, TEXT("DETNetTcp connect failed %s"), *MatchServerAddr);
		return -4;
	}
	ConnectState = Connecting;
	return 0;
}
void DETNetTcp::Close(bool Initiative)
{
	if (Socket)
	{
		Socket->Close();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
	OnClosed(Initiative);
}

void DETNetTcp::OnConnected()
{
}
void DETNetTcp::OnConnectionError(const FString& Error)
{
}
void DETNetTcp::OnClosed(bool Initiative)
{
}
void DETNetTcp::OnTick(float DeltaTime)
{
}

void DETNetTcp::Tick(float DeltaTime)
{
	OnTick(DeltaTime);
	if (Socket)
	{
		if (ConnectState == Connecting)
		{
			if (Socket->GetConnectionState() == SCS_Connected)
			{
				OnConnected();
				ConnectState = Connected;
				//GetCurrentWorld()->GetTimerManager().SetTimer(HeartbeatTimer, [this]() {

				//},5.0f,true);
			}
			else if (Socket->GetConnectionState() == SCS_ConnectionError)
			{
				OnConnectionError(Socket->GetDescription());
				ConnectState = ConnectFailed;
			}
		}
		while (Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::Zero()))
		{
			int32 rcvSize;
			if (Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.GetSlack(), rcvSize, ESocketReceiveFlags::None))
			{
				LastReceivedPacket = FDateTime::UtcNow();
				StreamBuffer->In(ReceiveBuffer.GetData(), rcvSize);
				CheckFrame();
			}
			else
			{//断开连接了（被动的）
				Close(false);
				break;
			}
		}
	}
}

int DETNetTcp::CheckFrame()
{
	while (StreamBuffer->GetSize() > HEADER_SIZE)
	{
		DETFrameHeader header;
		DETNetDecode decode(StreamBuffer->GetBuf(), StreamBuffer->GetSize());
		decode.Decode(header.Size);
		decode.Decode(header.ID);
		if (header.Size > StreamBuffer->GetSize() - HEADER_SIZE)	//包头到了，但是包体还没完全到达
		{
			break;
		}
		LastReceivedFrame = FDateTime::UtcNow();
		FrameBuffer.SetNum(header.Size);
		decode.Decode(FrameBuffer);
		FrameBuffer.Add('\0');
		DETNetMsgEvent::Emit((int)header.ID, FrameBuffer.GetData());
		StreamBuffer->Out(int(HEADER_SIZE + header.Size));
	}

	return 0;
}

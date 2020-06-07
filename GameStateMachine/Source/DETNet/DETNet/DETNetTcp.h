#pragma once
#include "DETNetCmn.h"
#include "DETNetCode.h"
#include "DETNetStruct.h"
class FSocket;
class ISocketSubsystem;

class DETTcpStreamBuffer
{
protected:
	TArray<uint8> Buffer;					// 用于申请数据缓冲区
	int	MaxSize;							// 可以存放的最大数据长度
	int	Size;								// 已经存放的数据总长度
	int	Begin;								// 数据开始的位置
	int	ScrollSize;							// 当取出数据导致Begin超过ScrollSize，就将所有数据滚到缓冲头部
	int	BaseMaxSize;
	int MaxExtendCount;						// 最大可以扩大次数
	int	CurrentExtend;						// 当前扩大次数
public:
	uint8* GetBuf(); 						// 目前数据的开始指针，在下一个变更操作(Free)前有效
	int	GetMaxSize();
	int	GetSize(); 							// 获得已经存在的数据长度
	uint8* GetTail(); 						// 获得尾指针
	int	GetSizeLeft();						// 获得还可以添加的数据长度
public:
	DETTcpStreamBuffer();
	~DETTcpStreamBuffer();
	int		Init(int nMaxSize, int nScrollSize);
	int		Release();								// 终结(会在析构中自动调用)
	void	Clean();								// 清除缓冲中的所有数据
	uint8* InAlloc(int nSize);						// 申请加入缓冲内存，自行填写内容，避免多申请拷贝一次
	bool	In(const uint8* pData, int nSize);		// 加入长度为nSize的数据
	int		Out(uint8* pData, int* pnSize);			// 取出数据，*pnSize里面存放取得的数据长度，返回0表示成功获得数据，-1表示没有数据
	void	Out(int nSize);							// 释放数据
	bool	Resize(int nSize);						//重新分配内存出来
};

class DETNET_EXPORT DETNetTcp
{
	enum
	{
		Closed,
		Connecting,
		Connected,
		ConnectFailed,
	};
	struct DETFrameHeader
	{
		uint32 Size;				//包大小
		uint32 ID;					//协议号
	};
#define HEADER_SIZE sizeof(DETFrameHeader)
public:
	DETNetTcp();
	virtual ~DETNetTcp();
private:
	ISocketSubsystem* SocketSubsystem;
	FSocket* Socket;

	TArray<uint8> ReceiveBuffer;
	TArray<uint8> FrameBuffer;
	DETTcpStreamBuffer* StreamBuffer;

	int ConnectState;

	FTimespan PingInterval, PongInterval;
	FDateTime LastSent;
	FDateTime LastReceivedPacket;
	FDateTime LastReceivedFrame;
	FTimerHandle HeartbeatTimer;

	/* Handlers for websocket events */
	virtual void OnConnected();
	virtual void OnConnectionError(const FString& Error);
	virtual void OnClosed(bool Initiative);
	virtual void OnTick(float DeltaTime);

	int CheckFrame();
	void Clear();

protected:
	FString MatchServerAddr;
public:
	void Init();
	int Connect(const FString& _Host = TEXT(""));
	void Close(bool Initiative = true);
	void Tick(float DeltaTime);

	template <typename InStructType>
	int Send(const InStructType& Data)
	{
		if (Socket == nullptr)
		{
			return -1;
		}
		auto cmd = dynamic_cast<const FICmd*>(&Data);
		if (cmd == nullptr)
		{
			return -2;
		}
		FString JsonString;
		DHJsonUtils::ToJson(&Data, JsonString);
		std::string str(TCHAR_TO_UTF8(*JsonString));
		DETFrameHeader header;
		header.Size = (uint32)str.size();
		header.ID = (uint32)cmd->GetID();
		TArray<uint8> Buf((const unsigned char*)str.c_str(), (int32)str.size());
		DETNetEncode encode(uint32(HEADER_SIZE + header.Size));
		encode.Encode(header.Size);
		encode.Encode(header.ID);
		encode.Encode(Buf);
		int32 sendBytes;
		Socket->Send(encode.GetBuffer().GetData(), encode.GetBufferSize(), sendBytes);
		LastSent = FDateTime::UtcNow();
		return 0;
	}

};
#pragma once
#include "DETNetCmn.h"

//序列化和反序列化（网络字节序大端）

class DETNET_EXPORT DETNetEncode
{
public:
	DETNetEncode(uint32 Size);
private:
	FNboSerializeToBuffer Buffer;
public:
	template<typename _Ty>
	void Encode(const _Ty& Val);
	void Encode(const bool& Val);
	void Encode(const int16& Val);
	void Encode(const uint16& Val);
	void Encode(const TArray<uint8>& Val);
	const TArray<uint8>& GetBuffer()const;
	int32 GetBufferSize()const;
};

template<typename _Ty>
void DETNetEncode::Encode(const _Ty& Val) {
	Buffer << Val;
}

class DETNET_EXPORT DETNetDecode
{
public:
	DETNetDecode(const uint8* InData, int32 Length);
	DETNetDecode(const TArray<uint8>& InData);
private:
	FNboSerializeFromBuffer Buffer;
public:
	template<typename _Ty>
	void Decode(_Ty& Val);
	void Decode(bool& Val);
	void Decode(int16& Val);
	void Decode(uint16& Val);
	void Decode(TArray<uint8>& Val);
	int AvailableToRead(void) const;
};

template<typename _Ty>
void DETNetDecode::Decode(_Ty& Val) {
	Buffer >> Val;
}
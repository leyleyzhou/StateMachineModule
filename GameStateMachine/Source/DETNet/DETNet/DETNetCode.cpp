#include "DETNetCode.h"

DETNetEncode::DETNetEncode(uint32 Size)
	:Buffer(Size)
{
}
const TArray<uint8>& DETNetEncode::GetBuffer()const
{
	return Buffer.GetBuffer();
}
int32 DETNetEncode::GetBufferSize()const
{
	return Buffer.GetBufferSize();
}

void DETNetEncode::Encode(const bool& Val)
{
	Buffer << (uint8)Val;
}
void DETNetEncode::Encode(const int16& Val)
{
	Encode(*(uint16*)&Val);
}
void DETNetEncode::Encode(const uint16& Val)
{
	uint8 OutBuf[2];
	OutBuf[0] = Val >> 8 & 0xFF;
	OutBuf[1] = Val & 0xFF;
	Buffer.WriteBinary(OutBuf, (uint32)sizeof(OutBuf));
}
void DETNetEncode::Encode(const TArray<uint8>& Val)
{
	Buffer.WriteBinary(Val.GetData(), (uint32)Val.Num());
}

DETNetDecode::DETNetDecode(const uint8* InData, int32 Length)
	:Buffer(InData, Length)
{
}
DETNetDecode::DETNetDecode(const TArray<uint8>& InData)
	: Buffer(InData.GetData(), InData.Num())
{
}
int DETNetDecode::AvailableToRead(void) const
{
	return Buffer.AvailableToRead();
}

void DETNetDecode::Decode(bool& Val)
{
	uint8 OutVal;
	Buffer >> OutVal;
	Val = !!OutVal;
}
void DETNetDecode::Decode(int16& Val)
{
	Decode(*(uint16*)&Val);
}
void DETNetDecode::Decode(uint16& Val)
{
	uint8 OutBuf[2];
	Buffer.ReadBinary(OutBuf, (uint32)sizeof(OutBuf));
	Val = (uint16)OutBuf[0] << 8 | (uint16)OutBuf[1];
}
void DETNetDecode::Decode(TArray<uint8>& Val)
{
	Buffer.ReadBinary(Val.GetData(), (uint32)Val.Num());
}


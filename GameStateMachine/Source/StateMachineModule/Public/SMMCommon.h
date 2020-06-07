#pragma once
#include "CoreMinimal.h"

#pragma region ExtractParmaType

template <typename T>
class RemoveReferenceTypeUtil
{
public:
	typedef T Type;
};

template <typename T>
class RemoveReferenceTypeUtil<const T&>
{
public:
	typedef T Type;
};

template<typename T>
class RemoveReferenceTypeUtil<T&>
{
public:
	typedef T Type;
};

#pragma endregion

#pragma  region CommandFunctionDefine

struct CommandInvokeCache
{
public:
	FString FuncSignature;
	TArray<uint8> ParamData;
};

class CommandConversion
{
public:
	FName SourceCommand;
	FName TargetCommand;
	//延迟执行如果被打开的话则SourceCommand被触发的时候不直接触发TargetCommand，缓存起来在停止Conversion的时候再执行
	bool bDelayExecute;
	//缓存每次调用，不剔除重复调用，这样在转换结束的时候可能会一次性发起多次TargetCommand调用
	bool bCacheEveryExecute;

	TArray<CommandInvokeCache> InvokeCaches;
};

class CommandFunctionBase
{
public:
	CommandFunctionBase() : bBlueprintFunc(false) {}
	virtual ~CommandFunctionBase() {}

	virtual void DoCommand(const TArray<uint8>& ParamData) {}
	bool bBlueprintFunc = false;
};

template<typename Cls>
class CommandBlueprintFunction : public CommandFunctionBase
{
public:
	Cls* Obj;
	FName FuncName;

	CommandBlueprintFunction()
	{
		bBlueprintFunc = true;
	}

	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		UFunction* pFindFunc = Obj->FindFunction(FuncName);
		if (pFindFunc)
		{
			if (pFindFunc->ParmsSize == ParamData.Num())
			{
				Obj->ProcessEvent(pFindFunc, (void*)ParamData.GetData());
			}
			else
			{
				check(false);
			}
		}
	}
};


template<typename Cls>
class CommandFuncParam0 : public CommandFunctionBase
{
public:
	typedef void(Cls::* FunctionType) (void);
	Cls* Obj;
	FunctionType Functor;

	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		(Obj->*Functor)();
	}
};

template<typename Cls, typename T1>
class CommandFuncParam1 : public CommandFunctionBase
{
public:
	typedef void(Cls::* FunctionType) (T1 Param1);
	Cls* Obj;
	FunctionType Functor;
	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		int32 DataOffset = 0;
		typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
		const T1NoRefType& Param1 = *((const T1NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		check(DataOffset == ParamData.Num());
		if (DataOffset == ParamData.Num())
		{
			(Obj->*Functor)(Param1);
		}
	}
};

template<typename Cls, typename T1, typename T2>
class CommandFuncParam2 :public CommandFunctionBase
{
public:
	typedef void(Cls::* FunctionType) (T1 Param1, T2 Param2);
	Cls* Obj;
	FunctionType Functor;
	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		int32 DataOffset = 0;
		typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
		const T1NoRefType& Parma1 = *((const T1NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		typedef typename RemoveReferenceTypeUtil<T2>::Type T2NoRefType;
		const T2NoRefType& Parma1 = *((const T2NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		check(DataOffset == ParamData.Num());
		if (DataOffset == ParamData.Num())
		{
			(Obj->*Functor)(Param1, Param2);
		}
	}

};

template<typename Cls, typename T1, typename T2, typename T3>
class CommandFuncParam3 :public CommandFunctionBase
{
public:
	typedef void(Cls::* FunctionType) (T1 Param1, T2 Param2, T3 Param3);
	Cls* Obj;
	FunctionType Functor;
	virtual void DoCommand(const TArray<uint8>& ParamData)
	{
		int32 DataOffset = 0;
		typedef typename RemoveReferenceTypeUtil<T1>::Type T1NoRefType;
		const T1NoRefType& Parma1 = *((const T1NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		typedef typename RemoveReferenceTypeUtil<T2>::Type T2NoRefType;
		const T2NoRefType& Parma1 = *((const T2NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T1NoRefType);

		typedef typename RemoveReferenceTypeUtil<T3>::Type T3NoRefType;
		const T3NoRefType& Parma1 = *((const T3NoRefType*)&ParamData[DataOffset]);
		DataOffset += sizeof(T3NoRefType);

		check(DataOffset == ParamData.Num());
		if (DataOffset == ParamData.Num())
		{
			(Obj->*Functor)(Param1, Param2, Param3);
		}
	}

};
#pragma endregion

#pragma region ParamSignature

template<typename T>
class ParamSignatureString
{
public:

};

template<typename T>
class ParamSignatureString<T*>
{
public:
	static FString GetParamSig()
	{
		return FString(TEXT("Ptr")) + ParamSignatureString<T>::GetParamSig();
	}
};

template<typename T>
class ParamSignatureString<T&>
{
public:
	static FString GetParamSig()
	{
		return ParamSignatureString<T>::GetParamSig();
	}
};

template<typename T>
class ParamSignatureString<const T>
{
public:
	static FString GetParamSig()
	{
		return FString(TEXT("Cst")) + ParamSignatureString<T>::GetParamSig();
	}
};

template<> class ParamSignatureString<int64> { public: static FString GetParamSig() { return TNameOf<int64>::GetName(); } };
template<> class ParamSignatureString<int32> { public: static FString GetParamSig() { return TNameOf<int32>::GetName(); } };
template<> class ParamSignatureString<int16> { public: static FString GetParamSig() { return TNameOf<int16>::GetName(); } };
template<> class ParamSignatureString<int8> { public: static FString GetParamSig() { return TNameOf<int8>::GetName(); } };
template<> class ParamSignatureString<uint64> { public: static FString GetParamSig() { return TNameOf<uint64>::GetName(); } };
template<> class ParamSignatureString<uint32> { public: static FString GetParamSig() { return TNameOf<uint32>::GetName(); } };
template<> class ParamSignatureString<uint16> { public: static FString GetParamSig() { return TNameOf<uint16>::GetName(); } };
template<> class ParamSignatureString<uint8> { public: static FString GetParamSig() { return TNameOf<uint8>::GetName(); } };
template<> class ParamSignatureString<float> { public: static FString GetParamSig() { return TNameOf<float>::GetName(); } };
template<> class ParamSignatureString<double> { public: static FString GetParamSig() { return TNameOf<double>::GetName(); } };
template<> class ParamSignatureString<FVector> { public: static FString GetParamSig() { return TEXT("FVector"); } };
template<> class ParamSignatureString<FRotator> { public: static FString GetParamSig() { return TEXT("FRotator"); } };
template<> class ParamSignatureString<bool> { public: static FString GetParamSig() { return TEXT("bool"); } };

#pragma endregion

#define return_if_true(o)				\
	do									\
	{									\
		if((o))							\
		{								\
			return;						\
		}								\
	} while (false);					\

#define return_if_false(o)				\
	do									\
	{									\
		if(!(o))						\
		{								\
			return;						\
		}								\
	} while (false);					\

#define return_if_nullptr(o)			\
	do									\
	{									\
		if(!(o))						\
		{								\
			return;						\
		}								\
	} while (false);					\

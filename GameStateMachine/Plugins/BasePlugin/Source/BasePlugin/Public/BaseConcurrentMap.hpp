#pragma once

#include "BaseCommon.h"

namespace DHBase
{
	struct FDefaultDeleter
	{
		template <
			typename T
		>
			static void Delete(T& Value) {}
	};

	template <
		typename KeyType,
		typename ValueType,
		typename Deleter = FDefaultDeleter,
		int32 BucketSize = 10,
		typename SetAllocator = FDefaultSetAllocator,
		typename KeyFuncs = TDefaultMapHashableKeyFuncs<KeyType, ValueType, false>
	>
		class ConcurrentHashMap
	{
		struct Bucket
		{
			TMap<KeyType, ValueType, SetAllocator, KeyFuncs> RawMap;
			FRWLock Lock;

			~Bucket()
			{
				for (auto& Iter : RawMap) {
					Deleter::template Delete(Iter.Value);
				}
			}
		};

		Bucket _Buckets[BucketSize];

		FORCEINLINE int32 SpreadHash(int32 Hash)
		{
			return (Hash ^ (((uint32)Hash) >> 16)) & 0x7fffffff;
		}

		FORCEINLINE Bucket* GetBucket(const KeyType& Key)
		{
			int32 Hash = SpreadHash(KeyFuncs::GetKeyHash(Key)) % BucketSize;
			return &_Buckets[Hash];
		}
	public:
		using HashMap = ConcurrentHashMap<KeyType, ValueType, Deleter, BucketSize, SetAllocator, KeyFuncs>;
		class ConstAccessor;
		class Accessor;

		ConcurrentHashMap() = default;

		~ConcurrentHashMap() = default;

		bool Find(const KeyType& Key, ValueType& OutValue)
		{
			auto Bucket = GetBucket(Key);

			FRWScopeLock Locker(Bucket->Lock, FRWScopeLockType::SLT_ReadOnly);
			auto Value = Bucket->RawMap.Find(Key);
			return_false_if_null(Value);
			OutValue = *Value;
			return true;
		}

		void Add(const KeyType& Key, ValueType OutValue)
		{
			auto Bucket = GetBucket(Key);

			FRWScopeLock Locker(Bucket->Lock, FRWScopeLockType::SLT_Write);
			Bucket->RawMap.Add(Key, OutValue);
		}

		void Emplace(const KeyType& Key, ValueType&& OutValue)
		{
			auto Bucket = GetBucket(Key);

			FRWScopeLock Locker(Bucket->Lock, FRWScopeLockType::SLT_Write);
			Bucket->RawMap.Add(Key, MoveTemp(OutValue));
		}

		bool Find(ConstAccessor& Accessor, const KeyType& Key)
		{
			auto Bucket = GetBucket(Key);

			Bucket->Lock.ReadLock();
			Accessor._LockType = FRWScopeLockType::SLT_ReadOnly;
			Accessor._Lock = &Bucket->Lock;

			auto Value = Bucket->RawMap.Find(Key);
			return_false_if_null(Value);

			Accessor._Value = Value;

			return true;
		}

		bool Find(Accessor& Accessor, const KeyType& Key)
		{
			auto Bucket = GetBucket(Key);

			Bucket->Lock.WriteLock();
			Accessor._LockType = FRWScopeLockType::SLT_Write;
			Accessor._Lock = &Bucket->Lock;
			Accessor._Key = const_cast<KeyType*>(&Key);
			Accessor._Bucket = Bucket;

			auto Value = Bucket->RawMap.Find(Key);
			return_false_if_null(Value);

			Accessor._Value = Value;

			return true;
		}

		bool ContainsKey(const KeyType& Key)
		{
			auto Bucket = GetBucket(Key);

			FRWScopeLock Locker(Bucket->Lock, FRWScopeLockType::SLT_ReadOnly);
			return Bucket->RawMap.Contains(Key);
		}

		bool IsEmpty()
		{
			FPlatformMisc::MemoryBarrier();

			for (int32 i = 0; i < BucketSize; ++i) {
				return_false_if_true(_Buckets[i].RawMap.Num() > 0);
			}

			return true;
		}

		void ForEach(TFunction<void(const KeyType&, ValueType&)> Function)
		{
			for (int32 i = 0; i < BucketSize; ++i) {
				FRWScopeLock Locker(_Buckets[i].Lock, FRWScopeLockType::SLT_Write);
				for (auto& Iter : _Buckets[i].RawMap) {
					Function(Iter.Key, Iter.Value);
				}
			}
		}

		bool Remove(const KeyType& Key)
		{
			auto Bucket = GetBucket(Key);

			FRWScopeLock Locker(Bucket->Lock, FRWScopeLockType::SLT_Write);

			ValueType Value;
			if (Bucket->RawMap.RemoveAndCopyValue(Key, Value)) {
				Deleter::Delete(Value);
				return true;
			}

			return false;
		}

		bool RemoveAndCopyValue(const KeyType& Key, ValueType& Value)
		{
			auto Bucket = GetBucket(Key);

			FRWScopeLock Locker(Bucket->Lock, FRWScopeLockType::SLT_Write);
			return Bucket->RawMap.RemoveAndCopyValue(Key, Value);
		}

		void Empty()
		{
			for (int32 i = 0; i < BucketSize; ++i) {
				FRWScopeLock Locker(_Buckets[i].Lock, FRWScopeLockType::SLT_Write);
				for (auto& Iter : _Buckets[i].RawMap) {
					Deleter::Delete(Iter.Value);
				}
				_Buckets[i].RawMap.Empty();
			}
		}
	public:
		class ConstAccessor
		{
			friend class ConcurrentHashMap<KeyType, ValueType, Deleter, BucketSize, SetAllocator, KeyFuncs>;
		protected:
			FRWLock* _Lock = nullptr;
			FRWScopeLockType _LockType = FRWScopeLockType::SLT_ReadOnly;
			ValueType* _Value = nullptr;
		public:
			ConstAccessor() = default;

			virtual ~ConstAccessor()
			{
				Release();
			}

			void Release()
			{
				return_if_null(_Lock);

				switch (_LockType) {
				case SLT_ReadOnly:
					_Lock->ReadUnlock();
					break;
				case SLT_Write:
					_Lock->WriteUnlock();
					break;
				default:
					break;
				}
				_Lock = nullptr;
			}

			virtual ValueType* GetValue()
			{
				return _Value;
			}

			virtual ValueType& GetValueRef()
			{
				return *_Value;
			}
		};

		class Accessor : public ConstAccessor
		{
			typedef ConstAccessor Super;
			Bucket* _Bucket = nullptr;
			KeyType* _Key = nullptr;
			friend class ConcurrentHashMap<KeyType, ValueType, Deleter, BucketSize, SetAllocator, KeyFuncs>;
		public:
			Accessor() = default;

			virtual ~Accessor() = default;

			void SetValue(ValueType Value)
			{
				if (Super::_Value) {
					Deleter::Delete(*Super::_Value);
				}

				Super::_Value = &_Bucket->RawMap.Add(*_Key, Value);
			}

			void Remove()
			{
				ValueType Value;
				if (_Bucket->RawMap.RemoveAndCopyValue(*_Key, Value)) {
					Deleter::Delete(Value);
				}
				Super::_Value = nullptr;
			}
		};
	};
}
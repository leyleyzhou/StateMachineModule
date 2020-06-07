#pragma once

#include "CoreMinimal.h"
#include "GCObject.h"
#include "BaseEditorHelper.hpp"
#include "Engine/World.h"
#include "StringTableRegistry.h"

#define IS_DEVELOPMENT_MODE (!(UE_BUILD_SHIPPING || UE_BUILD_TEST) || WITH_EDITOR)

#define DH_THREAD_SAFE
#define DH_LOCTABLE(o)        LOCTABLE("Lang", o)
#define DH_LOCTABLE_STR(o)    LOCTABLE("Lang", o).ToString()

#define return_if_true(o)           \
    do {                            \
        if ((o)) {                  \
			return;			        \
        }                           \
    } while (false)

#define return_if_null(o)           \
    do {                            \
        if ((o) == nullptr) {       \
			return;			        \
        }                           \
    } while (false)

#define return_true_if_true(o)      \
    do {                            \
		if ((o)) {                  \
            return true;            \
        }                           \
    } while (false)

#define return_false_if_true(o)     \
    do {                            \
		if ((o)) {                  \
            return false;           \
        }                           \
    } while (false)

#define return_false_if_null(o)		\
	do {							\
		if ((o) == nullptr) {		\
			return false;			\
		}							\
	} while(false)

#define return_false_if_false(o)    \
    do {                            \
		if (!(o)) {                 \
            return false;           \
        }                           \
    } while (false)


#define return_null_if_true(o)      \
    do {                            \
		if ((o)) {                  \
            return nullptr;         \
        }                           \
    } while (false)

#define return_null_if_null(o)      \
    do {                            \
		if ((o) == nullptr) {       \
            return nullptr;         \
        }                           \
    } while (false)

#ifndef safe_delete
#   define safe_delete(o)           \
    do {                            \
        if ((o) != nullptr) {       \
            delete (o);             \
            (o) = nullptr;          \
        }                           \
    } while (false)
#endif

#define break_if_true(o)           {\
    if ((o)) break;                 \
}

#define continue_if_true(o)        {\
    if ((o)) continue;              \
}

#define ten_to_float(v) (v/10.f)
#define pect_to_float(v) (v/100.f)
#define float_to_ten(v) ((int32)(v * 10))
#define float_to_pect(v) ((int32)(v * 100))
#define round_to_int(v) (FMath::RoundToInt(v))
#define is_nearly_zero(v) (FMath::IsNearlyZero(v))

#define TEST_BIT(Bitmask, Bit) (((Bitmask) & (1 << static_cast<uint32>(Bit))) > 0)
#define SET_BIT(Bitmask, Bit) (Bitmask |= 1 << static_cast<uint32>(Bit))
#define CLEAR_BIT(Bitmask, Bit) (Bitmask &= ~(1 << static_cast<uint32>(Bit)))

#define DH_DEBUG(FORMAT, ...)       \
    UE_LOG(LogTemp, Display, TEXT("[DEBUG]" FORMAT), ##__VA_ARGS__)

#if IS_DEVELOPMENT_MODE
#   define DH_ASSERT_TRUE(Value, ErrorMsg) do {         \
        if (!(Value)) {                                 \
            UE_LOG(LogTemp, Error, TEXT(ErrorMsg));     \
        }                                               \
    } while (false);
#else 
#   define DH_ASSERT_TRUE(Value, ErrorMsg)
#endif

template <bool>
struct BASEPLUGIN_API TemplateSwitch
{};
typedef TFunction<void()> FSimpleFunction;
template<typename T> struct DHRemovePointer { typedef T type; };
template<typename T> struct DHRemovePointer<T*> { typedef T type; };
template<typename T> struct DHRemovePointer<T* const> { typedef T type; };

template <typename A, typename B>
struct DHTuple2
{
    A One{};
    B Two{};
};

template <typename A, typename B, typename C>
struct DHTuple3
{
    A One{};
    B Two{};
    C Three{};
};

template <typename A, typename B, typename C, typename D>
struct DHTuple4
{
    A One{};
    B Two{};
    C Three{};
    D Four{};
};

#define DH_ID_NONE 0

typedef int32 DHUniqueID;

BASEPLUGIN_API DHUniqueID  GenerateUniqueID() DH_THREAD_SAFE;
BASEPLUGIN_API DHUniqueID CreateInstance(FSimpleFunction DeleteFunction);
BASEPLUGIN_API bool RemoveInstanceByID(const DHUniqueID InstanceID);
BASEPLUGIN_API void RemoveInstances();
BASEPLUGIN_API void OnWorldDestroy_DispatchOnce(UWorld*, TFunction<void(UWorld*)> Function);
BASEPLUGIN_API void OnGameEnd_DispatchOnce(FSimpleFunction Function);
BASEPLUGIN_API void MoveInstanceToWorld(const DHUniqueID InstanceID);
BASEPLUGIN_API void MoveInstanceToRoot(const DHUniqueID InstanceID);
BASEPLUGIN_API FDelegateHandle ExecuteNextFrame(FSimpleDelegate Delegate);

FORCEINLINE static FDelegateHandle ExecuteNextFrame(FSimpleFunction Function)
{
    return ExecuteNextFrame(FSimpleDelegate::CreateLambda(Function));
}

// 跨作用域 UObject 安全封箱
class BASEPLUGIN_API StrongRefUObject : public FGCObject
{
    UObject* _Object = nullptr;
public:
    StrongRefUObject(UObject* object) :
        _Object(object) {
    }

    virtual ~StrongRefUObject() = default;

    void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        Collector.AddReferencedObject(_Object);
    }

    UObject* Object()
    {
        return _Object;
    }
};

template <
    typename T,
    bool IsUObject = std::is_pointer<T>::value&& std::is_base_of<UObject, typename DHRemovePointer<T>::type>::value,
    bool IsRefObject = std::is_pointer<T>::value&& std::is_base_of<FRefCountedObject, typename DHRemovePointer<T>::type>::value,
    bool IsActor = std::is_pointer<T>::value&& std::is_base_of<AActor, typename DHRemovePointer<T>::type>::value
>
struct DHVariable
{
private:
    StrongRefUObject* _RefUObject = nullptr;

    ULevel* _Level = nullptr;

    typedef typename std::remove_reference<T>::type Type;

    Type _Value{};

    void InitializeActor(TemplateSwitch<true>&)
    {
        _Level = ((AActor*)_RefUObject->Object())->GetLevel();
    }

    void InitializeActor(TemplateSwitch<false>&)
    {
    }

    void Initialize(TemplateSwitch<true>&, T Value)
    {
        _RefUObject = new StrongRefUObject(Value);

        TemplateSwitch<IsActor> SW;
        InitializeActor(SW);
    }

    void Initialize(TemplateSwitch<false>&, T Value)
    {
        _Value = Value;
    }

    T Value(TemplateSwitch<true>&)
    {
        return (T)_RefUObject->Object();
    }

    T Value(TemplateSwitch<false>&)
    {
        return _Value;
    }

    void SafeFree(TemplateSwitch<true>&, TemplateSwitch<false>&)
    {
        safe_delete(_Value);
    }

    void SafeFree(TemplateSwitch<true>&, TemplateSwitch<true>&)
    {
        safe_delete(_Value);
    }

    void SafeFree(TemplateSwitch<false>&, TemplateSwitch<true>&)
    {
        _Value->Release();
    }

    void SafeFree(TemplateSwitch<false>&, TemplateSwitch<false>&)
    {
    }
public:
    void Initialize(T Value)
    {
        TemplateSwitch<IsUObject> SW;
        Initialize(SW, Value);
    }

    T Value()
    {
        TemplateSwitch<IsUObject> SW;
        return Value(SW);
    }

    bool IsInLevel(const TSet<ULevel*>& Levels)
    {
        if (!_Level) {
            return true;
        }

        return Levels.Contains(_Level);
    }

    ~DHVariable()
    {
        TemplateSwitch<IsUObject> SwUObject;
        TemplateSwitch<IsRefObject> SwRefObject;

        SafeFree(SwUObject, SwRefObject);
    }
};

class BASEPLUGIN_API IDHTickable
{
public:
    virtual ~IDHTickable() = default;

    virtual void Tick(float DeltaTime) = 0;
};

template <typename T>
FORCEINLINE const FString& GetNameOfType();

class BASEPLUGIN_API DHInstance_Internal
{
protected:
    DHUniqueID _InstID = DH_ID_NONE;

    void Release()
    {
        return_if_true(_InstID != DH_ID_NONE);

        RemoveInstanceByID(_InstID);

        _InstID = DH_ID_NONE;
    }
public:
    void AddToWorld()
    {
        return_if_true(_InstID == DH_ID_NONE);

        MoveInstanceToWorld(_InstID);
    }

    void AddToRoot()
    {
        return_if_true(_InstID == DH_ID_NONE);

        MoveInstanceToRoot(_InstID);
    }
};

/*
 * 单例
 * @param Type 子类
 * @param WithTagID 子类是否要支持Multiple操作
 */
template <class Type, bool WithTagID = false, bool IsActor = false, bool InWorld = true>
class DHInstance;

template <class Type>
class DHInstance<Type, false, false, true> : public DHInstance_Internal
{
    static Type* GetInstance(bool AutoCreate)
    {
        static TSharedPtr<Type> Instance;

        if (!Instance && AutoCreate) {
            Instance = MakeShareable(new Type);
            decltype(Instance)* Ptr = &Instance;
            Instance->_InstID = CreateInstance([Ptr] {
                *Ptr = nullptr;
                });
        }

        return Instance.Get();
    }
public:
    static Type* GetInstance()
    {
        return GetInstance(true);
    }

    static void ReleaseInstance()
    {
        Type* Inst = GetInstance(false);

        if (Inst) {
            Inst->Release();
        }
    }
};

template <class Type>
class DHInstance<Type, true, false, true> : public DHInstance_Internal
{
private:

    static Type* GetInstance(int32 TagID, bool AutoCreate)
    {
        static TMap<int32, TSharedPtr<Type>> Instance;

        TSharedPtr<Type> Value = Instance.FindRef(TagID);
        if (!Value && AutoCreate) {
            Value = MakeShareable(new Type(TagID));
            decltype(Instance)* Ptr = &Instance;
            Value->_InstID = CreateInstance([Ptr, TagID] {
                Instance->Remove(TagID);
                });
            Instance.Add(TagID, Value);
        }

        return Value.Get();
    }
public:
    static Type* GetInstance(int32 TagID)
    {
        return GetInstance(TagID, true);
    }

    static void ReleaseInstance(int32 TagID)
    {
        Type* Inst = GetInstance(TagID, false);

        if (Inst) {
            Inst->Release();
        }
    }
};

template <class Type>
class DHInstance<Type, false, true, true>
{
public:
    static Type* GetInstance(UObject* Context)
    {
        static TMap<UWorld*, Type*> Instances;

        UWorld* World = Context->GetWorld();
        check(World);
        auto Value = Instances.FindRef(World);
        if (!Value) {
            Value = World->SpawnActor<Type>();
            Instances.Add(World, Value);
            auto Ptr = &Instances;
            OnWorldDestroy_DispatchOnce(World, [Ptr](UWorld* World) {
                Ptr->Remove(World);
                });
        }

        return Value;
    }
};

template <class Type>
class DHInstance<Type, false, false, false>
{
public:
    static Type* GetInstance()
    {
        static Type Instance;

        return &Instance;
    }
};

template <
    typename T
>
T* DHNewObject(TemplateSwitch<true>&, UObject* Owner)
{
    auto Value = NewObject<T>(Owner);
    return Value;
}

template <
    typename T
>
T* DHNewObject(TemplateSwitch<false>&, UObject* Owner)
{
    return new T;
}

template <
    typename T
>
T* DHNewObject(UObject* Owner)
{
    TemplateSwitch<std::is_base_of<UObject, T>::value> SW;
    return DHNewObject<T>(SW, Owner);
}
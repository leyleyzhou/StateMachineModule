#pragma once
#include "BaseCommon.h"
//#include "DHScopedHelper.h"
#include "EngineUtils.h"
#include "BaseConcurrentMap.hpp"
#include "Ticker.h"
#if WITH_EDITOR
#   include "Editor.h"
#endif
#include "BaseFile.h"

struct Instance
{
	TFunction<void()> DeleteFunction;

	~Instance()
	{
		if (DeleteFunction) {
			DeleteFunction();
		}
	}
};

struct InstanceDeleter
{
	template <typename T>
	static void Delete(T* Inst)
	{
		safe_delete(Inst);
	}
};

static DHBase::ConcurrentHashMap<int32, Instance*, InstanceDeleter> InstanceGol;
static DHBase::ConcurrentHashMap<UWorld*, TDoubleLinkedList<TFunction<void(UWorld*)>>*> Worlds;
static TQueue<TFunction<void()>> GameQueue;

static void Game_OnEnded(bool);
static void World_OnFinishDestroy(UWorld*);

struct SetupWorldEvents
{
	SetupWorldEvents()
	{
#if WITH_EDITOR
		FEditorDelegates::EndPIE.AddStatic(Game_OnEnded);
#endif
		FWorldDelegates::OnPreWorldFinishDestroy.AddStatic(World_OnFinishDestroy);
	}
};

static SetupWorldEvents Setup{};


static void Game_OnEnded(bool)
{
	InstanceGol.Empty();

	TFunction<void()> Function;
	while (GameQueue.Peek(Function)) {
		Function();
		GameQueue.Pop();
	}

	BaseFiles::ResetGlobalCfgVersion();
}

static void World_OnFinishDestroy(UWorld* World)
{
	TDoubleLinkedList<TFunction<void(UWorld*)>>* List = nullptr;

	{
		decltype(Worlds)::Accessor Ac;
		if (Worlds.Find(Ac, World)) {
			List = Ac.GetValueRef();
			Ac.Remove();
		}
	}

	return_if_null(List);

	for (auto Node = List->GetHead(); Node; Node = Node->GetNextNode()) {
		Node->GetValue()(World);
	}

	safe_delete(List);
}

void OnWorldDestroy_DispatchOnce(UWorld* World, TFunction<void(UWorld*)> Function)
{
	decltype(Worlds)::Accessor Ac;

	if (Worlds.Find(Ac, World)) {
		Ac.GetValueRef()->AddTail(MoveTemp(Function));
	}
	else {
		auto List = new TDoubleLinkedList<TFunction<void(UWorld*)>>;
		List->AddTail(Function);
		Ac.SetValue(List);
	}
}

void OnGameEnd_DispatchOnce(TFunction<void()> Function)
{
	GameQueue.Enqueue(MoveTemp(Function));
}

DHUniqueID GenerateUniqueID()
{
	static volatile DHUniqueID UniqueID = DH_ID_NONE;

	return FPlatformAtomics::InterlockedIncrement(&UniqueID);
}

DHUniqueID CreateInstance(TFunction<void()> DeleteFunction)
{
	DHUniqueID InstID = GenerateUniqueID();

	auto Inst = new Instance;
	Inst->DeleteFunction = MoveTemp(DeleteFunction);

	InstanceGol.Add(InstID, Inst);
	return InstID;
}

bool RemoveInstanceByID(const DHUniqueID InstanceID)
{
	decltype(InstanceGol)::Accessor Acc;
	if (InstanceGol.Find(Acc, InstanceID)) {
		Acc.Remove();
		return true;
	}

	return false;
}

void RemoveInstances()
{
	InstanceGol.Empty();
}

void MoveInstanceToWorld(const DHUniqueID InstanceID)
{
}

void MoveInstanceToRoot(const DHUniqueID InstanceID)
{
}

FDelegateHandle ExecuteNextFrame(FSimpleDelegate Delegate)
{
	ensure(IsInGameThread());
	return FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Delegate](float DeltaTime) -> bool {
		Delegate.ExecuteIfBound();
		return false;
		}), 0);
}
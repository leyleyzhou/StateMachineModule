// Copyright 2004-2019 Perfect World Co.,Ltd. All Rights Reserved.
// @author Yongquan Fan, create on 2019/5/8.

#pragma once

#include <condition_variable>
#include <thread>
#include "BaseCommon.h"
#include "Queue.h"
#include "Ticker.h"

class PWGAME_API DHQueuedAsyncProcessor
{
public:
	class Feature
	{
		friend class DHQueuedAsyncProcessor;
		DHQueuedAsyncProcessor* _Processor;

		Feature(DHQueuedAsyncProcessor* Processor) :
			_Processor(Processor)
		{}
	public:
		~Feature() = default;

		void Complete(FSimpleFunction Function)
		{
			_Processor->Complete(Forward<FSimpleFunction>(Function));
		}

		void Complete()
		{
			_Processor->Complete([] {});
		}
	};

	typedef TFunction<void(Feature*)> FStartFunction;

	DHQueuedAsyncProcessor()
	{
		_Thread = new std::thread(std::bind(&DHQueuedAsyncProcessor::Run, this));
		_TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &DHQueuedAsyncProcessor::Tick), 0.5f);
	}

	~DHQueuedAsyncProcessor()
	{
		FTicker::GetCoreTicker().RemoveTicker(_TickHandle);

		return_if_true(FPlatformAtomics::InterlockedCompareExchange(&_Active, 0, 1) != 1);
		return_if_null(_Thread);

		std::unique_lock<std::mutex> Locker(_BeginMutex);
		_Sem.notify_one();

		if (_Thread->joinable()) {
			_Thread->join();
		}

		safe_delete(_Thread);
	}

	void Start(FStartFunction Function)
	{
		std::printf("%s %d\n", __FUNCTION__, __LINE__);
		check(Function);
		check(IsInGameThread());

		_StartQueue.Enqueue(Forward<FStartFunction>(Function));

		std::unique_lock<std::mutex> Locker(_BeginMutex);
		_Sem.notify_one();
	}

	void AddMainThreadFunction(FSimpleFunction Function)
	{
		_QueueToExecInMainThread.Enqueue(Forward<FSimpleFunction>(Function));
	}

private:
	void Complete(FSimpleFunction Function)
	{
		std::printf("%s %d\n", __FUNCTION__, __LINE__);
		_CompleteQueue.Enqueue(Forward<FSimpleFunction>(Function));
	}

	void Run()
	{
		_Active = 1;

		do {
			FStartFunction Function;
			if (_StartQueue.Dequeue(Function)) {
				return_if_true(FPlatformAtomics::AtomicRead(&_Active) != 1);

				Function(&_Feature);

				continue;
			}

			std::unique_lock<std::mutex> Locker(_BeginMutex);
			_Sem.wait(Locker);
		} while (FPlatformAtomics::AtomicRead(&_Active) == 1);
	}

	bool Tick(float)
	{
		check(IsInGameThread());

		FSimpleFunction Function;
		while (_CompleteQueue.Dequeue(Function)) {
			std::printf("%s %d\n", __FUNCTION__, __LINE__);
			if (Function) {
				Function();
			}
		}

		while (_QueueToExecInMainThread.Dequeue(Function)) {
			std::printf("%s %d\n", __FUNCTION__, __LINE__);
			if (Function) {
				Function();
			}
		}

		return true;
	}

private:
	friend class Feature;
	Feature _Feature{ this };
	std::thread* _Thread = nullptr;
	FDelegateHandle _TickHandle;
	volatile int8 _Active = 0;
	std::mutex _BeginMutex;
	std::condition_variable _Sem;
	TQueue<FStartFunction, EQueueMode::Mpsc> _StartQueue;
	TQueue<FSimpleFunction, EQueueMode::Mpsc> _CompleteQueue;
	TQueue<FSimpleFunction, EQueueMode::Mpsc> _QueueToExecInMainThread;
};
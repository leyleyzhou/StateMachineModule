#pragma once

#include "BaseCommon.h"
#include "Queue.h"
#include "Containers/Ticker.h"

template <
	typename ...Args
>
class DHEventOnce
{
public:
	typedef TFunction<void(Args ...)> Function;

	DHEventOnce() = default;

	~DHEventOnce()
	{
		if (_Handle.IsValid()) {
			FTicker::GetCoreTicker().RemoveTicker(_Handle);
		}
	}

	void Add(Function Func)
	{
		_Queue.Enqueue(Forward<Function>(Func));
	}

	void Broadcast(Args ...As)
	{
		return_if_true(_Queue.IsEmpty());

		Function Func;
		while (_Queue.Peek(Func)) {
			Func(As...);
			_Queue.Pop();
		}
	}

	void BroadcastNextFrame(Args ...As)
	{
		return_if_true(_Queue.IsEmpty());

		if (_Handle.IsValid()) {
			FTicker::GetCoreTicker().RemoveTicker(_Handle);
		}

		_Handle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this, As...](float Tick) -> bool {
			Broadcast(As...);
			_Handle.Reset();
			return false;
		}), 0);
	}
private:
	TQueue<Function> _Queue;
	FDelegateHandle _Handle;
};
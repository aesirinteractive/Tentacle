// Copyright singinwhale https://www.singinwhale.com and contributors. Distributed under the MIT license.

#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Templates/UnrealTemplate.h"
#include "Templates/Function.h"
#include "Misc/Timespan.h"
#include "Templates/SharedPointer.h"
#include "Misc/DateTime.h"
#include "HAL/Event.h"
#include "HAL/PlatformProcess.h"
#include "Misc/ScopeLock.h"

/**
 * Base class for the internal state of asynchronous return values (futures).
 */
class FWeakFutureState
{
public:
	/** Default constructor. */
	FWeakFutureState()
		: CompletionEvent(FPlatformProcess::GetSynchEventFromPool(true)), Complete(false), Cancelled(false)
	{
	}

	/**
	 * Create and initialize a new instance with a callback.
	 *
	 * @param InCompletionCallback A function that is called when the state is completed.
	 */
	FWeakFutureState(TUniqueFunction<void()>&& InCompletionCallback)
		: CompletionCallback(MoveTemp(InCompletionCallback)), CompletionEvent(FPlatformProcess::GetSynchEventFromPool(true)), Complete(false)
	{
	}

	/** Destructor. */
	~FWeakFutureState()
	{
		FPlatformProcess::ReturnSynchEventToPool(CompletionEvent);
		CompletionEvent = nullptr;
	}

public:
	/**
	 * Checks whether the asynchronous result has been set.
	 *
	 * @return true if the result has been set, false otherwise.
	 * @see WaitFor
	 */
	bool IsComplete() const
	{
		return Complete;
	}

	bool WasCancelled() const
	{
		return Cancelled;
	}

	/**
	 * Blocks the calling thread until the future result is available.
	 *
	 * @param Duration The maximum time span to wait for the future result.
	 * @return true if the result is available, false otherwise.
	 * @see IsComplete
	 */
	bool WaitFor(const FTimespan& Duration) const
	{
		if (CompletionEvent->Wait(Duration))
		{
			return true;
		}

		return false;
	}

	/**
	 * Set a continuation to be called on completion of the promise
	 * @param Continuation
	 */
	void SetContinuation(TUniqueFunction<void()>&& Continuation)
	{
		bool bShouldJustRun = IsComplete();
		if (!bShouldJustRun)
		{
			FScopeLock Lock(&Mutex);
			bShouldJustRun = IsComplete();
			if (!bShouldJustRun)
			{
				CompletionCallback = MoveTemp(Continuation);
			}
		}
		if (bShouldJustRun && Continuation)
		{
			Continuation();
		}
	}

	void Cancel()
	{
		MarkCancelled();
	}

	void PromiseCount_Acquire()
	{
		FScopeLock Lock(&Mutex);
		PromiseCount.Increment();
	}

	void PromiseCount_Release()
	{
		int32 Count = PromiseCount.Decrement();
		if (Count == 0)
		{
			MarkCancelled();
		}
	}

protected:
	void MarkCancelled()
	{
		if (IsComplete())
			return;

		Cancelled = true;
		MarkComplete();
	};

	/** Notifies any waiting threads that the result is available. */
	void MarkComplete()
	{
		TUniqueFunction<void()> Continuation;
		{
			FScopeLock Lock(&Mutex);
			Continuation = MoveTemp(CompletionCallback);
			Complete = true;
		}
		CompletionEvent->Trigger();

		if (Continuation)
		{
			Continuation();
		}
	}

private:
	/** Mutex used to allow proper handling of continuations */
	mutable FCriticalSection Mutex;

	/** An optional callback function that is executed the state is completed. */
	TUniqueFunction<void()> CompletionCallback;

	/** Holds an event signaling that the result is available. */
	FEvent* CompletionEvent;

	/** Whether the asynchronous result is available. */
	TAtomic<bool> Complete;
	TAtomic<bool> Cancelled;
	FThreadSafeCounter PromiseCount;
};


/**
 * Implements the internal state of asynchronous return values (futures).
 */
template <typename InternalResultType>
class TWeakFutureState
	: public FWeakFutureState
{
public:
	/** Default constructor. */
	TWeakFutureState()
		: FWeakFutureState()
	{
	}

	~TWeakFutureState()
	{
	}

	/**
	 * Create and initialize a new instance with a callback.
	 *
	 * @param CompletionCallback A function that is called when the state is completed.
	 */
	TWeakFutureState(TUniqueFunction<void()>&& CompletionCallback)
		: FWeakFutureState(MoveTemp(CompletionCallback))
	{
	}

public:
	/**
	 * Gets the result (will block the calling thread until the result is available).
	 *
	 * @return The result value.
	 * @see EmplaceResult
	 */
	const TOptional<InternalResultType>& GetResult() const UE_LIFETIMEBOUND
	{
		while (!IsComplete())
		{
			WaitFor(FTimespan::MaxValue());
		}

		return Result;
	}

	/**
	 * Gets the result (will block the calling thread until the result is available).
	 *
	 * @return The result value.
	 * @see EmplaceResult
	 */
	TOptional<InternalResultType>& GetResult() UE_LIFETIMEBOUND
	{
		return Result;
	}

	/**
	 * Sets the result and notifies any waiting threads.
	 *
	 * @param Args The arguments to forward to the constructor of the result.
	 * @see GetResult
	 */
	template <typename... ArgTypes>
	void EmplaceResult(ArgTypes&&... Args)
	{
		check(!IsComplete());
		Result.Emplace(Forward<ArgTypes>(Args)...);
		MarkComplete();
	}

private:
	/** Holds the asynchronous result. */
	TOptional<InternalResultType> Result;
};

template <>
class TWeakFutureState<void>
	: public FWeakFutureState
{
public:
	void EmplaceResult()
	{
		MarkComplete();
	}
};

/* TWeakFuture
*****************************************************************************/

/**
 * Abstract base template for futures and shared futures.
 */
template <typename InternalResultType>
class TWeakFutureBase
{
public:
	using FInternalResultType = InternalResultType;
	/**
	 * Checks whether this future object has its value set.
	 *
	 * @return true if this future has a shared state and the value has been set, false otherwise.
	 * @see IsValid
	 */
	bool IsReady() const
	{
		return State.IsValid() ? State->IsComplete() : false;
	}

	/**
	 * Checks whether this future object has a valid state.
	 *
	 * @return true if the state is valid, false otherwise.
	 * @see IsReady
	 */
	bool IsValid() const
	{
		return State.IsValid();
	}

	bool WasCancelled() const
	{
		return State->WasCancelled();
	}

	/**
	 * Blocks the calling thread until the future result is available.
	 *
	 * Note that this method may block forever if the result is never set. Use
	 * the WaitFor or WaitUntil methods to specify a maximum timeout for the wait.
	 *
	 * @see WaitFor, WaitUntil
	 */
	void Wait() const
	{
		if (State.IsValid())
		{
			while (!WaitFor(FTimespan::MaxValue()));
		}
	}

	/**
	 * Blocks the calling thread until the future result is available or the specified duration is exceeded.
	 *
	 * @param Duration The maximum time span to wait for the future result.
	 * @return true if the result is available, false otherwise.
	 * @see Wait, WaitUntil
	 */
	bool WaitFor(const FTimespan& Duration) const
	{
		return State.IsValid() ? State->WaitFor(Duration) : false;
	}

	/**
	 * Blocks the calling thread until the future result is available or the specified time is hit.
	 *
	 * @param Time The time until to wait for the future result (in UTC).
	 * @return true if the result is available, false otherwise.
	 * @see Wait, WaitUntil
	 */
	bool WaitUntil(const FDateTime& Time) const
	{
		return WaitFor(Time - FDateTime::UtcNow());
	}

protected:
	typedef TSharedPtr<TWeakFutureState<InternalResultType>, ESPMode::ThreadSafe> StateType;

	/** Default constructor. */
	TWeakFutureBase() = default;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InState The shared state to initialize with.
	 */
	TWeakFutureBase(const StateType& InState)
		: State(InState)
	{
	}

	/** Protected copy constructor. */
	TWeakFutureBase(const TWeakFutureBase&) = default;

	/** Protected copy assignment operator. */
	TWeakFutureBase& operator=(const TWeakFutureBase&) = default;

	/** Protected move constructor. */
	TWeakFutureBase(TWeakFutureBase&&) = default;

	/** Protected move assignment operator. */
	TWeakFutureBase& operator=(TWeakFutureBase&&) = default;

	/** Protected destructor. */
	~TWeakFutureBase() = default;

protected:
	/**
	 * Gets the shared state object.
	 *
	 * @return The shared state object.
	 */
	const StateType& GetState() const
	{
		// if you hit this assertion then your future has an invalid state.
		// this happens if you have an uninitialized future or if you moved
		// it to another instance.
		check(State.IsValid());

		return State;
	}

	/**
	 * Set a completion callback that will be called once the future completes
	 *	or immediately if already completed
	 *
	 * @param Continuation a continuation taking an argument of type TWeakFuture<InternalResultType>
	 * @return nothing at the moment but could return another future to allow future chaining
	 */
	template <typename Func>
	auto Then(Func Continuation);

	/**
	 * Convenience wrapper for Then that
	 *	set a completion callback that will be called once the future completes
	 *	or immediately if already completed
	 * @param Continuation a continuation taking an argument of type InternalResultType
	 * @return nothing at the moment but could return another future to allow future chaining
	 */
	template <typename Func>
	auto Next(Func Continuation);

	/**
	 * Reset the future.
	 *	Resetting a future removes any continuation from its shared state and invalidates it.
	 *	Useful for discarding yet to be completed future cleanly.
	 */
	void Reset()
	{
		if (IsValid())
		{
			this->State->SetContinuation(nullptr);
			this->State.Reset();
		}
	}

private:
	/** Holds the future's state. */
	StateType State;
};


template <typename ResultType>
class TWeakSharedFuture;

/**
 * Template for unshared futures.
 */
template <typename ResultType>
class TWeakFuture
	: public TWeakFutureBase<ResultType>
{
	typedef TWeakFutureBase<ResultType> BaseType;

public:
	/** Default constructor. */
	TWeakFuture() = default;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InState The shared state to initialize with.
	 */
	TWeakFuture(const typename BaseType::StateType& InState)
		: BaseType(InState)
	{
	}

	/** Deleted copy constructor (futures cannot be copied). */
	TWeakFuture(const TWeakFuture&) = delete;

	/** Deleted copy assignment operator (futures cannot be copied). */
	TWeakFuture& operator=(const TWeakFuture&) = delete;

	/** Move constructor. */
	TWeakFuture(TWeakFuture&&) = default;

	/** Move assignment operator. */
	TWeakFuture& operator=(TWeakFuture&&) = default;

	/** Destructor. */
	~TWeakFuture() = default;

public:
	/**
	 * Gets the future's result by const reference.
	 *
	 * @return The result.
	 * @note Not equivalent to std::future::get(). The future remains valid.
	 */
	const TOptional<ResultType>& Get() const UE_LIFETIMEBOUND
	{
		return this->GetState()->GetResult();
	}

	/**
	 * Gets the future's result by non-const reference.
	 *
	 * @return The result.
	 * @note Not equivalent to std::future::get(). The future remains valid.
	 */
	TOptional<ResultType>& GetMutable() UE_LIFETIMEBOUND
	{
		return this->GetState()->GetResult();
	}

	/**
	 * Consumes the future's result and invalidates the future.
	 *
	 * @return The result.
	 * @note Equivalent to std::future::get(). Invalidates the future.
	 */
	TOptional<ResultType> Consume()
	{
		TWeakFuture<ResultType> Local(MoveTemp(*this));
		return MoveTemp(Local.GetState()->GetResult());
	}

	/**
	 * Moves this future's state into a shared future.
	 *
	 * @return The shared future object.
	 */
	TWeakSharedFuture<ResultType> Share()
	{
		return TWeakSharedFuture<ResultType>(MoveTemp(*this));
	}

	/**
	 * Expose Then functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Then;

	/**
	 * Expose Next functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Next;

	/**
	 * Expose Reset functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Reset;
};


/**
 * Template for unshared futures (specialization for reference types).
 */
template <typename ResultType>
class TWeakFuture<ResultType&>
	: public TWeakFutureBase<ResultType&>
{
	typedef TWeakFutureBase<ResultType&> BaseType;

public:
	/** Default constructor. */
	TWeakFuture() = default;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InState The shared state to initialize with.
	 */
	TWeakFuture(const typename BaseType::StateType& InState)
		: BaseType(InState)
	{
	}

	/** Deleted copy constructor (futures cannot be copied). */
	TWeakFuture(const TWeakFuture&) = delete;

	/** Deleted copy assignment operator (futures cannot be copied). */
	TWeakFuture& operator=(const TWeakFuture&) = delete;

	/** Move constructor. */
	TWeakFuture(TWeakFuture&&) = default;

	/** Move assignment operator. */
	TWeakFuture& operator=(TWeakFuture&&) = default;

	/** Destructor. */
	~TWeakFuture() = default;

public:
	/**
	 * Gets the future's result.
	 *
	 * @return The result.
	 * @note Not equivalent to std::future::get(). The future remains valid.
	 */
	TOptional<ResultType&> Get() const
	{
		return *this->GetState()->GetResult();
	}

	/**
	 * Gets the future's result.
	 *
	 * @return The result.
	 * @note Not equivalent to std::future::get(). The future remains valid.
	 */
	TOptional<ResultType&> GetMutable()
	{
		return this->GetState()->GetResult();
	}

	/**
	 * Consumes the future's result and invalidates the future.
	 *
	 * @return The result.
	 * @note Equivalent to std::future::get(). Invalidates the future.
	 */
	TOptional<ResultType&> Consume()
	{
		TWeakFuture<ResultType&> Local(MoveTemp(*this));
		return Local.GetState()->GetResult();
	}

	/**
	 * Moves this future's state into a shared future.
	 *
	 * @return The shared future object.
	 */
	TWeakSharedFuture<ResultType&> Share()
	{
		return TWeakSharedFuture<ResultType&>(MoveTemp(*this));
	}

	/**
	 * Expose Then functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Then;

	/**
	 * Expose Next functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Next;

	/**
	 * Expose Reset functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Reset;
};


/**
 * Template for unshared futures (specialization for void).
 */
template <>
class TWeakFuture<void>
	: public TWeakFutureBase<void>
{
	typedef TWeakFutureBase<void> BaseType;

public:
	/** Default constructor. */
	TWeakFuture() = default;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InState The shared state to initialize with.
	 */
	TWeakFuture(const BaseType::StateType& InState)
		: BaseType(InState)
	{
	}

	/** Deleted copy constructor (futures cannot be copied). */
	TWeakFuture(const TWeakFuture&) = delete;

	/** Deleted copy assignment operator (futures cannot be copied). */
	TWeakFuture& operator=(const TWeakFuture&) = delete;

	/** Move constructor. */
	TWeakFuture(TWeakFuture&&) = default;

	/** Move assignment operator. */
	TWeakFuture& operator=(TWeakFuture&&) = default;

	/** Destructor. */
	~TWeakFuture() = default;

public:
	/**
	 * Moves this future's state into a shared future.
	 *
	 * @return The shared future object.
	 */
	TWeakSharedFuture<void> Share();

	/**
	 * Expose Then functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Then;

	/**
	 * Expose Next functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Next;

	/**
	 * Expose Reset functionality
	 * @see TWeakFutureBase
	 */
	using BaseType::Reset;
};

/**
 * Template for unshared futures.
 */
template <typename... ResultTypes>
class TWeakFutureSet
	: public TWeakFuture<TTuple<ResultTypes...>>
{
	typedef TWeakFuture<TTuple<ResultTypes...>> Super;

public:
	TWeakFutureSet(TWeakFuture<TTuple<ResultTypes...>>&& WeakFuture)
		: Super(MoveTemp(WeakFuture))
	{}

	/**
	 * Expose Next functionality
	 * @see TWeakFutureBase
	 */
	// Next implementation
	template <typename Func>
	auto ExpandNext(Func Continuation) //-> TWeakFuture<decltype(Continuation(Consume()))>
	{
		return this->Then(
			[Continuation = MoveTemp(Continuation)](TWeakFuture<TTuple<ResultTypes...>> Self) mutable
			{
				if (Self.WasCancelled())
				{
					return TTuple<ResultTypes...>().ApplyBefore(Continuation);
				}
				else
				{
					return Self.Consume()->ApplyBefore(Continuation);
				}
			}
		);
	}
};

/* TWeakSharedFuture
*****************************************************************************/

/**
 * Template for shared futures.
 */
template <typename ResultType>
class TWeakSharedFuture
	: public TWeakFutureBase<ResultType>
{
	typedef TWeakFutureBase<ResultType> BaseType;

public:
	/** Default constructor. */
	TWeakSharedFuture() = default;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InState The shared state to initialize from.
	 */
	TWeakSharedFuture(const typename BaseType::StateType& InState)
		: BaseType(InState)
	{
	}

	/**
	 * Creates and initializes a new instances from a future object.
	 *
	 * @param Future The future object to initialize from.
	 */
	TWeakSharedFuture(TWeakFuture<ResultType>&& Future)
		: BaseType(MoveTemp(Future))
	{
	}

	/** Copy constructor. */
	TWeakSharedFuture(const TWeakSharedFuture&) = default;

	/** Copy assignment operator. */
	TWeakSharedFuture& operator=(const TWeakSharedFuture&) = default;

	/** Move constructor. */
	TWeakSharedFuture(TWeakSharedFuture&&) = default;

	/** Move assignment operator. */
	TWeakSharedFuture& operator=(TWeakSharedFuture&&) = default;

	/** Destructor. */
	~TWeakSharedFuture() = default;

public:
	/**
	 * Gets the future's result.
	 *
	 * @return The result.
	 */
	const ResultType& Get() const
	{
		return this->GetState()->GetResult();
	}
};


/**
 * Template for shared futures (specialization for reference types).
 */
template <typename ResultType>
class TWeakSharedFuture<ResultType&>
	: public TWeakFutureBase<ResultType&>
{
	typedef TWeakFutureBase<ResultType&> BaseType;

public:
	/** Default constructor. */
	TWeakSharedFuture() = default;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InState The shared state to initialize from.
	 */
	TWeakSharedFuture(const typename BaseType::StateType& InState)
		: BaseType(InState)
	{
	}

	/**
	* Creates and initializes a new instances from a future object.
	*
	* @param Future The future object to initialize from.
	*/
	TWeakSharedFuture(TWeakFuture<ResultType>&& Future)
		: BaseType(MoveTemp(Future))
	{
	}

	/** Copy constructor. */
	TWeakSharedFuture(const TWeakSharedFuture&) = default;

	/** Copy assignment operator. */
	TWeakSharedFuture& operator=(const TWeakSharedFuture&) = default;

	/** Move constructor. */
	TWeakSharedFuture(TWeakSharedFuture&&) = default;

	/** Move assignment operator. */
	TWeakSharedFuture& operator=(TWeakSharedFuture&&) = default;

	/** Destructor. */
	~TWeakSharedFuture() = default;

public:
	/**
	 * Gets the future's result.
	 *
	 * @return The result.
	 */
	ResultType& Get() const
	{
		return *this->GetState()->GetResult();
	}
};


/**
 * Template for shared futures (specialization for void).
 */
template <>
class TWeakSharedFuture<void>
	: public TWeakFutureBase<void>
{
	typedef TWeakFutureBase<void> BaseType;

public:
	/** Default constructor. */
	TWeakSharedFuture() = default;

	/**
	 * Creates and initializes a new instance from shared state.
	 *
	 * @param InState The shared state to initialize from.
	 */
	TWeakSharedFuture(const BaseType::StateType& InState)
		: BaseType(InState)
	{
	}

	/**
	 * Creates and initializes a new instances from a future object.
	 *
	 * @param Future The future object to initialize from.
	 */
	TWeakSharedFuture(TWeakFuture<void>&& Future)
		: BaseType(MoveTemp(Future))
	{
	}

	/** Copy constructor. */
	TWeakSharedFuture(const TWeakSharedFuture&) = default;

	/** Copy assignment operator. */
	TWeakSharedFuture& operator=(const TWeakSharedFuture&) = default;

	/** Move constructor. */
	TWeakSharedFuture(TWeakSharedFuture&&) = default;

	/** Move assignment operator. */
	TWeakSharedFuture& operator=(TWeakSharedFuture&&) = default;

	/** Destructor. */
	~TWeakSharedFuture() = default;
};


inline TWeakSharedFuture<void> TWeakFuture<void>::Share()
{
	return TWeakSharedFuture<void>(MoveTemp(*this));
}


/* TWeakPromise
*****************************************************************************/

template <typename InternalResultType>
class TWeakPromiseBase
{
	typedef TSharedPtr<TWeakFutureState<InternalResultType>, ESPMode::ThreadSafe> StateType;

public:
	/** Default constructor. */
	TWeakPromiseBase()
		: State(MakeShared<TWeakFutureState<InternalResultType>, ESPMode::ThreadSafe>())
	{
		State->PromiseCount_Acquire();
	}

	/**
	 * Copy constructor.
	 *
	 * @param Other The promise holding the shared state to copy.
	 */
	TWeakPromiseBase(const TWeakPromiseBase& Other)
		: State(Other.State)
	{
		if (State)
		{
			State->PromiseCount_Acquire();
		}
	}

	/**
	 * Move constructor.
	 *
	 * @param Other The promise holding the shared state to move.
	 */
	TWeakPromiseBase(TWeakPromiseBase&& Other)
		: State(MoveTemp(Other.State))
	{
		Other.State.Reset();
	}

	/**
	 * Create and initialize a new instance with a callback.
	 *
	 * @param CompletionCallback A function that is called when the future state is completed.
	 */
	TWeakPromiseBase(TUniqueFunction<void()>&& CompletionCallback)
		: State(MakeShared<TWeakFutureState<InternalResultType>, ESPMode::ThreadSafe>(MoveTemp(CompletionCallback)))
	{
		State->PromiseCount_Acquire();
	}

public:
	/**
	 * Copy assignment operator.
	 *
	 * @param Other The promise holding the shared state to copy.
	 */
	TWeakPromiseBase& operator=(const TWeakPromiseBase& Other)
	{
		State = Other.State;
		if (State)
		{
			State->PromiseCount_Acquire();
		}
		return *this;
	}

	/** Move assignment operator. */
	TWeakPromiseBase& operator=(TWeakPromiseBase&& Other)
	{
		State = Other.State;
		Other.State.Reset();
		return *this;
	}

protected:
	/** Destructor. */
	~TWeakPromiseBase()
	{
		if (State.IsValid())
		{
			State->PromiseCount_Release();
		}
	}

	/**
	 * Gets the shared state object.
	 *
	 * @return The shared state object.
	 */
	const StateType& GetState()
	{
		// if you hit this assertion then your promise has an invalid state.
		// this happens if you move the promise to another instance.
		check(State.IsValid());

		return State;
	}

private:
	/** Holds the shared state object. */
	StateType State;
};


/**
 * Template for promises.
 */
template <typename ResultType>
class TWeakPromise
	: public TWeakPromiseBase<ResultType>
{
public:
	typedef TWeakPromiseBase<ResultType> BaseType;

	/** Default constructor (creates a new shared state). */
	TWeakPromise()
		: BaseType(), FutureRetrieved(false)
	{
	}

	/**
	 * Copy constructor.
	 *
	 * @param Other The promise holding the shared state to copy.
	 */
	TWeakPromise(const TWeakPromise& Other)
		: BaseType(Other), FutureRetrieved(Other.FutureRetrieved)
	{
	}

	/**
	 * Move constructor.
	 *
	 * @param Other The promise holding the shared state to move.
	 */
	TWeakPromise(TWeakPromise&& Other)
		: BaseType(MoveTemp(Other)), FutureRetrieved(MoveTemp(Other.FutureRetrieved))
	{
	}

	/**
	 * Create and initialize a new instance with a callback.
	 *
	 * @param CompletionCallback A function that is called when the future state is completed.
	 */
	TWeakPromise(TUniqueFunction<void()>&& CompletionCallback)
		: BaseType(MoveTemp(CompletionCallback)), FutureRetrieved(false)
	{
	}

public:
	/**
	 * Copy assignment operator.
	 *
	 * @param Other The promise holding the shared state to copy.
	 */
	TWeakPromise& operator=(const TWeakPromise& Other)
	{
		BaseType::operator=(Other);
		FutureRetrieved = Other.FutureRetrieved;

		return *this;
	}

	/**
	 * Move assignment operator.
	 *
	 * @param Other The promise holding the shared state to move.
	 */
	TWeakPromise& operator=(TWeakPromise&& Other)
	{
		BaseType::operator=(MoveTemp(Other));
		FutureRetrieved = MoveTemp(Other.FutureRetrieved);

		return *this;
	}

public:
	/**
	 * Gets a TWeakFuture object associated with the shared state of this promise.
	 *
	 * @return The TWeakFuture object.
	 */
	TWeakFuture<ResultType> GetWeakFuture()
	{
		check(!FutureRetrieved);
		FutureRetrieved = true;

		return TWeakFuture<ResultType>(this->GetState());
	}

	/**
	 * Sets the promised result.
	 *
	 * The result must be set only once. An assertion will
	 * be triggered if this method is called a second time.
	 *
	 * @param Result The result value to set.
	 */
	FORCEINLINE void SetValue(const ResultType& Result)
	{
		EmplaceValue(Result);
	}

	/**
	 * Sets the promised result (from rvalue).
	 *
	 * The result must be set only once. An assertion will
	 * be triggered if this method is called a second time.
	 *
	 * @param Result The result value to set.
	 */
	FORCEINLINE void SetValue(ResultType&& Result)
	{
		EmplaceValue(MoveTemp(Result));
	}

	/**
	 * Sets the promised result.
	 *
	 * The result must be set only once. An assertion will
	 * be triggered if this method is called a second time.
	 *
	 * @param Args The arguments to forward to the constructor of the result.
	 */
	template <typename... ArgTypes>
	void EmplaceValue(ArgTypes&&... Args)
	{
		this->GetState()->EmplaceResult(Forward<ArgTypes>(Args)...);
	}

	void Cancel()
	{
		this->GetState()->Cancel();
	}

private:
	/** Whether a future has already been retrieved from this promise. */
	bool FutureRetrieved;
};


/**
 * Template for promises (specialization for reference types).
 */
template <typename ResultType>
class TWeakPromise<ResultType&>
	: public TWeakPromiseBase<ResultType&>
{
	typedef TWeakPromiseBase<ResultType&> BaseType;

public:
	/** Default constructor (creates a new shared state). */
	TWeakPromise()
		: BaseType(), FutureRetrieved(false)
	{
	}

	/**
	 * Copy constructor.
	 *
	 * @param Other The promise holding the shared state to copy.
	 */
	TWeakPromise(const TWeakPromise& Other)
		: BaseType(Other), FutureRetrieved(Other.FutureRetrieved)
	{
	}

	/**
	 * Move constructor.
	 *
	 * @param Other The promise holding the shared state to move.
	 */
	TWeakPromise(TWeakPromise&& Other)
		: BaseType(MoveTemp(Other)), FutureRetrieved(MoveTemp(Other.FutureRetrieved))
	{
	}

	/**
	 * Create and initialize a new instance with a callback.
	 *
	 * @param CompletionCallback A function that is called when the future state is completed.
	 */
	TWeakPromise(TUniqueFunction<void()>&& CompletionCallback)
		: BaseType(MoveTemp(CompletionCallback)), FutureRetrieved(false)
	{
	}

public:
	/**
	 * Copy assignment operator.
	 *
	 * @param Other The promise holding the shared state to copy.
	 */
	TWeakPromise& operator=(const TWeakPromise& Other)
	{
		BaseType::operator=(Other);
		FutureRetrieved = Other.FutureRetrieved;

		return *this;
	}

	/**
	 * Move assignment operator.
	 *
	 * @param Other The promise holding the shared state to move.
	 */
	TWeakPromise& operator=(TWeakPromise&& Other)
	{
		BaseType::operator=(MoveTemp(Other));
		FutureRetrieved = MoveTemp(Other.FutureRetrieved);

		return this;
	}

public:
	/**
	 * Gets a TWeakFuture object associated with the shared state of this promise.
	 *
	 * @return The TWeakFuture object.
	 */
	TWeakFuture<ResultType&> GetWeakFuture()
	{
		check(!FutureRetrieved);
		FutureRetrieved = true;

		return TWeakFuture<ResultType&>(this->GetState());
	}

	/**
	 * Sets the promised result.
	 *
	 * The result must be set only once. An assertion will
	 * be triggered if this method is called a second time.
	 *
	 * @param Result The result value to set.
	 */
	FORCEINLINE void SetValue(ResultType& Result)
	{
		EmplaceValue(Result);
	}

	/**
	 * Sets the promised result.
	 *
	 * The result must be set only once. An assertion will
	 * be triggered if this method is called a second time.
	 *
	 * @param Result The result value to set.
	 */
	void EmplaceValue(ResultType& Result)
	{
		this->GetState()->EmplaceResult(Result);
	}

	void Cancel()
	{
		this->GetState()->Cancel();
	}

private:
	/** Whether a future has already been retrieved from this promise. */
	bool FutureRetrieved;
};


/**
 * Template for promises (specialization for void results).
 */
template <>
class TWeakPromise<void>
	: public TWeakPromiseBase<void>
{
	typedef TWeakPromiseBase<void> BaseType;

public:
	/** Default constructor (creates a new shared state). */
	TWeakPromise()
		: BaseType(), FutureRetrieved(false)
	{
	}

	/**
	 * Move constructor.
	 *
	 * @param Other The promise holding the shared state to move.
	 */
	TWeakPromise(TWeakPromise&& Other)
		: BaseType(MoveTemp(Other)), FutureRetrieved(false)
	{
	}

	/**
	 * Create and initialize a new instance with a callback.
	 *
	 * @param CompletionCallback A function that is called when the future state is completed.
	 */
	TWeakPromise(TUniqueFunction<void()>&& CompletionCallback)
		: BaseType(MoveTemp(CompletionCallback)), FutureRetrieved(false)
	{
	}

public:
	/**
	 * Copy assignment operator.
	 *
	 * @param Other The promise holding the shared state to copy.
	 */
	TWeakPromise& operator=(const TWeakPromise& Other)
	{
		BaseType::operator=(Other);
		FutureRetrieved = Other.FutureRetrieved;

		return *this;
	}

	/**
	 * Move assignment operator.
	 *
	 * @param Other The promise holding the shared state to move.
	 */
	TWeakPromise& operator=(TWeakPromise&& Other)
	{
		BaseType::operator=(MoveTemp(Other));
		FutureRetrieved = MoveTemp(Other.FutureRetrieved);

		return *this;
	}

public:
	/**
	 * Gets a TWeakFuture object associated with the shared state of this promise.
	 *
	 * @return The TWeakFuture object.
	 */
	TWeakFuture<void> GetWeakFuture()
	{
		check(!FutureRetrieved);
		FutureRetrieved = true;

		return TWeakFuture<void>(GetState());
	}

	/**
	 * Sets the promised result.
	 *
	 * The result must be set only once. An assertion will
	 * be triggered if this method is called a second time.
	 */
	FORCEINLINE void SetValue()
	{
		EmplaceValue();
	}

	/**
	 * Sets the promised result.
	 *
	 * The result must be set only once. An assertion will
	 * be triggered if this method is called a second time.
	 */
	void EmplaceValue()
	{
		this->GetState()->EmplaceResult();
	}

	void Cancel()
	{
		this->GetState()->Cancel();
	}

private:
	/** Whether a future has already been retrieved from this promise. */
	bool FutureRetrieved;
};


template <class... ResultTypes>
class TWeakPromiseSet
	: public TWeakPromise<TTuple<ResultTypes...>>
{
	using BaseType = TWeakPromise<TTuple<ResultTypes...>>;

public:
	TWeakFutureSet<ResultTypes...> GetWeakFutureSet()
	{
		return this->GetWeakFuture();
	}

private:
	// making this private since we want to return the set version
	using BaseType::GetWeakFuture;
};


template <class... ResultTypes>
class TWeakSharedPromiseSet
	: public TWeakPromise<TTuple<ResultTypes...>>
{
	using BaseType = TWeakPromise<TTuple<ResultTypes...>>;

public:
	TWeakFutureSet<ResultTypes...> GetWeakFutureSet()
	{
		return this->GetWeakFuture();
	}

private:
	// making this private since we want to return the set version
	using BaseType::GetWeakFuture;
};

/* TWeakFuture::Then
*****************************************************************************/

namespace FutureDetail
{
	/**
	* Template for setting a promise value from a continuation.
	*/
	template <typename Func, typename ParamType, typename ResultType>
	inline void SetWeakPromiseValue(TWeakPromise<ResultType>& Promise, Func& Function, TWeakFuture<ParamType>&& Param)
	{
		Promise.SetValue(Function(MoveTemp(Param)));
	}

	template <typename Func, typename ParamType>
	inline void SetWeakPromiseValue(TWeakPromise<void>& Promise, Func& Function, TWeakFuture<ParamType>&& Param)
	{
		Function(MoveTemp(Param));
		Promise.SetValue();
	}
}

// Then implementation
template <typename InternalResultType>
template <typename Func>
auto TWeakFutureBase<InternalResultType>::Then(Func Continuation) //-> TWeakFuture<decltype(Continuation(MoveTemp(TWeakFuture<InternalResultType>())))>
{
	check(IsValid());
	using ReturnValue = decltype(Continuation(MoveTemp(TWeakFuture<InternalResultType>())));

	TWeakPromise<ReturnValue> Promise;
	TWeakFuture<ReturnValue> FutureResult = Promise.GetWeakFuture();
	TUniqueFunction<void()> Callback = [PromiseCapture = MoveTemp(Promise), ContinuationCapture = MoveTemp(Continuation), StateCapture = this->State]() mutable
	{
		if (StateCapture->WasCancelled())
		{
			ContinuationCapture(TWeakFuture<InternalResultType>(MoveTemp(StateCapture)));
			PromiseCapture.Cancel();
		}
		else
		{
			FutureDetail::SetWeakPromiseValue(PromiseCapture, ContinuationCapture, TWeakFuture<InternalResultType>(MoveTemp(StateCapture)));
		}
	};

	// This invalidates this future.
	StateType MovedState = MoveTemp(this->State);
	MovedState->SetContinuation(MoveTemp(Callback));
	return FutureResult;
}

// Next implementation
template <typename InternalResultType>
template <typename Func>
auto TWeakFutureBase<InternalResultType>::Next(Func Continuation) //-> TWeakFuture<decltype(Continuation(Consume()))>
{
	return this->Then(
		[Continuation = MoveTemp(Continuation)](TWeakFuture<InternalResultType> Self) mutable
		{
			if constexpr (std::is_same_v<InternalResultType, void>)
			{
				return Continuation(Self.WasCancelled());
			}
			else
			{
				if (Self.WasCancelled())
				{
					return Continuation(TOptional<InternalResultType>{});
				}
				else
				{
					return Continuation(Self.Consume());
				}
			}
		}
	);
}

/** Helper to create and immediately fulfill a promise */
template <typename ResultType, typename... ArgTypes>
TWeakPromise<ResultType> MakeFulfilledWeakPromise(ArgTypes&&... Args)
{
	TWeakPromise<ResultType> Promise;
	Promise.EmplaceValue(Forward<ArgTypes>(Args)...);
	return Promise;
}


/** Helper to create and immediately fulfill a promise */
template <typename ResultType>
TPair<TWeakPromise<ResultType>, TWeakFuture<ResultType>> MakeWeakPromisePair()
{
	TWeakPromise<ResultType> Promise;
	return {MoveTemp(Promise), Promise.GetWeakFuture()};
}

/** Helper to create and immediately fulfill a promise set */
template <typename... ResultTypes>
TPair<TWeakPromiseSet<ResultTypes...>, TWeakFutureSet<ResultTypes...>> MakeWeakPromiseSetPair()
{
	TWeakPromiseSet<ResultTypes...> Promise;
	return {MoveTemp(Promise), Promise.GetWeakFuture()};
}

namespace AwaitAllWeakPrivate
{
	template <class... ResultTypes>
	auto MakeTupleValidFunction(TSharedRef<TTuple<TOptional<ResultTypes>...>> BufferState, TIntegerSequence<int32>)
	{
		return [](TSharedRef<TTuple<TOptional<ResultTypes>...>> BufferState)
		{
			return true;
		};
	}

	template <int32 I, int32 ...Is, class... ResultTypes>
	auto MakeTupleValidFunction(TSharedRef<TTuple<TOptional<ResultTypes>...>> BufferState, TIntegerSequence<int32, I, Is...>)
	{
		auto NextValidFunc = MakeTupleValidFunction(BufferState, TIntegerSequence<int32, Is...>());
		return [NextValidFunc](TSharedRef<TTuple<TOptional<ResultTypes>...>> BufferState)
		{
			return bool(BufferState->template Get<I>()) && NextValidFunc(BufferState);
		};
	}

	template <class... AllTs, typename TNewResultCallback>
	void CollectFutures(TSharedRef<TTuple<TOptional<TOptional<AllTs>>...>>& BufferState, TNewResultCallback& CheckValidFunc, TTuple<TWeakFuture<AllTs>...>& Futures, TIntegerSequence<int32>)
	{
	}

	template <int32 I, int32 ...Is, class... AllTs, typename TNewResultCallback>
	void CollectFutures(TSharedRef<TTuple<TOptional<TOptional<AllTs>>...>>& BufferState, TNewResultCallback& NewResultCallback, TTuple<TWeakFuture<AllTs>...>& Futures, TIntegerSequence<int32, I, Is...>)
	{
		Futures.template Get<I>().Next(
			[NewResultCallback, BufferState](auto OptionalResult) mutable
			{
				BufferState->template Get<I>() = OptionalResult;
				NewResultCallback(BufferState);
			}
		);
		CollectFutures(BufferState, NewResultCallback, Futures, TIntegerSequence<int32,Is...>());
	}
}

/**
 * Await a tuple of futures.
 * This is very helpful for template magic involving variadic functions.
 * Each weak future will be resolved to an optional value type.
 * Once all futures have returned or have been dropped the future will complete.
 */
template <class... ValTypes>
TWeakFutureSet<TOptional<ValTypes>...> AwaitAllWeak(TTuple<TWeakFuture<ValTypes>...> Futures)
{
	// We have to buffer the future values as they trickle in. Ideally we would do this inside the future state but we do not have time for that right now.
	// In the meantime, we will have to live with this extra allocation.
	// We use nested optionals to differentiate between futures that are still pending and ones that finished unsuccessfully.
	TSharedRef<TTuple<TOptional<TOptional<ValTypes>>...>> BufferState = MakeShared<TTuple<TOptional<TOptional<ValTypes>>...>>();
	TWeakPromiseSet<TOptional<ValTypes>...> Promise = {};
	TWeakFutureSet<TOptional<ValTypes>...> FutureSet = Promise.GetWeakFutureSet();
	auto TupleValidFunc = AwaitAllWeakPrivate::MakeTupleValidFunction(BufferState, TMakeIntegerSequence<int32, sizeof...(ValTypes)>());

	auto PromiseFulfillableFunc = [Promise = MoveTemp(Promise), TupleValidFunc](TSharedRef<TTuple<TOptional<TOptional<ValTypes>>...>> BufferState) mutable
	{
		if (TupleValidFunc(BufferState))
		{
			TTuple<TOptional<ValTypes>...> FinishedResults = TransformTuple( *BufferState, [](const auto& Result)
			{
				checkf(Result.IsSet(), TEXT("TupleValidFunc was supposed to validate that all results are in!"));
				return *Result;
			});
			Promise.EmplaceValue(MoveTemp(FinishedResults));
		}
	};

	AwaitAllWeakPrivate::CollectFutures( BufferState, PromiseFulfillableFunc, Futures, TMakeIntegerSequence<int32, sizeof...(ValTypes)>() );
	return MoveTemp(FutureSet);
}

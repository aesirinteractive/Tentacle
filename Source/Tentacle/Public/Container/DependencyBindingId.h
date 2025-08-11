// Copyright 2023 Manuel Wagner https://www.singinwhale.com

#pragma once
#include "TypeId.h"

namespace DI
{
	class TENTACLE_API FDependencyBindingId
	{
	public:
		FDependencyBindingId() = default;

		explicit FDependencyBindingId(FTypeId InBoundTypeId)
			: BoundTypeId(MoveTemp(InBoundTypeId)), BindingName(NAME_None)
		{
		}

		explicit FDependencyBindingId(FTypeId InBoundTypeId, FName InBindingName)
			: BoundTypeId(MoveTemp(InBoundTypeId)), BindingName(MoveTemp(InBindingName))
		{
		}

		virtual ~FDependencyBindingId() = default;

		FORCEINLINE const FTypeId& GetBoundTypeId() const
		{
			return BoundTypeId;
		}

		FORCEINLINE const FName& GetBindingName() const
		{
			return BindingName;
		}

		FORCEINLINE FString ToString() const
		{
			return FString::Printf(TEXT("%s:%s"), *BoundTypeId.GetName().ToString(), *BindingName.ToString());
		}

	private:
		FTypeId BoundTypeId = {};
		FName BindingName = NAME_None;
	};

	FORCEINLINE bool operator==(const FDependencyBindingId& A, const FDependencyBindingId& B)
	{
		return A.GetBoundTypeId() == B.GetBoundTypeId()
			&& A.GetBindingName() == B.GetBindingName();
	}

	FORCEINLINE uint32 GetTypeHash(const FDependencyBindingId& Binding)
	{
		return HashCombine(GetTypeHash(Binding.GetBoundTypeId()), GetTypeHash(Binding.GetBindingName()));
	}

	template <class T>
	static FDependencyBindingId MakeBindingId()
	{
		return FDependencyBindingId(DI::GetTypeId<T>());
	}

	template <class T>
	static FDependencyBindingId MakeBindingId(FName BindingName)
	{
		return FDependencyBindingId(DI::GetTypeId<T>(), MoveTemp(BindingName));
	}
}

// Copyright 2023 Manuel Wagner https://www.singinwhale.com

#pragma once
#include "TypeId.h"

class TENTACLE_API FDependencyBindingId
{
public:
	FDependencyBindingId() = default;

	explicit FDependencyBindingId(FTypeId InBoundTypeId)
		: BoundTypeId(InBoundTypeId)
		, BindingName(NAME_None)
	{
	}

	explicit FDependencyBindingId(FTypeId InBoundTypeId, FName InBindingName)
		: BoundTypeId(InBoundTypeId)
		, BindingName(InBindingName)
	{
	}

	virtual ~FDependencyBindingId() = default;

	FORCEINLINE FTypeId GetBoundTypeId() const
	{
		return BoundTypeId;
	}

	FORCEINLINE FName GetBindingName() const
	{
		return BindingName;
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

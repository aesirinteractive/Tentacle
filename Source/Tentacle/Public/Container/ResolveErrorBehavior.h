// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"

namespace DI
{
	class FDependencyBindingId;

	enum class EResolveErrorBehavior
	{
		ReturnNull,
		LogWarning,
		LogError,
		EnsureAlways,
		AssertCheck,
		BlueprintException,
	};

	constexpr EResolveErrorBehavior GDefaultResolveErrorBehavior = EResolveErrorBehavior::LogError;

	TENTACLE_API void HandleResolveError(const FDependencyBindingId& BindingId, EResolveErrorBehavior ErrorBehavior);
}

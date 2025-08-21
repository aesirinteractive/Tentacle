// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"

namespace DI
{
	class FBindingId;

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

	TENTACLE_API void HandleResolveError(const FBindingId& BindingId, EResolveErrorBehavior ErrorBehavior);
}

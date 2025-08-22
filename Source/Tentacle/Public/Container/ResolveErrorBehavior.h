// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"

namespace DI
{
	class FBindingId;

	enum class EResolveErrorBehavior
	{
		// Return nullptr or unset optional
		ReturnNull,
		// Log a warning to LogDependencyInjection
		LogWarning,
		// Log an error to LogDependencyInjection
		LogError,
		// Trigger an ensureAlwaysMsgf
		EnsureAlways,
		// Trigger a checkf
		AssertCheck,
		// Throw a blueprint exception
		BlueprintException,
	};

	constexpr EResolveErrorBehavior GDefaultResolveErrorBehavior = EResolveErrorBehavior::LogError;

	TENTACLE_API void HandleResolveError(const FBindingId& BindingId, EResolveErrorBehavior ErrorBehavior);
}

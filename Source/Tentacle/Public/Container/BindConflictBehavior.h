// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"

namespace DI
{
	class FDependencyBindingId;

	enum class EBindConflictBehavior
	{
		None,
		LogWarning,
		LogError,
		EnsureAlways,
		AssertCheck,
		BlueprintException,
	};

	constexpr EBindConflictBehavior GDefaultConflictBehavior = EBindConflictBehavior::LogError;

	TENTACLE_API void HandleBindingConflict(const FDependencyBindingId& BindingId, EBindConflictBehavior ConflictBehavior);
}

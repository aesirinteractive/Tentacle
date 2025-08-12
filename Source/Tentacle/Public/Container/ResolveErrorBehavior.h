// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"

namespace DI
{
	enum class EResolveErrorBehavior
	{
		ReturnNull,
		LogWarning,
		LogError,
		EnsureAlways,
		AssertCheck,
	};

	constexpr EResolveErrorBehavior GDefaultResolveErrorBehavior = EResolveErrorBehavior::LogError;
}
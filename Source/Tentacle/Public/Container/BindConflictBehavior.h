// Copyright Aesir Interactive, GmbH. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace DI
{
	enum class EBindConflictBehavior
	{
		None,
		LogWarning,
		LogError,
		Ensure,
		AssertCheck,
	};

	constexpr EBindConflictBehavior GDefaultConflictBehavior = EBindConflictBehavior::LogError;
}

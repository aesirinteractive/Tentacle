// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"

namespace DI
{
	enum class EBindResult
	{
		// The binding has been created successfully.
		Bound,

		// The binding is in conflict with an already created binding and has been rejected.
		Conflict
	};
}

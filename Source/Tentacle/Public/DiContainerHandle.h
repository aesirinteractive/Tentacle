// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiContainer.h"
#include "DiContainerHandle.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TENTACLE_API FScriptDiContainer
{
	GENERATED_BODY()

public:
	FScriptDiContainer() = default;

	// GC
	void AddStructReferencedObjects(class FReferenceCollector& Collector);
	// --

private:
	FDiContainer DiContainer;
};

template<>
struct TStructOpsTypeTraits<FScriptDiContainer> : public TStructOpsTypeTraitsBase2<FScriptDiContainer>
{
	enum
	{
		WithAddStructReferencedObjects = true,
	};
};

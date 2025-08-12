// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Contexts/DiContext.h"
#include "Contexts/DIContextInterface.h"
#include "DiContainerObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TENTACLE_API UDiContainerObject : public UObject, public IDiContextInterface
{
	GENERATED_BODY()

public:

	// GC
	static void AddReferencedObjects(UObject* Self, FReferenceCollector& Collector);
	// --
public:
	// - IDIContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return *DiContainer; };
	virtual const DI::FChainedDiContainer& GetDiContainer()  const override { return *DiContainer; };
	// --

protected:
	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

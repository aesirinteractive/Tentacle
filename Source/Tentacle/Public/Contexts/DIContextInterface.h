// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AutoInjectable.h"
#include "UObject/Interface.h"
#include "DIContextInterface.generated.h"

namespace DI
{
	class FDiContainer;
	class FChainedDiContainer;
}


UINTERFACE(NotBlueprintable, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UDiContextInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface on UObjects that act as a Dependency Injection Context.
 * This Interface cannot be implemented in blueprint because BP cannot handle shared pointers.
 * To use DI Contexts in Blueprint see UDiContainerObject and UDiContextComponent.
 */
class TENTACLE_API IDiContextInterface
{
	GENERATED_BODY()

public:
	virtual DI::FChainedDiContainer& GetDiContainer() = 0;
	virtual const DI::FChainedDiContainer& GetDiContainer() const = 0;
};

namespace DI
{
	TENTACLE_API TScriptInterface<IDiContextInterface> TryFindDiContext(UObject* StartObject);
	TENTACLE_API TScriptInterface<IDiContextInterface> TryGetLocalDiContext(UObject* StartObject);
}

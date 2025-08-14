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
 * 
 */
class TENTACLE_API IDiContextInterface
{
	GENERATED_BODY()

public:
	virtual DI::FChainedDiContainer& GetDiContainer() = 0;
	virtual const DI::FChainedDiContainer& GetDiContainer() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void RequestInitialize(const TScriptInterface<IAutoInjectable>& InitializationTarget);
};

namespace DI
{
	TENTACLE_API TScriptInterface<IDiContextInterface> TryFindDiContext(UObject* StartObject);
}

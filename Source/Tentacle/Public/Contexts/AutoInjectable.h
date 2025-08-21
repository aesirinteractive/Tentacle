// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AutoInjectable.generated.h"

class IDiContextInterface;
UINTERFACE(Blueprintable)
class UAutoInjectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface on components that require auto initialization.
 * If you are not sure if someone will call AutoInject on you already, you can use IAutoInjector to request initialization from a supporting context.
 */
class TENTACLE_API IAutoInjectable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void AutoInject(const TScriptInterface<IDiContextInterface>& DiContext);
	virtual void AutoInject_Implementation(const TScriptInterface<IDiContextInterface>& DiContext) {}
};

namespace DI
{
	TENTACLE_API bool RequestAutoInject(TScriptInterface<IAutoInjectable> AutoInjectableObject);
}
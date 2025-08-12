// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AutoInjectable.generated.h"

class IDiContextInterface;
// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UAutoInjectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TENTACLE_API IAutoInjectable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void AutoInject(const TScriptInterface<IDiContextInterface>& DiContext);
	virtual void AutoInject_Implementation(const TScriptInterface<IDiContextInterface>& DiContext) {}
};

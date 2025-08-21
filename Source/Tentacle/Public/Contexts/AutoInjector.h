// Copyright Aesir Interactive, GmbH. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AutoInjectable.h"
#include "DIContextInterface.h"
#include "UObject/Interface.h"
#include "AutoInjector.generated.h"

UINTERFACE(NotBlueprintable)
class UAutoInjector : public UDiContextInterface
{
	GENERATED_BODY()
};

/**
 * Implement this interface if you want to allow objects in this context to request initialization from here.
 * This pattern is preferred over using the DiContext directly because the context parent can decide when and how the object is initialized.
 */
class TENTACLE_API IAutoInjector : public IDiContextInterface
{
	GENERATED_BODY()

public:
	/**
	 * Request to be initialized from this context.
	 * Implementers must call IAutoInjectable::AutoInject on the initialization target as soon as possible.
	 * @param InitializationTarget The object that wants AutoInject to be called as soon as possible.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RequestInitialize(const TScriptInterface<IAutoInjectable>& InitializationTarget);
};

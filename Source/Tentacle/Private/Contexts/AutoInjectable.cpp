// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.
#include "Contexts/AutoInjectable.h"

#include "Tentacle.h"
#include "Contexts/AutoInjector.h"
#include "Contexts/DIContextInterface.h"

bool DI::RequestAutoInject(TScriptInterface<IAutoInjectable> AutoInjectableObject)
{
	if (!AutoInjectableObject.GetObject())
		return false;

	TScriptInterface<IDiContextInterface> DiContextInterface = DI::TryFindDiContext(AutoInjectableObject.GetObject());

	if (!DiContextInterface)
	{
		UE_LOG(LogDependencyInjection, Error, TEXT("Could not find DiContext for Injectable %s"), *AutoInjectableObject.GetObject()->GetName());
		return false;
	}

	IAutoInjector* AutoInjector = Cast<IAutoInjector>(DiContextInterface.GetObject());
	if (!AutoInjector)
	{
		UE_LOG(LogDependencyInjection, Error, TEXT("DiContext for Injectable %s is not an AutoInjector"), *AutoInjectableObject.GetObject()->GetName());
		return false;
	}

	AutoInjector->RequestInitialize(AutoInjectableObject);
	return true;
}

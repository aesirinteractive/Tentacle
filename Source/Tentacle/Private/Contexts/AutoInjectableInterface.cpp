// Copyright singinwhale https://www.singinwhale.com and contributors. Distributed under the MIT license.
#include "Contexts/AutoInjectableInterface.h"

#include "Tentacle.h"
#include "Contexts/AutoInjector.h"
#include "Contexts/DIContextInterface.h"

bool DI::RequestAutoInject(TScriptInterface<IAutoInjectableInterface> AutoInjectableObject)
{
	if (!AutoInjectableObject.GetObject())
		return false;

	if (IAutoInjector* ImplementingOuter = AutoInjectableObject.GetObject()->GetImplementingOuter<IAutoInjector>())
	{
		ImplementingOuter->RequestInitialize(AutoInjectableObject);
		return true;
	}

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

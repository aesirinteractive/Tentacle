// Copyright Aesir Interactive, GmbH. All Rights Reserved.


#include "Contexts/AutoInjector.h"


void IAutoInjector::RequestInitialize(const TScriptInterface<IAutoInjectable>& InitializationTarget)
{
	if (ensureAlwaysMsgf(InitializationTarget.GetObject() && InitializationTarget.GetObject()->Implements<UAutoInjectable>(), TEXT("Invalid Target")))
	{
		TScriptInterface<IDiContextInterface> DiContext = CastChecked<UObject>(this);
		IAutoInjectable::Execute_AutoInject(InitializationTarget.GetObject(), DiContext);
	}
}

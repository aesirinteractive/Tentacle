// Copyright singinwhale https://www.singinwhale.com and contributors. Distributed under the MIT license.


#include "ExampleActor.h"

void AExampleActor::ComponentRegistered(UActorComponent* Component)
{
	// uncomment if you have the engine mod
	// Super::ComponentRegistered(Component); 
	
	if (IAutoInjectableInterface* AutoInjectable = Cast<IAutoInjectableInterface>(Component))
	{
		IAutoInjectableInterface::Execute_AutoInject(Component, this);
	}
}

DI::FChainedDiContainer& AExampleActor::GetDiContainer()
{
	return *DiContainer;
}

const DI::FChainedDiContainer& AExampleActor::GetDiContainer() const
{
	return *DiContainer;
}

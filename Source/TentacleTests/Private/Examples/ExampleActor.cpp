// Copyright singinwhale https://www.singinwhale.com and contributors. Distributed under the MIT license.


#include "ExampleActor.h"


AExampleActor::AExampleActor()
{
}

// Called when the game starts or when spawned
void AExampleActor::BeginPlay()
{
	Super::BeginPlay();

}

void AExampleActor::ComponentRegistered(UActorComponent* Component)
{
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

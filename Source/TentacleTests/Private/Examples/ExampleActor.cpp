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
	if (IAutoInjectable* AutoInjectable = Cast<IAutoInjectable>(Component))
	{
		IAutoInjectable::Execute_AutoInject(Component, this);
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

// Copyright Manuel Wagner https://www.singinwhale.com


#include "ExampleActor.h"


AExampleActor::AExampleActor()
{
}

// Called when the game starts or when spawned
void AExampleActor::BeginPlay()
{
	Super::BeginPlay();

}

DI::FChainedDiContainer& AExampleActor::GetDiContainer()
{
	return *DiContainer;
}

const DI::FChainedDiContainer& AExampleActor::GetDiContainer() const
{
	return *DiContainer;
}

// Copyright Manuel Wagner https://www.singinwhale.com


#include "ExampleComponent.h"

#include "Container/ChainedDiContainer.h"
#include "Contexts/DIContextInterface.h"
#include "Mocks/SimpleService.h"


void UExampleComponent::AutoInject_Implementation(const TScriptInterface<IDiContextInterface>& DiContext)
{
	// Here are mulitple examples of how to resolve dependencies, just pick one

	DiContext->GetDiContainer().Inject().IntoFunctionWithNames(*this, &UExampleComponent::InjectDependencies, "SimpleService");
	DiContext->GetDiContainer().Bind().BindInstance<UExampleComponent>(this, DI::EBindConflictBehavior::AssertCheck);


	DiContext->GetDiContainer().Inject().AsyncIntoFunctionByType(*this, &UExampleComponent::InjectDependencies);
	DiContext->GetDiContainer().Resolve().TryResolveFutureTypeInstances<USimpleUService, UExampleComponent>()
	         .ExpandNext(
		         [this, DiContainer = DiContext->GetDiContainer().AsShared()](TOptional<TObjectPtr<USimpleUService>> Service, TOptional<TObjectPtr<UExampleComponent>>)
		         {
		         	this->InjectDependencies(*Service);
					DiContainer->Bind().BindInstance<UExampleComponent>(this, DI::EBindConflictBehavior::AssertCheck);
		         }
	         );

}

void UExampleComponent::InjectDependencies(TObjectPtr<USimpleUService> InSimpleUService)
{
	UE_LOG(LogTemp, Log, TEXT("Injected: %s"), *InSimpleUService->GetName());
	SimpleUService = InSimpleUService;
}

void UExampleComponent::InjectDependenciesWithExtraArgs(TObjectPtr<USimpleUService> InSimpleUService, FString InExtraString)
{
	SimpleUService = InSimpleUService;
	ExtraString = InExtraString;
}

void UExampleComponent::BeginPlay()
{
	Super::BeginPlay();

	DI::RequestAutoInject(this);
}

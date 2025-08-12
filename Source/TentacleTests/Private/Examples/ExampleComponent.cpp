// Copyright Manuel Wagner https://www.singinwhale.com


#include "ExampleComponent.h"

#include "Container/ChainedDiContainer.h"
#include "Contexts/DIContextInterface.h"
#include "Mocks/SimpleService.h"



void UExampleComponent::AutoInject_Implementation(const TScriptInterface<IDiContextInterface>& DiContext)
{
	DiContext->GetDiContainer().Inject().IntoFunctionByType(*this, &UExampleComponent::InjectDependencies);
	DiContext->GetDiContainer().Bind().BindInstance<UExampleComponent>(this, DI::EBindConflictBehavior::AssertCheck);
}

void UExampleComponent::InjectDependencies(TObjectPtr<USimpleUService> SimpleUService)
{
	UE_LOG(LogTemp, Log, TEXT("Injected: %s"), *SimpleUService->GetName());
}


void UExampleComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TScriptInterface<IDiContextInterface> DiContext = DI::TryFindDiContext(this))
	{
		DiContext->RequestInitialize(this);
	}
}
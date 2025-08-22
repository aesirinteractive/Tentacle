// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "Container/ChainedDiContainer.h"
#include "Contexts/AutoInjector.h"
#include "Contexts/DIContextInterface.h"
#include "GameFramework/Actor.h"
#include "ExampleActor.generated.h"

UCLASS(HideDropdown, NotBlueprintable)
class TENTACLETESTS_API AExampleActor : public AActor, public IAutoInjector, public IDiContextInterface
{
	GENERATED_BODY()

public:
	AExampleActor();

	// - AActor
	virtual void BeginPlay() override;

	// ReSharper disable once CppEnforceOverridingFunctionStyle
	virtual void ComponentRegistered(UActorComponent* Component);

	// - IDiContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override;
	virtual const DI::FChainedDiContainer& GetDiContainer() const override;
	// --

	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

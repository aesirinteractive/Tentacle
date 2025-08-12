// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "Container/ChainedDiContainer.h"
#include "Contexts/DIContextInterface.h"
#include "GameFramework/Actor.h"
#include "ExampleActor.generated.h"

UCLASS()
class TENTACLETESTS_API AExampleActor : public AActor, public IDiContextInterface
{
	GENERATED_BODY()

public:
	AExampleActor();

	// - AActor
	virtual void BeginPlay() override;
	// - IDiContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override;
	virtual const DI::FChainedDiContainer& GetDiContainer() const override;
	// --

	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

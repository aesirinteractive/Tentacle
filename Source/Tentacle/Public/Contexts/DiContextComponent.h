// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DIContextInterface.h"
#include "Components/ActorComponent.h"
#include "Container/ChainedDiContainer.h"

#include "DiContextComponent.generated.h"

/**
 * Component to put DiContext functionality on blueprint created actors that cannot implement IDiContextInterface themselves.
 * @see DI::TryFindDiContext(UObject*)
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), NotBlueprintable)
class TENTACLE_API UDiContextComponent : public UActorComponent, public IDiContextInterface
{
	GENERATED_BODY()

public:
	UDiContextComponent();

	virtual void InitializeComponent() override;

public:
	void SetAsParentOnAllComponentsOf(AActor& Actor) const;

	static void AddReferencedObjects(UObject* Self, FReferenceCollector& Collector);

	
public:
	// - IDIContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return *DiContainer; }
	virtual const DI::FChainedDiContainer& GetDiContainer()  const override { return *DiContainer; }
	// --

protected:
	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

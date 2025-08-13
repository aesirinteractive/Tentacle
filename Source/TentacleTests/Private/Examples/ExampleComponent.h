// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Contexts/AutoInjectable.h"
#include "ExampleComponent.generated.h"


class USimpleUService;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TENTACLETESTS_API UExampleComponent : public UActorComponent, public IAutoInjectable
{
	GENERATED_BODY()

public:
	// - AActor
	virtual void BeginPlay() override;
	// - IAutoInjector
	virtual void AutoInject_Implementation(const TScriptInterface<IDiContextInterface>& DiContext) override;
	// --

	void InjectDependencies(TObjectPtr<USimpleUService> SimpleUService);
	void InjectDependenciesWithExtraArgs(TObjectPtr<USimpleUService> InSimpleUService, FString ExtraString);

	UPROPERTY()
	TObjectPtr<USimpleUService> SimpleUService;

	FString ExtraString;
};

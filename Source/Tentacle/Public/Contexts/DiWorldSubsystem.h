// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Container/ChainedDiContainer.h"
#include "DIContextInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "DiWorldSubsystem.generated.h"

/**
 * Di Context for the world.
 * Requires UTentacleSettings::bEnableScopeSubsystems to be enabled.
 * Worlds chain to the game instance.
 * This context should be used for binding dependencies that are tied to a world, like Mass, GameServices, GameMode and GameState.
 */
UCLASS()
class TENTACLE_API UDiWorldSubsystem : public UWorldSubsystem, public IDiContextInterface
{
	GENERATED_BODY()

public:
	static UDiWorldSubsystem* TryGet(const UObject* WorldContext);

	// - USubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// --
public:
	// - IDIContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return *DiContainer; };
	virtual const DI::FChainedDiContainer& GetDiContainer() const override { return *DiContainer; };
	// --

protected:
	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

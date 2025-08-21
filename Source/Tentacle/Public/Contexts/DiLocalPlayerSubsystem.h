// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Container/ChainedDiContainer.h"
#include "DIContextInterface.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "DiLocalPlayerSubsystem.generated.h"

/**
 * Di Context for Local Players.
 * Requires UTentacleSettings::bEnableScopeSubsystems to be enabled.
 * LocalPlayers chain to the game instance.
 * This context should be used for binding dependencies that are tied to a local player like player input, viewport clients etc.
 */
UCLASS()
class TENTACLE_API UDiLocalPlayerSubsystem : public ULocalPlayerSubsystem, public IDiContextInterface
{
	GENERATED_BODY()

public:
	// - USubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// - ULocalPlayerSubsystem
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;
	// --

	// - IDIContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return *DiContainer; };
	virtual const DI::FChainedDiContainer& GetDiContainer()  const override { return *DiContainer; };
	// --

protected:
	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

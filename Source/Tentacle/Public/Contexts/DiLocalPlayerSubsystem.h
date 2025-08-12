// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContext.h"
#include "DIContextInterface.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "DiLocalPlayerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TENTACLE_API UDiLocalPlayerSubsystem : public ULocalPlayerSubsystem, public IDiContextInterface
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;

	
public:
	// - IDIContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return *DiContainer; };
	virtual const DI::FChainedDiContainer& GetDiContainer()  const override { return *DiContainer; };
	// --

protected:
	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

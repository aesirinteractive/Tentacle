// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContext.h"
#include "DIContextInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DiGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TENTACLE_API UDiGameInstanceSubsystem : public UGameInstanceSubsystem, public IDiContextInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	void TrySetChainParentInWorldSubsystem(const UWorld* World) const;

private:
	void HandleWorldAdded(UWorld* World) const;
	void HandleWorldDestroyed(UWorld* World) const;	
public:
	// - IDIContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return *DiContainer; };
	virtual const DI::FChainedDiContainer& GetDiContainer()  const override { return *DiContainer; };
	// --

protected:
	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

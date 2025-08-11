// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContext.h"
#include "DIContextInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "DiWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TENTACLE_API UDiWorldSubsystem : public UWorldSubsystem, public IDIContextInterface
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	// - IDIContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return *DiContainer; };
	virtual const DI::FChainedDiContainer& GetDiContainer() const override { return *DiContainer; };
	// --

protected:
	TSharedRef<DI::FChainedDiContainer> DiContainer = MakeShared<DI::FChainedDiContainer>();
};

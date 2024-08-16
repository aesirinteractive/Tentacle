// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContext.h"
#include "Subsystems/WorldSubsystem.h"
#include "DiWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TENTACLE_API UDiWorldSubsystem : public UWorldSubsystem, public FDiContext
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};

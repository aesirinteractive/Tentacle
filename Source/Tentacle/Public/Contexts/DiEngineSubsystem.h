// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContext.h"
#include "Subsystems/EngineSubsystem.h"
#include "DiEngineSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TENTACLE_API UDiEngineSubsystem : public UEngineSubsystem, public FDiContext
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void TrySetDiParentInGameInstance(const UGameInstance& GameInstance) const;
	void HandleWorldAdded(UWorld* World) const;
};

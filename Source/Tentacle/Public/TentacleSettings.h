// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TentacleSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game)
class TENTACLE_API UTentacleSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * Set to true to allow the auto-creation of scope subsystems
	 * (GameInstance-, World-, LocalPlayer-, Engine-Subsystem)
	 */
	UPROPERTY(EditAnywhere, Config)
	bool bEnableScopeSubsystems = false;
	
	UPROPERTY(EditAnywhere, Config, meta = (EditCondition = bEnableScopeSubsystems))
	bool bEnableDefaultChaining = false;
};

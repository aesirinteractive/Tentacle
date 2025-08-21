// Fill out your copyright notice in the Description page of Project Settings.


#include "Contexts/DiEngineSubsystem.h"

#include "TentacleSettings.h"
#include "Contexts/DiGameInstanceSubsystem.h"

bool UDiEngineSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return GetDefault<UTentacleSettings>()->bEnableScopeSubsystems;
}

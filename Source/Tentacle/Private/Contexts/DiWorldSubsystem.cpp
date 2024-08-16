// Fill out your copyright notice in the Description page of Project Settings.


#include "Contexts/DiWorldSubsystem.h"

#include "TentacleSettings.h"

bool UDiWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return GetDefault<UTentacleSettings>()->bEnableScopeSubsystems;
}

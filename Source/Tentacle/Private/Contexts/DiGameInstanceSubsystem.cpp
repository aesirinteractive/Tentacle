// Fill out your copyright notice in the Description page of Project Settings.


#include "Contexts/DiGameInstanceSubsystem.h"

#include "TentacleSettings.h"
#include "Contexts/DiEngineSubsystem.h"
#include "Contexts/DiWorldSubsystem.h"

void UDiGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!GetDefault<UTentacleSettings>()->bEnableDefaultChaining)
		return;

	if (!GEngine)
		return;

	UDiEngineSubsystem* DiEngineSubsystem = GEngine->GetEngineSubsystem<UDiEngineSubsystem>();
	DiContainer->SetParentContainer(DiEngineSubsystem->GetDiContainer().AsShared());
}

bool UDiGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return GetDefault<UTentacleSettings>()->bEnableScopeSubsystems;
}

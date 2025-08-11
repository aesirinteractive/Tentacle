// Fill out your copyright notice in the Description page of Project Settings.


#include "Contexts/DiEngineSubsystem.h"

#include "TentacleSettings.h"
#include "Contexts/DiGameInstanceSubsystem.h"

bool UDiEngineSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return GetDefault<UTentacleSettings>()->bEnableScopeSubsystems;
}

void UDiEngineSubsystem::TrySetDiParentInGameInstance(const UGameInstance& GameInstance) const
{
	if (UDiGameInstanceSubsystem* DiGameInstanceSubsystem = GameInstance.GetSubsystem<UDiGameInstanceSubsystem>())
	{
		DiGameInstanceSubsystem->GetDiContainer().SetParentContainer(DiContainer.ToWeakPtr());
	}
}

void UDiEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!GetDefault<UTentacleSettings>()->bEnableDefaultChaining)
		return;

	GEngine->OnWorldAdded().AddUObject(this, &ThisClass::HandleWorldAdded);

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		TObjectPtr<UGameInstance> GameInstance = WorldContext.OwningGameInstance;
		if (!GameInstance)
			continue;
		
		TrySetDiParentInGameInstance(*GameInstance);
	}
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UDiEngineSubsystem::HandleWorldAdded(UWorld* World) const
{
	if (const UGameInstance* GameInstance = World->GetGameInstance())
	{
		TrySetDiParentInGameInstance(*GameInstance);
	}
}

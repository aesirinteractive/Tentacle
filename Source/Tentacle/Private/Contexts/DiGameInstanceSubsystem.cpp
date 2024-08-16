// Fill out your copyright notice in the Description page of Project Settings.


#include "Contexts/DiGameInstanceSubsystem.h"

#include "TentacleSettings.h"
#include "Contexts/DiWorldSubsystem.h"

void UDiGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!GetDefault<UTentacleSettings>()->bEnableDefaultChaining)
		return;

	TrySetChainParentInWorldSubsystem(GetGameInstance()->GetWorld());
}

bool UDiGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return GetDefault<UTentacleSettings>()->bEnableScopeSubsystems;
}

void UDiGameInstanceSubsystem::TrySetChainParentInWorldSubsystem(const UWorld* World) const
{
	if (!IsValid(World))
		return;

	UDiWorldSubsystem* DiWorldSubsystem = World->GetSubsystem<UDiWorldSubsystem>();
	if (!DiWorldSubsystem)
		return;

	DiWorldSubsystem->GetDiContainer().SetParentContainer(MyDiContainer.ToWeakPtr());
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UDiGameInstanceSubsystem::HandleWorldAdded(UWorld* World) const
{
	TrySetChainParentInWorldSubsystem(World);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UDiGameInstanceSubsystem::HandleWorldDestroyed(UWorld* World) const
{
	UDiWorldSubsystem* DiWorldSubsystem = World->GetSubsystem<UDiWorldSubsystem>();
	if (!DiWorldSubsystem)
		return;

	DiWorldSubsystem->GetDiContainer().SetParentContainer(MyDiContainer.ToWeakPtr());
}

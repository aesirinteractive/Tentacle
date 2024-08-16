// Fill out your copyright notice in the Description page of Project Settings.


#include "Contexts/DiLocalPlayerSubsystem.h"

#include "TentacleSettings.h"

bool UDiLocalPlayerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return GetDefault<UTentacleSettings>()->bEnableScopeSubsystems;
}

void UDiLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	PlayerControllerChanged(LocalPlayer->GetPlayerController(nullptr));
}

void UDiLocalPlayerSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);

	if (!GetDefault<UTentacleSettings>()->bEnableDefaultChaining)
		return;
	//TODO player controller must multiplex world and local player
}

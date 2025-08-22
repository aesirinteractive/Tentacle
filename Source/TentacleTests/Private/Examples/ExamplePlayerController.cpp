// Copyright Aesir Interactive, GmbH. All Rights Reserved.


#include "ExamplePlayerController.h"

#include "Contexts/DiLocalPlayerSubsystem.h"
#include "Contexts/DiWorldSubsystem.h"

AExamplePlayerController::AExamplePlayerController()
{
	DiContextComponent = CreateDefaultSubobject<UDiContextComponent>("DiContextComponent");
}

void AExamplePlayerController::SetPlayer(UPlayer* InPlayer)
{
	if (class ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UDiLocalPlayerSubsystem* DiLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UDiLocalPlayerSubsystem>())
		{
			TSharedRef<DI::FChainedDiContainer> LocalPlayerContainer = DiLocalPlayerSubsystem->GetDiContainer().AsShared();
			ForkingDiContainer->RemoveParentContainer(LocalPlayerContainer);
		}
	}

	Super::SetPlayer(InPlayer);

	if (class ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UDiLocalPlayerSubsystem* DiLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UDiLocalPlayerSubsystem>())
		{
			TSharedRef<DI::FChainedDiContainer> LocalPlayerContainer = DiLocalPlayerSubsystem->GetDiContainer().AsShared();
			ForkingDiContainer->AddParentContainer(LocalPlayerContainer, 1);
		}
	}
}

void AExamplePlayerController::SetPawn(APawn* InPawn)
{
	if (!GetDefault<UTentacleSettings>()->bEnableDefaultChaining)

	if (TScriptInterface<IDiContextInterface> PawnDiContext = DI::TryGetLocalDiContext(GetPawn()))
	{
		PawnDiContext->GetDiContainer().SetParentContainer(nullptr);
	}

	Super::SetPawn(InPawn);

	if (TScriptInterface<IDiContextInterface> PawnDiContext = DI::TryGetLocalDiContext(GetPawn()))
	{
		PawnDiContext->GetDiContainer().SetParentContainer(GetDiContainer().AsShared());
	}
}

void AExamplePlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (UDiWorldSubsystem* DiWorldSubsystem = UDiWorldSubsystem::TryGet(this))
	{
		ForkingDiContainer->AddParentContainer(DiWorldSubsystem->GetDiContainer().AsShared(), 0);
	}

	DiContextComponent->SetAsParentOnAllComponentsOf(this);
}

void AExamplePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DiContextComponent->GetDiContainer().SetParentContainer(ForkingDiContainer);
}

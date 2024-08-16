// Fill out your copyright notice in the Description page of Project Settings.


#include "Contexts/DiContextComponent.h"

#include "Contexts/DiWorldSubsystem.h"

UDiContextComponent::UDiContextComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UDiContextComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (UWorld* World = GetWorld())
	{
		if (UDiWorldSubsystem* DiWorldSubsystem = World->GetSubsystem<UDiWorldSubsystem>())
		{
			GetDiContainer().SetParentContainer(DiWorldSubsystem->GetDiContainer().AsWeak());
		}
	}
}

void UDiContextComponent::SetAsParentOnAllComponentsOf(AActor& Actor) const
{
	Actor.ForEachComponent(false, [this](UActorComponent* ActorComponent)
	{
		if (ActorComponent == this)
			return;

		if (IDiContext* DiContext = Cast<IDiContext>(ActorComponent))
		{
			DiContext->GetDiContainer()->SetParentContainer(MyDiContainer);
		}
	});
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContext.h"
#include "Components/ActorComponent.h"

#include "DiContextComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TENTACLE_API UDiContextComponent : public UActorComponent, public FDiContext
{
	GENERATED_BODY()

public:
	UDiContextComponent();

	virtual void InitializeComponent() override;
public:
	void SetAsParentOnAllComponentsOf(AActor& Actor) const;
};

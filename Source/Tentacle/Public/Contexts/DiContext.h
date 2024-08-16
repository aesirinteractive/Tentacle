// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChainedDiContainer.h"
#include "UObject/Interface.h"
#include "DiContext.generated.h"

UINTERFACE()
class UDiContext : public UInterface
{
	GENERATED_BODY()
};


class TENTACLE_API IDiContext
{
	GENERATED_BODY()

public:
	virtual TSharedRef<Tentacle::FChainedDiContainer> GetDiContainer() = 0;
};

class TENTACLE_API FDiContext
{
public:
	FDiContext():
		MyDiContainer(MakeShared<Tentacle::FChainedDiContainer>())
	{
	}

	Tentacle::FChainedDiContainer& GetDiContainer() { return *MyDiContainer; };
	const Tentacle::FChainedDiContainer& GetDiContainer() const { return *MyDiContainer; };

protected:
	TSharedRef<Tentacle::FChainedDiContainer> MyDiContainer;
};

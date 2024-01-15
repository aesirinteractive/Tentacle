// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiContainerHandle.h"
#include "UObject/Interface.h"
#include "DIContextInterface.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class UScriptDIContextInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TENTACLE_API IScriptDIContextInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	bool GetDiContainer(FScriptDiContainer& OutScriptDiContainer);
};


UINTERFACE(NotBlueprintable, BlueprintType)
class UDIContextInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TENTACLE_API IDIContextInterface
{
	GENERATED_BODY()

public:
	virtual const FDiContainer& GetDiContainer() const = 0;
};

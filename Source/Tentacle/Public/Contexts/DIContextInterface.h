// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DIContextInterface.generated.h"

namespace DI
{
	class FDiContainer;
	class FChainedDiContainer;
}


UINTERFACE(NotBlueprintable, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
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
	virtual DI::FChainedDiContainer& GetDiContainer() = 0;
	virtual const DI::FChainedDiContainer& GetDiContainer() const = 0;
};

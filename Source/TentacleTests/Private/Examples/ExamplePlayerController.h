// Copyright Aesir Interactive, GmbH. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Container/ChainedDiContainer.h"
#include "Container/ForkingDiContainer.h"
#include "Contexts/DiContextComponent.h"
#include "Contexts/DIContextInterface.h"
#include "GameFramework/PlayerController.h"
#include "ExamplePlayerController.generated.h"

class UDiContextComponent;

/**
 * Example player controller that demonstrates a split parent relationship
 */
UCLASS(HideDropdown, NotBlueprintable)
class TENTACLETESTS_API AExamplePlayerController : public APlayerController, public IDiContextInterface
{
	GENERATED_BODY()

public:
	AExamplePlayerController();

	// - IDiContextInterface
	virtual DI::FChainedDiContainer& GetDiContainer() override { return DiContextComponent->GetDiContainer(); }
	virtual const DI::FChainedDiContainer& GetDiContainer() const override { return DiContextComponent->GetDiContainer(); }
	// - APlayerController
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void SetPawn(APawn* InPawn) override;
	// - AActor
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	// --
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDiContextComponent> DiContextComponent;
private:

	TSharedRef<DI::FForkingDiContainer> ForkingDiContainer = MakeShared<DI::FForkingDiContainer>();
};

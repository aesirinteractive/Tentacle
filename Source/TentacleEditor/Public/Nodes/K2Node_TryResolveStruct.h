
#include "CoreMinimal.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CallFunction.h"
#include "K2Node_TryResolveStruct.generated.h"


UCLASS()
class TENTACLEEDITOR_API UK2Node_TryResolveStruct : public UK2Node_CallFunction
{
	GENERATED_BODY()

	// UK2Node
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	// --
};

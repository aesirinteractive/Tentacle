// Copyright Manuel Wagner https://www.singinwhale.com
#include "Nodes/K2Node_TryResolveStruct.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Contexts/DiBlueprintFunctionLibrary.h"
#include "Kismet/BlueprintInstancedStructLibrary.h"


#define LOCTEXT_NAMESPACE "TentacleEditor"

void UK2Node_TryResolveStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);
	UClass* Action = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		auto CustomizeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, const FName FunctionName)
		{
			UK2Node_TryResolveStruct* Node = CastChecked<UK2Node_TryResolveStruct>(NewNode);
			UFunction* Function = UDiBlueprintFunctionLibrary::StaticClass()->FindFunctionByName(FunctionName);
			check(Function);
			Node->SetFromFunction(Function);
		};
		
		// TryResolveStruct()
		UBlueprintNodeSpawner* TryResolveStructSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(TryResolveStructSpawner != nullptr);
		TryResolveStructSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UDiBlueprintFunctionLibrary, TryResolveStruct));
		ActionRegistrar.AddBlueprintAction(Action, TryResolveStructSpawner);

		// TryResolveStructCopy()
		UBlueprintNodeSpawner* TryResolveStructCopySpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(TryResolveStructCopySpawner != nullptr);
		TryResolveStructCopySpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UDiBlueprintFunctionLibrary, TryResolveStructCopy));
		ActionRegistrar.AddBlueprintAction(Action, TryResolveStructCopySpawner);
	}
}

bool UK2Node_TryResolveStruct::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	const UEdGraphPin* ValuePin = FindPinChecked(FName(TEXT("OutStructData")));

	if (MyPin == ValuePin && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	{
		if (OtherPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
		{
			OutReason = TEXT("OutStructData must be a struct.");
			return true;
		}
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
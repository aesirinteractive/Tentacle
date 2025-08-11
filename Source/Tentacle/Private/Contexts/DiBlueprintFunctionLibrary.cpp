// Copyright Aesir Interactive, GmbH. All Rights Reserved.


#include "Contexts/DiBlueprintFunctionLibrary.h"

#include "Blueprint/BlueprintExceptionInfo.h"
#include "Container/ChainedDiContainer.h"
#include "Contexts/DIContextInterface.h"


UObject* UDiBlueprintFunctionLibrary::TryResolveObject(TScriptInterface<IDIContextInterface> DiContextInterface,
                                                       UClass* ObjectType,
                                                       FName BindingName)
{
	return DiContextInterface->GetDiContainer().Resolve().TryResolveUObjectByClass(ObjectType, BindingName);
}

bool UDiBlueprintFunctionLibrary::TryResolveStruct(
	TScriptInterface<IDIContextInterface> DiContextInterface,
	FName BindingName,
	int32& OutStructData,
	EStructUtilsResult& Result)
{
	checkNoEntry();
	return false;
}

void UDiBlueprintFunctionLibrary::BindObject(
	TScriptInterface<IDIContextInterface> DiContextInterface,
	UObject* Object,
	FName BindingName)
{
	BindObjectAsType(DiContextInterface, Object, Object->GetClass(), BindingName);
}

void UDiBlueprintFunctionLibrary::BindObjectAsType(
	TScriptInterface<IDIContextInterface> DiContextInterface,
	UObject* Object,
	UClass* ObjectBindingType,
	FName BindingName)
{
	FFrame* Stack = FFrame::GetThreadLocalTopStackFrame();
	if (!DiContextInterface)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			INVTEXT("DiContextInterface is invalid")
		);

		FBlueprintCoreDelegates::ThrowScriptException(Stack->Object, *Stack, ExceptionInfo);
	}

	DI::FChainedDiContainer DiContainer = DiContextInterface->GetDiContainer();
	TSharedRef<DI::TUObjectDependencyBinding<UObject>> UObjectDependencyBinding = MakeShared<DI::TUObjectDependencyBinding<UObject>>(
		DI::FDependencyBindingId(FTypeId(ObjectBindingType), BindingName),
		Object
	);
	DI::EBindResult Result = DiContainer.BindSpecific(UObjectDependencyBinding, DI::EBindConflictBehavior::LogError);
	switch (Result)
	{
	case DI::EBindResult::Bound:
		return;
	case DI::EBindResult::Conflict:
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::FatalError,
				INVTEXT("Binding conflict for binding %s")
			);

			FBlueprintCoreDelegates::ThrowScriptException(Stack->Object, *Stack, ExceptionInfo);
			break;
		}
	}
}

DEFINE_FUNCTION(UDiBlueprintFunctionLibrary::execTryResolveStruct)
{
	P_GET_TINTERFACE(IDIContextInterface, DiContextInterface);
	P_GET_PROPERTY(FNameProperty, BindingName);

	// Read wildcard Value input.
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);

	const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_GET_ENUM_REF(EStructUtilsResult, Result);

	P_FINISH;

	if (!ValueProp || !ValuePtr || !DiContextInterface)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			INVTEXT("Failed to resolve the Value for Struct")
		);

		FBlueprintCoreDelegates::ThrowScriptException(Stack.Object, Stack, ExceptionInfo);

		P_NATIVE_BEGIN;
			Result = EStructUtilsResult::NotValid;
			(*static_cast<bool*>(RESULT_PARAM)) = false;
		P_NATIVE_END;
	}
	else
	{
		P_NATIVE_BEGIN;
			bool bResult = DiContextInterface->GetDiContainer().Resolve().TryResolveUStruct(
				ValueProp->Struct,
				ValuePtr,
				BindingName
			);
			Result = bResult ? EStructUtilsResult::Valid : EStructUtilsResult::NotValid;
			(*static_cast<bool*>(RESULT_PARAM)) = bResult;
		P_NATIVE_END;
	}
}

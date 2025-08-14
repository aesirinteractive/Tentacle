// Copyright Manuel Wagner https://www.singinwhale.com


#include "Contexts/DiBlueprintFunctionLibrary.h"

#include "Blueprint/BlueprintExceptionInfo.h"
#include "Container/ChainedDiContainer.h"
#include "Contexts/DIContextInterface.h"


bool UDiBlueprintFunctionLibrary::RequestAutoInject(TScriptInterface<IAutoInjectable> AutoInjectableObject, bool& bResult)
{
	if (!AutoInjectableObject.GetObject())
	{
		if (FFrame* StackFrame = FFrame::GetThreadLocalTopStackFrame())
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				INVTEXT("Accessed None attempting to call RequestAutoInject.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(StackFrame->Object, *StackFrame, ExceptionInfo);
		}
		return false;
	}

	bResult = DI::RequestAutoInject(AutoInjectableObject);
	return bResult;
}

UObject* UDiBlueprintFunctionLibrary::TryResolveObject(TScriptInterface<IDiContextInterface> DiContextInterface, UClass* ObjectType, FName BindingName)
{
	return DiContextInterface->GetDiContainer().Resolve().TryResolveUObjectByClass(ObjectType, BindingName, DI::EResolveErrorBehavior::LogError);
}

bool UDiBlueprintFunctionLibrary::TryResolveStruct(
	TScriptInterface<IDiContextInterface> DiContextInterface,
	FName BindingName,
	int32& OutStructData,
	EStructUtilsResult& Result)
{
	checkNoEntry();
	return false;
}

bool UDiBlueprintFunctionLibrary::TryResolveStructCopy(
	TScriptInterface<IDiContextInterface> DiContextInterface,
	UScriptStruct* StructType,
	FName BindingName,
	int32& OutStructData,
	EStructUtilsResult& Result)
{
	checkNoEntry();
	return false;
}

void UDiBlueprintFunctionLibrary::BindObject(
	TScriptInterface<IDiContextInterface> DiContextInterface,
	UObject* Object,
	FName BindingName)
{
	if (!DiContextInterface)
	{
		if (FFrame* StackFrame = FFrame::GetThreadLocalTopStackFrame())
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				INVTEXT("Accessed None DiContextInterface attempting to call BindObject.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(StackFrame->Object, *StackFrame, ExceptionInfo);
		}
		return;
	}
	BindObjectAsType(DiContextInterface, Object, Object->GetClass(), BindingName);
}

void UDiBlueprintFunctionLibrary::BindStruct(TScriptInterface<IDiContextInterface> DiContextInterface, FName BindingName, int32 StructData)
{
	checkNoEntry();
}

void UDiBlueprintFunctionLibrary::BindObjectAsType(
	TScriptInterface<IDiContextInterface> DiContextInterface,
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
		return;
	}

	DI::FChainedDiContainer& DiContainer = DiContextInterface->GetDiContainer();
	TSharedRef<DI::TUObjectDependencyBinding<UObject>> UObjectDependencyBinding = MakeShared<DI::TUObjectDependencyBinding<UObject>>(
		DI::FDependencyBindingId(FTypeId(ObjectBindingType), BindingName),
		Object
	);
	DI::EBindResult Result = DiContainer.BindSpecific(UObjectDependencyBinding, DI::EBindConflictBehavior::BlueprintException);
}

DEFINE_FUNCTION(UDiBlueprintFunctionLibrary::execTryResolveStruct)
{
	P_GET_TINTERFACE(IDiContextInterface, DiContextInterface);
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

DEFINE_FUNCTION(UDiBlueprintFunctionLibrary::execTryResolveStructCopy)
{
	P_GET_TINTERFACE(IDiContextInterface, DiContextInterface);
	P_GET_OBJECTPTR(UScriptStruct, StructType);
	P_GET_PROPERTY(FNameProperty, BindingName);

	// Read wildcard Value input.
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);

	const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_GET_ENUM_REF(EStructUtilsResult, Result);

	P_FINISH;

	if (!ValueProp || !ValuePtr || !DiContextInterface || !StructType)
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
			bool bResult = DiContextInterface->GetDiContainer()
			                                 .Resolve()
			                                 .TryResolveUStruct(
				                                 StructType,
				                                 ValuePtr,
				                                 BindingName
			                                 );
			Result = bResult ? EStructUtilsResult::Valid : EStructUtilsResult::NotValid;
			(*static_cast<bool*>(RESULT_PARAM)) = bResult;
		P_NATIVE_END;
	}
}

DEFINE_FUNCTION(UDiBlueprintFunctionLibrary::execBindStruct)
{
	P_GET_TINTERFACE(IDiContextInterface, DiContextInterface);
	P_GET_PROPERTY(FNameProperty, BindingName);

	// Read wildcard Value input.
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);

	const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr || !DiContextInterface)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			INVTEXT("Failed to resolve the Value for Struct")
		);

		FBlueprintCoreDelegates::ThrowScriptException(Stack.Object, Stack, ExceptionInfo);
	}
	else
	{
		P_NATIVE_BEGIN;
		{
			TSharedRef<DI::FRawDataBinding> StructDataBinding = MakeShared<DI::FUStructBinding>(ValueProp->Struct, BindingName, ValuePtr);
			DiContextInterface->GetDiContainer().BindSpecific(StructDataBinding, DI::EBindConflictBehavior::BlueprintException);
		}
		P_NATIVE_END;
	}
}

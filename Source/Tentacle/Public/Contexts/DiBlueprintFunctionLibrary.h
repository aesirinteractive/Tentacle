// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "DIContextInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintInstancedStructLibrary.h"

#include "DiBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TENTACLE_API UDiBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "ObjectType", DefaultToSelf = "DiContextInterface"))
	static UObject* TryResolveObject(
		TScriptInterface<IDiContextInterface> DiContextInterface,
		UClass* ObjectType,
		FName BindingName);


	UFUNCTION( BlueprintCallable, CustomThunk,
		meta=( BlueprintInternalUseOnly=true, CustomStructureParam="OutStructData", DefaultToSelf = "DiContextInterface", ReturnDisplayName="Is Valid",
			ExpandEnumAsExecs="Result" ))
	static bool TryResolveStruct(
		TScriptInterface<IDiContextInterface> DiContextInterface,
		FName BindingName,
		UPARAM(ref)
		int32& OutStructData,
		EStructUtilsResult& Result);


	UFUNCTION( BlueprintCallable, CustomThunk,
		meta=( BlueprintInternalUseOnly=true, CustomStructureParam="OutStructData", DefaultToSelf = "DiContextInterface", ReturnDisplayName="Is Valid",
			ExpandEnumAsExecs="Result" ))
	static bool TryResolveStructCopy(
		TScriptInterface<IDiContextInterface> DiContextInterface,
		UScriptStruct* StructType,
		FName BindingName,
		int32& OutStructData,
		EStructUtilsResult& Result);


	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "DiContextInterface"))
	static void BindObject(
		TScriptInterface<IDiContextInterface> DiContextInterface,
		UObject* Object,
		FName BindingName);

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "DiContextInterface"))
	static void BindObjectAsType(
		TScriptInterface<IDiContextInterface> DiContextInterface,
		UObject* Object,
		UClass* ObjectBindingType,
		FName BindingName);

private:
	DECLARE_FUNCTION(execTryResolveStruct);
	DECLARE_FUNCTION(execTryResolveStructCopy);
};

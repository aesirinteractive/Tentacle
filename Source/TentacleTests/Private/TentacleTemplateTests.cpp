// Copyright 2023 Manuel Wagner https://www.singinwhale.com


#include "TentacleTemplates.h"
#include "CoreMinimal.h"
#include "Mocks/SimpleService.h"

namespace Tentacle
{
	static_assert(THasUClass<USimpleUService>::Value, "UClasses should have a UClass");
	static_assert(THasUStruct<USimpleUService>::Value, "UClasses should have a UStruct");
	static_assert(!TModels<CNativeMemberTypeIdProvider, USimpleUService>::Value, "UClasses should not provide member type ids");

	static_assert(!THasUClass<FSimpleNativeService>::Value, "Native classes should not have a UClass");
	static_assert(!THasUStruct<FSimpleNativeService>::Value, "Native classes should not have a UStruct");
	static_assert(TModels<CNativeMemberTypeIdProvider, FSimpleNativeService>::Value, "Native classes should provide native type ids through members");
	static_assert(!TModels<CNativeMemberTypeIdProvider, FMockEngineType>::Value, "Native foreign classes should provide native type ids through free functions");
	static_assert(sizeof(Tentacle::GetFreeTypeId<FMockEngineType>()), "Native foreign classes should provide native type ids through free functions");

	static_assert(!THasUClass<FSimpleUStructService>::Value, "UStruct should not have a UClass");
	static_assert(THasUStruct<FSimpleUStructService>::Value, "UStruct should have a UStruct");
	static_assert(!TModels<CNativeMemberTypeIdProvider, FSimpleUStructService>::Value, "UStructs should not provide a native member type");

	// non-nullable types
	static_assert(std::is_same_v<TBindingInstanceReferenceType<USimpleUService>, USimpleUService&>, "UObjects should be referenced as reference");
	static_assert(std::is_same_v<TBindingInstanceReferenceType<USimpleInterfaceImplementation>, USimpleInterfaceImplementation&>, "UObjects that implement an UInterface should be referenced as references");
	static_assert(std::is_same_v<TBindingInstanceReferenceType<ISimpleInterface>, const TScriptInterface<ISimpleInterface>&>, "UObjects that implement an UInterface should be referenced as references");
	static_assert(std::is_same_v<TBindingInstanceReferenceType<FSimpleUStructService>, const FSimpleUStructService&>, "UStructs should be referenced as const Refs");
	static_assert(std::is_same_v<TBindingInstanceReferenceType<FSimpleNativeService>, TSharedRef<FSimpleNativeService>>, "NativeTypes should be referenced as SharedRefs");

	// nullable types
	static_assert(std::is_same_v<TBindingInstanceNullableType<USimpleUService>, TObjectPtr<USimpleUService>>, "UObjects should be optionally passed as TObjectPtr");
	static_assert(std::is_same_v<TBindingInstanceNullableType<USimpleInterfaceImplementation>, TObjectPtr<USimpleInterfaceImplementation>>, "UObjects that implement an UInterface should be optionally passed as TObjectPtr");
	static_assert(std::is_same_v<TBindingInstanceNullableType<ISimpleInterface>, TScriptInterface<ISimpleInterface>>, "Interfaces should be optionally passed as TScriptInterface");
	static_assert(std::is_same_v<TBindingInstanceNullableType<FSimpleUStructService>, TOptional<FSimpleUStructService>>, "UStructs should be optionally passed as TOptional");
	static_assert(std::is_same_v<TBindingInstanceNullableType<FSimpleNativeService>, TSharedPtr<FSimpleNativeService>>, "Native Types should be optionally passed as SharedPtr");

	// non-nullable base types
	static_assert(std::is_same_v<TBindingRefBaseType<USimpleUService&>::Type, USimpleUService>, "UObjects should be referenced as reference");
	static_assert(std::is_same_v<TBindingRefBaseType<USimpleInterfaceImplementation&>::Type, USimpleInterfaceImplementation>, "UObjects that implement an UInterface should be referenced as references");
	static_assert(std::is_same_v<TBindingRefBaseType<const TScriptInterface<ISimpleInterface>&>::Type, ISimpleInterface>, "Interfaces should be referenced as TScriptInterface constRef");
	static_assert(std::is_same_v<TBindingRefBaseType<const FSimpleUStructService&>::Type, FSimpleUStructService>, "UStructs should be referenced as const Refs");
	static_assert(std::is_same_v<TBindingRefBaseType<TSharedRef<FSimpleNativeService>>::Type, FSimpleNativeService>, "NativeTypes should be referenced as SharedRefs");

	// nullable types
	static_assert(std::is_same_v<TBindingNullableBaseType<TObjectPtr<USimpleUService>>::Type, USimpleUService>, "UObjects should be optionally passed as TObjectPtr");
	static_assert(std::is_same_v<TBindingNullableBaseType<TObjectPtr<USimpleInterfaceImplementation>>::Type, USimpleInterfaceImplementation>, "UObjects that implement an UInterface should be optionally passed as TObjectPtr");
	static_assert(std::is_same_v<TBindingNullableBaseType<TScriptInterface<ISimpleInterface>>::Type, ISimpleInterface>, "Interfaces should be optionally passed as TScriptInterface");
	static_assert(std::is_same_v<TBindingNullableBaseType<TOptional<FSimpleUStructService>>::Type, FSimpleUStructService>, "UStructs should be optionally passed as TOptional");
	static_assert(std::is_same_v<TBindingNullableBaseType<TSharedPtr<FSimpleNativeService>>::Type, FSimpleNativeService>, "Native Types should be optionally passed as SharedPtr");

}

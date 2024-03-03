// Copyright 2023 Manuel Wagner https://www.singinwhale.com


#include "TentacleTemplates.h"
#include "CoreMinimal.h"
#include "Mocks/SimpleService.h"
#include "Templates/AreTypesEqual.h"

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
	static_assert(std::is_same_v<TBindingInstanceReferenceType<FSimpleUStructService>, const FSimpleUStructService&>, "UStructs should be referenced as const Refs");
	static_assert(std::is_same_v<TBindingInstanceReferenceType<FSimpleNativeService>, TSharedRef<FSimpleNativeService>>, "NativeTypes should be referenced as SharedRefs");

	// nullable types
	static_assert(std::is_same_v<TBindingInstanceNullableType<USimpleUService>, TObjectPtr<USimpleUService>>, "UObjects should be optionally passed as TObjectPtr");
	static_assert(std::is_same_v<TBindingInstanceNullableType<FSimpleUStructService>, TOptional<FSimpleUStructService>>, "UStructs should be optionally passed as TOptional");
	static_assert(std::is_same_v<TBindingInstanceNullableType<FSimpleNativeService>, TSharedPtr<FSimpleNativeService>>, "Native Types should be optionally passed as SharedPtr");
}

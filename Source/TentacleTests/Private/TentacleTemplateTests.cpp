// Copyright 2023 Manuel Wagner https://www.singinwhale.com


#include "TentacleTemplates.h"
#include "CoreMinimal.h"
#include "Mocks/SimpleService.h"
#include "Templates/AreTypesEqual.h"

namespace Tentacle
{
	static_assert(THasUClass<USimpleUService>::Value);
	static_assert(THasUStruct<USimpleUService>::Value);
	static_assert(!TModels<CNativeMemberTypeIdProvider, USimpleUService>::Value);

	static_assert(!THasUClass<FSimpleNativeService>::Value);
	static_assert(!THasUStruct<FSimpleNativeService>::Value);
	static_assert(TModels<CNativeMemberTypeIdProvider, FSimpleNativeService>::Value);

	static_assert(!THasUClass<FSimpleUStructService>::Value);
	static_assert(THasUStruct<FSimpleUStructService>::Value);
	static_assert(!TModels<CNativeMemberTypeIdProvider, FSimpleUStructService>::Value);

	static_assert(std::is_same_v<TBindingInstanceReferenceType<USimpleUService>, USimpleUService&>);
	static_assert(std::is_same_v<TBindingInstanceReferenceType<FSimpleUStructService>, const FSimpleUStructService&>);
	static_assert(std::is_same_v<TBindingInstanceReferenceType<FSimpleNativeService>, TSharedRef<FSimpleNativeService>>);

	static_assert(std::is_same_v<TBindingInstanceNullableType<USimpleUService>, TObjectPtr<USimpleUService>>);
	static_assert(std::is_same_v<TBindingInstanceNullableType<FSimpleUStructService>, TOptional<FSimpleUStructService>>);
	static_assert(std::is_same_v<TBindingInstanceNullableType<FSimpleNativeService>, TSharedPtr<FSimpleNativeService>>);
}

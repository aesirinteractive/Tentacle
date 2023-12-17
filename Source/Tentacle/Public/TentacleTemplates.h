// Copyright 2023 Manuel Wagner https://www.singinwhale.com


#pragma once

namespace Tentacle
{
	template <class T>
	TEnableIf<TIsIInterface<T>::Value, UClass*>::Type
	GetStaticClass()
	{
		return T::UClassType::StaticClass();
	}

	template <class T>
	TEnableIf<TIsDerivedFrom<T, UObject>::Value, UClass*>::Type
	GetStaticClass()
	{
		return T::StaticClass();
	}

	template <class T>
	TEnableIf<TModels<CStaticStructProvider, T>::Value, UScriptStruct*>::Type
	GetStaticClass()
	{
		return T::StaticStruct();
	}

	template <class T>
	using THasUClass = TOr<TIsIInterface<T>, TIsDerivedFrom<T, UObject>>;

	template <class T>
	using THasUStruct = TOr<THasUClass<T>, TModels<CStaticStructProvider, T>>;

	template <class T>
	constexpr bool HasUStruct()
	{
		return THasUStruct<T>::Value;
	}

	namespace BindingInstanceTypes
	{
		template <class T>
		using Native = TSharedRef<T>;

		template <class T>
		using UStruct = TOptional<T>;

		template <class T>
		using UObject = TObjectPtr<T>;
	}

	template <class T, typename TUObjectType, typename TUInterfaceType, typename TUStructType, typename TNativeType>
	using TBindingInstanceTypeSwitch = std::conditional_t<
		THasUStruct<T>::Value,
		std::conditional_t<THasUClass<T>::Value,
			std::conditional_t<TIsIInterface<T>::Value,
				TUInterfaceType,
				TUObjectType>,
			TUStructType>,
		TNativeType>;

	template <class T>
	using TBindingInstanceReferenceType = TBindingInstanceTypeSwitch<
		T,
		/* TUObjectType */		T&,
		/* TUInterfaceType */	T&,
		/* TUStructType */		const T&,
		/* TNativeType */		TSharedRef<T>>;

	template <class T>
	using TBindingInstanceNullableType = TBindingInstanceTypeSwitch<
		T,
		/* TUObjectType */ 		TObjectPtr<T>,
		/* TUInterfaceType */ 	T*,
		/* TUStructType */ 		TOptional<T>,
		/* TNativeType */ 		TSharedPtr<T>>;
}

// Copyright 2023 Manuel Wagner https://www.singinwhale.com


#pragma once

namespace Tentacle
{
	template <bool Predicate, typename TypeA = void, typename TypeB = void>
	struct TConditional;

	template <typename TypeA, typename TypeB>
	struct TConditional<true, TypeA, TypeB>
	{
		using Type = TypeA;
	};

	template <typename TypeA, typename TypeB>
	struct TConditional<false, TypeA, TypeB>
	{
		using Type = TypeB;
	};


	template <class T>
	typename TEnableIf<TIsIInterface<T>::Value, UClass*>::Type
	GetStaticClass()
	{
		return T::UClassType::StaticClass();
	}

	template <class T>
	typename TEnableIf<TIsDerivedFrom<T, UObject>::Value, UClass*>::Type
	GetStaticClass()
	{
		return T::StaticClass();
	}

	template <class T>
	typename TEnableIf<TModels<CStaticStructProvider, T>::Value, UScriptStruct*>::Type
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
	using TBindingInstanceTypeSwitch = typename TConditional<
		THasUStruct<typename TDecay<T>::Type>::Value,
		typename TConditional<THasUClass<typename TDecay<T>::Type>::Value,
		                      typename TConditional<TIsIInterface<typename TDecay<T>::Type>::Value,
		                                            TUInterfaceType,
		                                            TUObjectType>::Type,
		                      TUStructType>::Type,
		TNativeType>::Type;

	template <class T>
	using TBindingInstanceReferenceType = TBindingInstanceTypeSwitch<
		T,
		/* TUObjectType */ T&,
		/* TUInterfaceType */ const TScriptInterface<T>&,
		/* TUStructType */ const T&,
		/* TNativeType */ TSharedRef<T>>;

	template <class T>
	using TBindingInstanceNullableType = TBindingInstanceTypeSwitch<
		T,
		/* TUObjectType */ TObjectPtr<T>,
		/* TUInterfaceType */ TScriptInterface<T>,
		/* TUStructType */ TOptional<T>,
		/* TNativeType */ TSharedPtr<T>>;


	template <class T>
	struct TBindingRefBaseType;

	template <class T>
	struct TBindingRefBaseType<T&>
	{
		using Type = typename TDecay<T>::Type;
	};

	template <class T>
	struct TBindingRefBaseType<const TScriptInterface<T>&>
	{
		using Type = typename TScriptInterface<T>::InterfaceType;
	};

	template <class T>
	struct TBindingRefBaseType<TSharedRef<T>>
	{
		using Type = typename TSharedRef<T>::ElementType;
	};

	template <class T>
	struct TBindingRefBaseType<TObjectPtr<T>>
	{
		using Type = typename TDecay<T>::Type;
	};

	template <class T>
	struct TBindingNullableBaseType;

	template <class T>
	struct TBindingNullableBaseType<const T&>
	{
		using Type = typename TDecay<T>::Type;
	};

	template <class T>
	struct TBindingNullableBaseType<TScriptInterface<T>>
	{
		using Type = typename TScriptInterface<T>::InterfaceType;
	};

	template <class T>
	struct TBindingNullableBaseType<TSharedPtr<T>>
	{
		using Type = typename TSharedPtr<T>::ElementType;
	};

	template <class T>
	struct TBindingNullableBaseType<TOptional<T>>
	{
		using Type = typename TSharedPtr<T>::ElementType;
	};

	template <class T>
	struct TBindingNullableBaseType<TObjectPtr<T>>
	{
		using Type = typename TObjectPtr<T>::ElementType;
	};

	template <class T> TScriptInterface<T> ToRefType(TScriptInterface<T> Nullable) { return Nullable; }
	template <class T> TSharedRef<T> ToRefType(TSharedPtr<T> Nullable) { return Nullable.ToSharedRef(); }
	template <class T> T& ToRefType(const TOptional<T>& Nullable) { return Nullable.GetValue(); }
	template <class T> T& ToRefType(const TObjectPtr<T>& Nullable) { return *Nullable; }

	template<class T>
	struct TToRefType
	{
		using Type = TBindingInstanceReferenceType<typename TBindingNullableBaseType<T>::Type>;
	};
}

// Copyright 2023 Manuel Wagner https://www.singinwhale.com


#pragma once

namespace DI
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

	// Binding Instance Reference Type (Non-nullable)
	template <class T>
	using TBindingInstRef = TBindingInstanceTypeSwitch<
		T,
		/* TUObjectType */ TObjectPtr<T>, //using TObjectPtr<T> and not T& because converting from T& to TObjectPtr throws a warning. 
		/* TUInterfaceType */ const TScriptInterface<T>&,
		/* TUStructType */ const T&,
		/* TNativeType */ TSharedRef<T>>;

	// Binding Instance Optional Type (Nullable)
	template <class T>
	using TBindingInstPtr = TBindingInstanceTypeSwitch<
		T,
		/* TUObjectType */ TObjectPtr<T>,
		/* TUInterfaceType */ TScriptInterface<T>,
		/* TUStructType */ TOptional<T>,
		/* TNativeType */ TSharedPtr<T>>;


	/* Gets the base/inner/raw type from a TBindingInstRef */
	template <class T>
	struct TBindingInstRefBaseType;

	template <class T>
	struct TBindingInstRefBaseType<T&>
	{
		using Type = typename TDecay<T>::Type;
	};

	template <class T>
	struct TBindingInstRefBaseType<const TScriptInterface<T>&>
	{
		using Type = typename TScriptInterface<T>::InterfaceType;
	};

	template <class T>
	struct TBindingInstRefBaseType<TSharedRef<T>>
	{
		using Type = typename TSharedRef<T>::ElementType;
	};

	template <class T>
	struct TBindingInstRefBaseType<TObjectPtr<T>>
	{
		using Type = typename TDecay<T>::Type;
	};

	/* Gets the base/inner/raw type from a TBindingInstPtr */
	template <class T>
	struct TBindingInstPtrBaseType;

	template <class T>
	struct TBindingInstPtrBaseType<const T&>
	{
		using Type = typename TDecay<T>::Type;
	};

	template <class T>
	struct TBindingInstPtrBaseType<TScriptInterface<T>>
	{
		using Type = typename TScriptInterface<T>::InterfaceType;
	};

	template <class T>
	struct TBindingInstPtrBaseType<TSharedPtr<T>>
	{
		using Type = typename TSharedPtr<T>::ElementType;
	};

	template <class T>
	struct TBindingInstPtrBaseType<TOptional<T>>
	{
		using Type = typename TSharedPtr<T>::ElementType;
	};

	template <class T>
	struct TBindingInstPtrBaseType<TObjectPtr<T>>
	{
		using Type = typename TObjectPtr<T>::ElementType;
	};

	/**
	 * Converts a binding instance optional (TBindingInstPtr)to a binding instance reference (TBindingInstRef)
	 * Asserts if that is not possible.
	 */
	template <class T>
	TScriptInterface<T> ToRefType(TScriptInterface<T> Nullable)
	{
		static_assert(std::is_same_v<TBindingInstPtr<T>, TScriptInterface<T>>);
		static_assert(std::is_same_v<TBindingInstRef<T>, TScriptInterface<T>>);
		check(Nullable.GetObjectRef());
		return Nullable;
	}

	template <class T>
	TSharedRef<T> ToRefType(TSharedPtr<T> Nullable)
	{
		static_assert(std::is_same_v<TBindingInstPtr<T>, TSharedPtr<T>>);
		static_assert(std::is_same_v<TBindingInstRef<T>, TSharedRef<T>>);
		return Nullable.ToSharedRef();
	}

	template <class T>
	T& ToRefType(const TOptional<T>& Nullable)
	{
		static_assert(std::is_same_v<TBindingInstPtr<T>, TOptional<T>>);
		static_assert(std::is_same_v<TBindingInstRef<T>, T&>);
		return Nullable.GetValue();
	}

	template <class T>
	const TObjectPtr<T>& ToRefType(const TObjectPtr<T>& Nullable)
	{
		static_assert(std::is_same_v<TBindingInstPtr<T>, TObjectPtr<T>>);
		static_assert(std::is_same_v<TBindingInstRef<T>, TObjectPtr<T>>);
		check(Nullable);
		return Nullable;
	}

	/** Converts the binding instance optional type to a matching reference type. */
	template <class T>
	struct TToRefType
	{
		using Type = TBindingInstRef<typename TBindingInstPtrBaseType<T>::Type>;
	};
}

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
		/* TUInterfaceType */ TScriptInterface<T>,
		/* TUStructType */ const T&,
		/* TNativeType */ TSharedRef<T>>;

	// Binding Instance Optional Type (Nullable)
	template <class T>
	using TBindingInstPtr = TBindingInstanceTypeSwitch<
		T,
		/* TUObjectType */ TObjectPtr<T>,
		/* TUInterfaceType */ TScriptInterface<T>,
		/* TUStructType */ TOptional<const T&>,
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
	struct TBindingInstRefBaseType<TScriptInterface<T>>
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

	template<typename T>
	concept CHasType = requires
	{
		typename T::Type;
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
	T& ToRefType(TOptional<T>& Nullable)
	{
		static_assert(std::is_same_v<TBindingInstPtr<T>, TOptional<T>>);
		static_assert(std::is_same_v<TBindingInstRef<T>, T&>);
		return Nullable.GetValue();
	}

	template <class T>
	const T& ToRefType(const TOptional<T>& Nullable)
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



	namespace Private
	{
		template<typename ...TTupleTypes>
		std::tuple<TTupleTypes...> TTupleToStdTuple(TTuple<TTupleTypes...> Tuple)
		{
			return Tuple.ApplyAfter(&std::make_tuple);
		}
		template<typename ...TTupleTypes>
		TTuple<TTupleTypes...> TTupleFromStdTuple(std::tuple<TTupleTypes...> Tuple)
		{
			return std::apply(&MakeTuple, Forward(Tuple));
		}


	}

	template<typename ...TTuples>
	auto TupleCat(TTuples ...Tuples)
	{
		return Private::TTupleFromStdTuple(std::tuple_cat(Private::TTupleToStdTuple(Tuples)...));
	}

	/**
	 * Creates a TTuple<T, T, ..., T> of the requested length with every element initialised to Value.
	 * Example:
	 *   MakeUniformTuple<FName, 3>(NAME_None);   // -> TTuple<FName, FName, FName> { NAME_None, NAME_None, NAME_None }
	 */
	template <typename T, uint32... Indices>
	auto MakeUniformTupleImpl(const T& Value, TIntegerSequence<uint32, Indices...>)
	{
		// Expand the tuple with N copies of Value
		return MakeTuple((static_cast<void>(Indices), Value)...);
	}

	template <typename T, uint32 N>
	auto MakeUniformTuple(const T& Value)
	{
		return MakeUniformTupleImpl<T>(Value, TMakeIntegerSequence<uint32, N>{});
	}

	template<class T>
	using TVoid = decltype(static_cast<void>(DeclVal<T>()));

	namespace FunctionTraits
	{
		template <typename T>
		struct TFunctionTraits : TFunctionTraits<decltype(&T::operator())> {};

		// Member function pointer (non-const)
		template <typename C, typename R, typename... Args>
		struct TFunctionTraits<R (C::*)(Args...)>
		{
			using ResultType = R;
			using ArgsTuple  = TTuple<Args...>;
		};

		// Member function pointer (const)
		template <typename C, typename R, typename... Args>
		struct TFunctionTraits<R (C::*)(Args...) const>
		{
			using ResultType = R;
			using ArgsTuple  = TTuple<Args...>;
		};

		// Free function type
		template <typename R, typename... Args>
		struct TFunctionTraits<R(Args...)>
		{
			using ResultType = R;
			using ArgsTuple  = TTuple<Args...>;
		};

		// Free function pointer
		template <typename R, typename... Args>
		struct TFunctionTraits<R(*)(Args...)> : TFunctionTraits<R(Args...)> {};

		// UE's TFunction
		template <typename R, typename... Args>
		struct TFunctionTraits<TFunction<R(Args...)>> : TFunctionTraits<R(Args...)> {};
	}
}

// Copyright singinwhale https://www.singinwhale.com and contributors. Distributed under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "ResolveErrorBehavior.h"
#include "TentacleTemplates.h"
#include "WeakFuture.h"

namespace DI
{
	template <class TFutureValueType, class TDiContainer>
	class TAfterAsyncInject : public TWeakFuture<TFutureValueType>
	{
		using Super = TWeakFuture<TFutureValueType>;

	public:
		TAfterAsyncInject(TDiContainer& InDiContainer, Super&& WrappedFuture)
			: Super(MoveTemp(WrappedFuture))
			, DiContainer(InDiContainer)
		{
		}

		/**
		 * Binds an instance as its direct type
		 * Keep in mind that when resolving this type, that you need to use the same type as it has been bound with.
		 * Resolving via its parent class is not supported.
		 * If you need to resolve a binding by multiple types, you can bind it to all required types manually.
		 */
		template <class T>
		TWeakFuture<EBindResult> ThenBindInstance(DI::TBindingInstRef<T> Instance, EBindConflictBehavior ConflictBehavior = GDefaultConflictBehavior)
		{
			return this->template ThenBindNamedInstance<T>(Instance, FName(NAME_None), ConflictBehavior);
		}

		/**
		 * Binds a named instance as a specific type
		 * Keep in mind that when resolving this type, that you need to use the same type as it has been bound with.
		 * Resolving via its parent class is not supported.
		 * If you need to resolve a binding by multiple types, you can bind it to all required types manually.
		 */
		template <class TBinding>
		TWeakFuture<EBindResult> ThenBindNamedInstance(DI::TBindingInstRef<TBinding> Instance, const FName& InstanceName, EBindConflictBehavior ConflictBehavior = GDefaultConflictBehavior)
		{
			auto [BindPromise, Future] = MakeWeakPromisePair<EBindResult>();
			this->Then(
				[Promise = MoveTemp(BindPromise), WeakInstance = MakeWeakBindingInstPtr<TBinding>(Instance), DiContainer = DiContainer.AsShared(), InstanceName, ConflictBehavior](
				TWeakFuture<TFutureValueType> CompletedValue) mutable
				{
					if (CompletedValue.WasCanceled())
					{
						Promise.Cancel();
					}
					else
					{
						if (TOptional<TBindingInstRef<TBinding>> ValidInstance = ResolveWeakBindingInstPtr(WeakInstance))
						{
							Promise.EmplaceValue(DiContainer->Bind().template NamedInstance<TBinding>(*ValidInstance, InstanceName, ConflictBehavior));
						}
						else
						{
							Promise.Cancel();
						}
					}
				});
			return MoveTemp(Future);
		}

	private:
		TDiContainer& DiContainer;
	};

	/**
	 * DiContainer agnostic implementation of common injection operations.
	 * This helps in keeping the number of functions to be implemented for a DiContainer type to be very minimal
	 * to allow variations without resorting to inheritance.
	 */
	template <class TDiContainer>
	class TInjector
	{
	public:
		explicit TInjector(TDiContainer& InDiContainer)
			: DiContainer(InDiContainer)
		{
		}

		/**
		 * Calls the given function on instance with the resolved types.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * bool UExampleComponent::InjectDependencies(TObjectPtr<USimpleUService> InSimpleUService);
		 * bool bResult = DiContainer.Inject().IntoFunctionByType(*ExampleComponent, &UExampleComponent::InjectDependencies);
		 * @endcode
		 * @param Instance - the object on which to call MemberFunction
		 * @param MemberFunction - member function pointer to a member function of Instance
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs>
		TRetVal IntoFunctionByType(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypes<TArgs...>(ErrorBehavior);

			return ResolvedTypes.ApplyAfter(MemberFunction, Instance);
		}

		/**
		 * Calls the given function with the resolved types.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * TObjectPtr<USimpleUService> InjectDependencies(TObjectPtr<USimpleUService> SimpleUService){
		 *     return SimpleUService.
		 * };
		 * TObjectPtr<USimpleUService> Result = DiContainer.Inject().IntoFunctionByType(&InjectDependencies);
		 * @endcode
		 * @param FreeFunction - function pointer to a function
		 * @return whatever the passed function returns.
		 */
		template <class TRetVal, class... TArgs>
		TRetVal IntoFunctionByType(TRetVal (*FreeFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypes<TArgs...>(ErrorBehavior);
			return ResolvedTypes.ApplyAfter(FreeFunction);
		}

		/**
		 * Calls the given lambda with the resolved types.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * DiContainer.Inject().IntoLambda(
		 *   [&](TObjectPtr<USimpleUService> SimpleUService)
		 *   {
		 *     ExampleComponent->InjectDependenciesWithExtraArgs(SimpleUService, ExtraString);
		 *   }
		 *);
		 * @endcode
		 * @param Callable - callable to call with the resolved arguments
		 * @return whatever the passed function returns.
		 */
		template <class TCallable>
		auto IntoLambda(TCallable&& Callable, EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypesTuple<typename FunctionTraits::TFunctionTraits<TCallable>::ArgsTuple>(ErrorBehavior);
			return ResolvedTypes.ApplyAfter(Callable);
		}

		/**
		 * Calls the given lambda with the resolved types.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * DiContainer.Inject().IntoLambda(
		 *   [&](TObjectPtr<USimpleUService> SimpleUService)
		 *   {
		 *     ExampleComponent->InjectDependenciesWithExtraArgs(SimpleUService, ExtraString);
		 *   }
		 *);
		 * @endcode
		 * @param Callable - callable to call with the resolved arguments
		 * @return whatever the passed function returns.
		 */
		template <class TCallable, class ...TNames>
		auto IntoLambdaWithNames(TCallable&& Callable, EResolveErrorBehavior ErrorBehavior, TNames... BindingNames) const
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypesTupleNamed<typename FunctionTraits::TFunctionTraits<TCallable>::ArgsTuple>( ErrorBehavior, BindingNames... );
			return ResolvedTypes.ApplyAfter(Callable);
		}

		/** Version without explicit ErrorBehavior */
		template <class TCallable, class ...TNames>
		auto IntoLambdaWithNames(TCallable&& Callable, TNames... BindingNames) const
		{
			return this->IntoLambdaWithNames(MoveTemp(Callable), GDefaultResolveErrorBehavior, BindingNames...);
		}

		/**
		 * Calls the given function on instance with the resolved named types.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * bool UExampleComponent::InjectDependencies(TObjectPtr<USimpleUService> InSimpleUService);
		 * bool bResult = DiContainer.Inject().IntoFunctionWithNames(*ExampleComponent, &UExampleComponent::InjectDependencies, "SimpleService");
		 * @endcode
		 * @param Instance - the object on which to call MemberFunction
		 * @param MemberFunction - member function pointer to a member function of Instance
		 * @param ErrorBehavior - specifies what to do if any of the bindings are not found.
		 * @param BindingNames - List the names of the bindings to resolve. Use NAME_None for type-only bindings.
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		TRetVal IntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior, TNames&&... BindingNames) const
		{
			auto ResolvedTypes = this->template ResolveFromArgumentNames<TArgs...>(ErrorBehavior, Forward<TNames>(BindingNames)...);
			return ResolvedTypes.ApplyAfter(MemberFunction, &Instance);
		}

		/** Version without explicit ErrorBehavior */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		TRetVal IntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), TNames&&... BindingNames) const
		{
			return this->IntoFunctionWithNames<T, TRetVal, TArgs...>(Instance, MemberFunction, GDefaultResolveErrorBehavior, Forward<TNames>(BindingNames)...);
		}

		/**
		 * Calls the given function with the resolved named types.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * TObjectPtr<USimpleUService> InjectDependencies(TObjectPtr<USimpleUService> SimpleUService){
		 *     return SimpleUService.
		 * };
		 * TObjectPtr<USimpleUService> Result = DiContainer.Inject().IntoFunctionByType(&InjectDependencies, DI::GDefaultResolveErrorBehavior, "SimpleService");
		 * @endcode
		 * @param FreeFunction - function pointer to a function
		 * @param ErrorBehavior - specifies what to do if any of the bindings are not found.
		 * @param BindingNames - List the names of the bindings to resolve. Use NAME_None for type-only bindings.
		 * @return whatever the passed function returns.
		 */
		template <class TRetVal, class... TArgs, class... TNames>
		TRetVal IntoFunctionWithNames(TRetVal (*FreeFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior, TNames&&... BindingNames) const
		{
			auto ResolvedTypes = this->template ResolveFromArgumentNames<TArgs...>(ErrorBehavior, Forward<TNames>(BindingNames)...);
			return ResolvedTypes.ApplyAfter(FreeFunction);
		}

		/** Version without explicit ErrorBehavior */
		template <class TRetVal, class... TArgs, class... TNames>
		TRetVal IntoFunctionWithNames(TRetVal (*FreeFunction)(TArgs...), TNames&&... BindingNames) const
		{
			return this->IntoFunctionWithNames<TRetVal, TArgs...>(FreeFunction, GDefaultResolveErrorBehavior, Forward<TNames>(BindingNames)...);
		}

		/**
		 * Calls the given function on the instance with the resolved types when they are fully resolvable.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * bool UExampleComponent::InjectDependencies(TObjectPtr<USimpleUService> InSimpleUService);
		 * bool bResult = DiContainer.Inject().AsyncIntoFunctionByType(*ExampleComponent, &UExampleComponent::InjectDependencies);
		 * @endcode
		 * @warning If not all the bindings can be resolved, the function will not be called at all!
		 * @param Instance - the object on which to call MemberFunction
		 * @param MemberFunction - member function pointer to a member function of Instance
		 * @param ErrorBehavior - specifies what to do if any of the bindings are not found.
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, TAfterAsyncInject<TRetVal, TDiContainer>>::Type
		AsyncIntoFunctionByType(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			return this->template AsyncIntoFunctionWithNames<T>(Instance, MemberFunction, ErrorBehavior, (TVoid<TArgs>(), NAME_None)...);
		}

		/**
		 * Calls the given function on the instance with the resolved types when they are fully resolvable.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * bool FExampleNative::Initialize(TSharedPtr<FSimpleNativeService> SimpleNative);
		 * DiContainer.Inject().AsyncIntoFunctionByType(MakeShared<FExampleNative>(), &FExampleNative::Initialize);
		 * @endcode
		 * @warning If not all the bindings can be resolved, the function will not be called at all!
		 * @param Instance - the object on which to call MemberFunction
		 * @param MemberFunction - member function pointer to a member function of Instance
		 * @param ErrorBehavior - specifies what to do if any of the bindings are not found.
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs>
		typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, TAfterAsyncInject<TRetVal, TDiContainer>>::Type
		AsyncIntoFunctionByType(TSharedRef<T> Instance,
			TRetVal (T::*MemberFunction)(TArgs...),
			EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			return this->template AsyncIntoFunctionWithNames<T>(Instance, MemberFunction, ErrorBehavior, (TVoid<TArgs>(), NAME_None)...);
		}

		/**
		 * Calls the given function on the instance with the resolved types when they are fully resolvable.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * bool FExampleNative::Initialize(TSharedPtr<FSimpleNativeService> SimpleNative);
		 * DiContainer.Inject().AsyncIntoFunctionByType(MakeShared<FExampleNative>(), &FExampleNative::Initialize);
		 * @endcode
		 * @warning If not all the bindings can be resolved, the function will not be called at all!
		 * @param Instance - the object on which to call MemberFunction
		 * @param MemberFunction - member function pointer to a member function of Instance
		 * @param ErrorBehavior - specifies what to do if any of the bindings are not found.
		 * @param BindingNames - List the names of the bindings to resolve. Use NAME_None for type-only bindings.
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived && !TIsConst<TDiContainer>::Value, TAfterAsyncInject<TRetVal, TDiContainer>>::Type
		AsyncIntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior, TNames&&... BindingNames) const
		{
			return TAfterAsyncInject(DiContainer, this->template AsyncIntoFunctionWithNamesInternal<T, TRetVal, TArgs...>(Instance, MemberFunction, ErrorBehavior, BindingNames...));
		}
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived  && TIsConst<TDiContainer>::Value, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior, TNames&&... BindingNames) const
		{
			return this->template AsyncIntoFunctionWithNamesInternal<T, TRetVal, TArgs...>(Instance, MemberFunction, ErrorBehavior, BindingNames...);
		}

		/** Version for UObject types referenced without ErrorBehavior */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived && !TIsConst<TDiContainer>::Value, TAfterAsyncInject<TRetVal, TDiContainer>>::Type
		AsyncIntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), TNames&&... BindingNames) const
		{
			return TAfterAsyncInject(DiContainer, this->template AsyncIntoFunctionWithNamesInternal<T, TRetVal, TArgs...>(Instance, MemberFunction, GDefaultResolveErrorBehavior, BindingNames...));
		}

		/** Version for UObject types referenced without ErrorBehavior */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived && TIsConst<TDiContainer>::Value, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), TNames&&... BindingNames) const
		{
			return this->template AsyncIntoFunctionWithNamesInternal<T, TRetVal, TArgs...>(Instance, MemberFunction, GDefaultResolveErrorBehavior, BindingNames...);
		}

		/** Version for native types referenced via SharedPtr */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived && !TIsConst<TDiContainer>::Value, TAfterAsyncInject<TRetVal, TDiContainer>>::Type
		AsyncIntoFunctionWithNames(TSharedRef<T> Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior, TNames... BindingNames) const
		{
			return TAfterAsyncInject(DiContainer, this->template AsyncIntoFunctionWithNamesInternal(Instance, MemberFunction, ErrorBehavior, BindingNames...));
		}

		/** Version for native types referenced via SharedPtr without ErrorBehavior */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived && TIsConst<TDiContainer>::Value, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionWithNames(TSharedRef<T> Instance, TRetVal (T::*MemberFunction)(TArgs...), TNames... BindingNames) const
		{
			return this->template AsyncIntoFunctionWithNames<T, TRetVal, TArgs...>(Instance, MemberFunction, GDefaultResolveErrorBehavior, BindingNames...);
		}

	private:
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionWithNamesInternal(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior, TNames&&... BindingNames) const
		{
			return this->DiContainer
			           .Resolve()
			           .template WaitForManyNamed<typename TBindingInstPtrBaseType<TArgs>::Type...>(&Instance, ErrorBehavior, BindingNames...)
			           .ExpandNext([WeakInstance = MakeWeakObjectPtr(&Instance), MemberFunction](TOptional<TArgs>... OptionalResolvedTypes)
			           {
				           if (T* ValidInstance = WeakInstance.Get())
				           {
					           const bool bAllIsResolved = (OptionalResolvedTypes.IsSet() && ...);
					           if (bAllIsResolved)
					           {
						           return (*ValidInstance.*MemberFunction)(OptionalResolvedTypes.GetValue()...);
					           }
				           }
			           });
		}

		/** Version for native types referenced via SharedPtr */
		template <class T, class TRetVal, class... TArgs, class... TNames>
		typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionWithNamesInternal(TSharedRef<T> Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior, TNames... BindingNames) const
		{
			return this->DiContainer
			           .Resolve()
			           .template WaitForManyNamed<TBindingInstPtrBaseType<TArgs>::Type...>(nullptr, ErrorBehavior, BindingNames...)
			           .ExpandNext([WeakInstance = Instance.ToWeakPtr(), MemberFunction](TOptional<TArgs>... OptionalResolvedTypes)
			           {
				           if (TSharedRef<T> ValidInstance = WeakInstance.Pin())
				           {
					           const bool bAllIsResolved = (OptionalResolvedTypes.IsSet() && ...);
					           if (bAllIsResolved)
					           {
						           return ((*ValidInstance).*MemberFunction)(OptionalResolvedTypes.GetValue()...);
					           }
				           }
			           });
		}

		template <class... TArgumentTypes>
		auto ResolveFromArgumentTypes(EResolveErrorBehavior ErrorBehavior) const
		{
			return MakeTuple(this->ResolveFromArgumentType<TArgumentTypes>(ErrorBehavior)...);
		}

		template <class TTupleWithArgsType>
		struct TResolveTuple
		{
		};

		template <class... TTupleWithArgsTypes>
		struct TResolveTuple<TTuple<TTupleWithArgsTypes...>>
		{
			static TTuple<TTupleWithArgsTypes...> Resolve(const TInjector& Injector, EResolveErrorBehavior ErrorBehavior)
			{
				return Injector.ResolveFromArgumentTypes<TTupleWithArgsTypes...>(ErrorBehavior);
			}

			static TTuple<TTupleWithArgsTypes...> ResolveNamed(const TInjector& Injector, EResolveErrorBehavior ErrorBehavior, FName BindingNames...)
			{
				return Injector.ResolveFromArgumentNames<TTupleWithArgsTypes...>(ErrorBehavior, BindingNames);
			}
		};

		template <class TTupleWithArgsType>
		auto ResolveFromArgumentTypesTuple(EResolveErrorBehavior ErrorBehavior) const
		{
			return TResolveTuple<TTupleWithArgsType>::Resolve(*this, ErrorBehavior);
		}

		template <class TTupleWithArgsType, class ...TNames>
		auto ResolveFromArgumentTypesTupleNamed(EResolveErrorBehavior ErrorBehavior, TNames... BindingNames) const
		{
			return TResolveTuple<TTupleWithArgsType>::ResolveNamed(*this, ErrorBehavior, BindingNames...);
		}

		template <class TArgumentType>
		auto ResolveFromArgumentType(EResolveErrorBehavior ErrorBehavior) const
		{
			static_assert(CHasType<TBindingInstPtrBaseType<TArgumentType>>,
				"The Argument Type cannot be used for dependency injection. If you cannot change it, resolve each argument individually and call the function manually.");
			return this->DiContainer.Resolve().template TryGet<typename TBindingInstPtrBaseType<TArgumentType>::Type>(ErrorBehavior);
		}

		template <class... TArgumentTypes, class ...TNames>
		auto ResolveFromArgumentNames(EResolveErrorBehavior ErrorBehavior, TNames... Names) const
		{
			return MakeTuple(DiContainer.Resolve().template TryGetNamed<typename TBindingInstRefBaseType<TArgumentTypes>::Type>(Names, ErrorBehavior)...);
		}

		TDiContainer& DiContainer;
	};
}

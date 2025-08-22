// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "ResolveErrorBehavior.h"
#include "TentacleTemplates.h"
#include "WeakFuture.h"

namespace DI
{
	/**
	 * DiContainer agnostic implementation of common injection operations.
	 * This helps in keeping the number of functions to be implemented for a DiContainer type to be very minimal
	 * to allow variations without resorting to inheritance.
	 */
	template <class TDiContainer>
	class TInjector
	{
	public:
		explicit TInjector(const TDiContainer& InDiContainer)
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
		TRetVal IntoFunctionByType(T& Instance, TRetVal (T::*MemberFunction)(TArgs...))
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypes<TArgs...>();

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
		TRetVal IntoFunctionByType(TRetVal (*FreeFunction)(TArgs...))
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypes<TArgs...>();
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
		auto IntoLambda(TCallable&& Callable)
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypesTuple<typename FunctionTraits::TFunctionTraits<TCallable>::ArgsTuple>();
			return ResolvedTypes.ApplyAfter(Callable);
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
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs>
		TRetVal IntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), FName Names...)
		{
			auto ResolvedTypes = this->template ResolveFromArgumentNames<TArgs...>(Names);
			return ResolvedTypes.ApplyAfter(MemberFunction, &Instance);
		}

		/**
		 * Calls the given function with the resolved named types.
		 * Keep in mind the different types for the different bindings. For reference see TBindingInstPtr.
		 * Example:
		 * @code
		 * TObjectPtr<USimpleUService> InjectDependencies(TObjectPtr<USimpleUService> SimpleUService){
		 *     return SimpleUService.
		 * };
		 * TObjectPtr<USimpleUService> Result = DiContainer.Inject().IntoFunctionByType(&InjectDependencies, "SimpleService");
		 * @endcode
		 * @param FreeFunction - function pointer to a function
		 * @return whatever the passed function returns.
		 */
		template <class TRetVal, class... TArgs>
		TRetVal IntoFunctionWithNames(TRetVal (*FreeFunction)(TArgs...), FName Names...)
		{
			auto ResolvedTypes = this->template ResolveFromArgumentNames<TArgs...>(Names);
			return ResolvedTypes.ApplyAfter(FreeFunction);
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
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionByType(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior)
		{
			return this->DiContainer
			           .Resolve()
			           .template TryResolveFutureTypeInstances<TBindingInstPtrBaseType<TArgs>::Type...>(nullptr, ErrorBehavior)
			           .ExpandNext(
				           [WeakInstance = MakeWeakObjectPtr(&Instance), MemberFunction](TOptional<TArgs>... OptionalResolvedTypes)
				           {
					           if (T* ValidInstance = WeakInstance.Get())
					           {
						           const bool bAllIsResolved = (OptionalResolvedTypes.IsSet() && ...);
						           if (bAllIsResolved)
						           {
							           return (*ValidInstance.*MemberFunction)(OptionalResolvedTypes.GetValue()...);
						           }
					           }
				           }
			           );
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
		 * @return whatever the passed function returns.
		 */
		template <class T, class TRetVal, class... TArgs>
		typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionByType(TSharedRef<T> Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior)
		{
			return this->DiContainer
			           .Resolve()
			           .template TryResolveFutureTypeInstances<TBindingInstPtrBaseType<TArgs>::Type...>(nullptr, ErrorBehavior)
			           .ExpandNext(
				           [WeakInstance = Instance.ToWeakPtr(), MemberFunction](TOptional<TArgs>... OptionalResolvedTypes)
				           {
					           if (TSharedRef<T> ValidInstance = WeakInstance.Pin())
					           {
						           const bool bAllIsResolved = (OptionalResolvedTypes.IsSet() && ...);
								   if (bAllIsResolved)
								   {
									   return ((*ValidInstance).*MemberFunction)(OptionalResolvedTypes.GetValue()...);
								   }
					           }
				           }
			           );
		}

	private:
		template <class... TArgumentTypes>
		auto ResolveFromArgumentTypes()
		{
			return MakeTuple(this->ResolveFromArgumentType<TArgumentTypes>()...);
		}

		template<class TTupleWithArgsType>
		struct TResolveTuple { };
		template<class ...TTupleWithArgsTypes>
		struct TResolveTuple<TTuple<TTupleWithArgsTypes...>>
		{
			TTuple<TTupleWithArgsTypes...> Resolve(TInjector& Injector)
			{
				return Injector.ResolveFromArgumentTypes<TTupleWithArgsTypes...>();
			}
		};
		template<class TTupleWithArgsType>
		auto ResolveFromArgumentTypesTuple()
		{
			return TResolveTuple<TTupleWithArgsType>().Resolve(*this);
		}

		template <class TArgumentType>
		auto ResolveFromArgumentType()
		{
			static_assert(CHasType<TBindingInstPtrBaseType<TArgumentType>>, "The Argument Type cannot be used for dependency injection. If you cannot change it, resolve each argument individually and call the function manually.");
			return this->DiContainer.Resolve().template TryResolveTypeInstance<typename TBindingInstPtrBaseType<TArgumentType>::Type>();
		}

		template <class... TArgumentTypes>
		auto ResolveFromArgumentNames(FName Names...)
		{
			return MakeTuple(DiContainer.Resolve().template TryResolveNamedInstance<typename TBindingInstRefBaseType<TArgumentTypes>::Type>(Names)...);
		}

		const TDiContainer& DiContainer;
	};
}

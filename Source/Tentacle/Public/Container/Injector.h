// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "ResolveErrorBehavior.h"
#include "TentacleTemplates.h"
#include "WeakFuture.h"

namespace DI
{
	/**
	 * 
	 */
	template <class TDiContainer>
	class TInjector
	{
	public:
		explicit TInjector(const TDiContainer& InDiContainer)
			: DiContainer(InDiContainer)
		{
		}

		template <class T, class TRetVal, class... TArgs>
		TRetVal IntoFunctionByType(T& Instance, TRetVal (T::*MemberFunction)(TArgs...))
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypes<TArgs...>();

			return ResolvedTypes.ApplyAfter(MemberFunction, Instance);
		}

		template <class TRetVal, class... TArgs>
		TRetVal IntoFunctionByType(TRetVal (*FreeFunction)(TArgs...))
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypes<TArgs...>();
			return ResolvedTypes.ApplyAfter(FreeFunction);
		}

		template <class TCallable>
		auto IntoLambda(TCallable&& Callable)
		{
			auto ResolvedTypes = this->template ResolveFromArgumentTypesTuple<typename FunctionTraits::TFunctionTraits<TCallable>::ArgsTuple>();
			return ResolvedTypes.ApplyAfter(Callable);
		}

		template <class T, class TRetVal, class... TArgs>
		TRetVal IntoFunctionWithNames(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), FName Names...)
		{
			auto ResolvedTypes = this->template ResolveFromArgumentNames<TArgs...>(Names);
			return ResolvedTypes.ApplyAfter(MemberFunction, &Instance);
		}

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

		template <class T, class TRetVal, class... TArgs>
		typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, TWeakFuture<TRetVal>>::Type
		AsyncIntoFunctionByType(T& Instance, TRetVal (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior)
		{
			return this->DiContainer
			           .Resolve()
			           .template TryResolveFutureTypeInstances<TBindingInstPtrBaseType<TArgs>::Type...>(nullptr, ErrorBehavior)
			           .ExpandNext(
				           [Instance, MemberFunction](TOptional<TArgs>... OptionalResolvedTypes)
				           {
					           const bool bAllIsResolved = (OptionalResolvedTypes.IsSet() && ...);
					           if (bAllIsResolved)
					           {
						           return (Instance.*MemberFunction)(OptionalResolvedTypes.GetValue()...);
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

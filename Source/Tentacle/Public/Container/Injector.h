// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "ResolveErrorBehavior.h"
#include "TentacleTemplates.h"

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

		template <class T, class... TArgs>
		void IntoFunctionByType(T& Instance, void (T::*MemberFunction)(TArgs...))
		{
			auto ResolvedTypes = this->DiContainer.Resolve().template TryResolveTypeInstances<TBindingInstPtrBaseType<TArgs>::Type...>();
			ResolvedTypes.ApplyAfter(MemberFunction, &Instance);
		}

		template <class T, class... TArgs>
		typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, void>::Type
		AsyncIntoFunctionByType(T& Instance, void (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior)
		{
			this->DiContainer.Resolve()
			    .template TryResolveFutureTypeInstances<TBindingInstPtrBaseType<TArgs>::Type...>(nullptr, ErrorBehavior)
			    .Next(
				    [WeakInstance = MakeWeakObjectPtr(&Instance), MemberFunction](TArgs... ResolvedTypes)
				    {
					    if (T* ValidInstance = WeakInstance.Get())
					    {
						    (*ValidInstance.*MemberFunction)(ResolvedTypes...);
					    }
				    }
			    );
		}

		template <class T, class... TArgs>
		typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, void>::Type
		AsyncIntoFunctionByType(T& Instance, void (T::*MemberFunction)(TArgs...), EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior)
		{
			this->DiContainer.Resolve()
			    .template TryResolveFutureTypeInstances<TBindingInstPtrBaseType<TArgs>::Type...>(nullptr, ErrorBehavior)
			    .Next(
				    [Instance, MemberFunction](TArgs... ResolvedTypes)
				    {
					    (Instance.*MemberFunction)(ResolvedTypes...);
				    }
			    );
		}

		template <class... TArgs, class... TExtraArgs>
		void IntoFunctionByType(void (*FreeFunction)(TExtraArgs..., TArgs...), TExtraArgs... ExtraArgs)
		{
			auto ResolvedTypes = this->ResolveFromArgumentTypes<TArgs...>();
			ResolvedTypes.ApplyAfter(FreeFunction, Forward<TExtraArgs...>(ExtraArgs)...);
		}

		template <class T, class... TArgs>
		void IntoFunctionWithNames(T& Instance, void (T::*MemberFunction)(TArgs...), FName Names...)
		{
			auto ResolvedTypes = this->ResolveFromArgumentTypes<TArgs...>();
			ResolvedTypes.ApplyAfter(MemberFunction, &Instance);
		}

	private:
		template <class... TArgumentTypes>
		auto ResolveFromArgumentTypes()
		{
			return MakeTuple(DiContainer.template ResolveTypeInstance<typename TBindingInstRefBaseType<TArgumentTypes>::Type>()...);
		}

		template <class... TArgumentTypes>
		auto ResolveFromArgumentNames(FName Names...)
		{
			return MakeTuple(DiContainer.template ResolveNamedInstance<typename TBindingInstRefBaseType<TArgumentTypes>::Type>(Names)...);
		}

		const TDiContainer& DiContainer;
	};
}

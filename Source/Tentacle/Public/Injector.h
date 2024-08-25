// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "Container/DiContainerConcept.h"
#include "TentacleTemplates.h"

namespace Tentacle
{
	/**
	 * 
	 */
	template <DiContainerConcept TDiContainer>
	class TInjector
	{
	public:
		explicit TInjector(const TDiContainer& InDiContainer)
			: DiContainer(InDiContainer)
		{
		}

		template <class... TTypesToResolve>
		auto ResolveFromArgumentTypes()
		{
			return MakeTuple(DiContainer.template ResolveTypeInstance<typename TBindingInstRefBaseType<TTypesToResolve>::Type>()...);
		}

		template <class T, class... TArgs>
		void InjectByFunction(T& Instance, void (T::*MemberFunction)(TArgs...))
		{
			auto ResolvedTypes = this->ResolveFromArgumentTypes<TArgs...>();
			ResolvedTypes.ApplyAfter(MemberFunction, Instance);
		}
		
		template <class... TArgs, class... TExtraArgs>
		void InjectByFunction(void (*FreeFunction)(TExtraArgs..., TArgs...), TExtraArgs... ExtraArgs)
		{
			auto ResolvedTypes = this->ResolveFromArgumentTypes<TArgs...>();
			ResolvedTypes.ApplyAfter(FreeFunction, Forward<TExtraArgs...>(ExtraArgs)...);
		}

	private:
		const TDiContainer& DiContainer;
	};

}

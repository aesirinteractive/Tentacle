// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContainer.h"

namespace DI
{
	/**
	 * 
	 */
	class TENTACLE_API FChainedDiContainer :
		public TSharedFromThis<FChainedDiContainer>
	{
	public:
		FChainedDiContainer() = default;
		virtual ~FChainedDiContainer() = default;

		void SetParentContainer(TWeakPtr<FChainedDiContainer> DiContainer);

		void AddReferencedObjects(FReferenceCollector& Collector)
		{
			MyDiContainer.AddReferencedObjects(Collector);
		}

		TBindingHelper<FChainedDiContainer> Bind() { return TBindingHelper(*this); }
		TResolveHelper<FChainedDiContainer> Resolve() const { return TResolveHelper(*this); };
		TInjector<FChainedDiContainer> Inject() const { return TInjector(*this); };

		EBindResult BindSpecific(TSharedRef<DI::FDependencyBinding> SpecificBinding, EBindConflictBehavior ConflictBehavior);
		TSharedPtr<DI::FDependencyBinding> FindBinding(const FDependencyBindingId& BindingId) const;
		FBindingSubscriptionList::FOnInstanceBound& Subscribe(const FDependencyBindingId& BindingId) const;

		bool Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle) const;

	private:
		FDiContainer& GetDiContainer() { return MyDiContainer; };
		const FDiContainer& GetDiContainer() const { return MyDiContainer; };

		FDiContainer MyDiContainer;

		TWeakPtr<FChainedDiContainer> ParentContainer;

		// Mutable so we can clean up invalid children in getters
		mutable TArray<TWeakPtr<FChainedDiContainer>, TInlineAllocator<1>> ChildrenContainers;
	};

	namespace DI
	{
		static_assert(TModels<CDiContainer, FChainedDiContainer>::Value);
		static_assert(TModels<CTypeHasBindSpecific, FChainedDiContainer>::Value);
		static_assert(TModels<CTypeHasFindBinding, FChainedDiContainer>::Value);
		static_assert(TModels<CTypeHasSubscribe, FChainedDiContainer>::Value);
		static_assert(DiContainerConcept<FChainedDiContainer>);
	}
}

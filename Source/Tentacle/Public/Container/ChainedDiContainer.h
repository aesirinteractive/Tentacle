// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContainer.h"

namespace DI
{
	/**
	 * DI Container that can defer resolving of bindings to its single parent.
	 *
	 * Binding will cause the parent containers to notify its children that a new binding has been bound.
	 * This behavior to prevent the memory overhead of duplicate bindings in favor of worse performance at bind and resolve time.
	 *
	 * Children do not cache bindings that have been bound in parent containers.
	 */
	class TENTACLE_API FChainedDiContainer :
		public TSharedFromThis<FChainedDiContainer>
	{
	public:
		FChainedDiContainer() = default;

		// Technically, we could have a copy constructor, but copying is usually a user error, so we delete it to catch these cases earlier.
		FChainedDiContainer(const FChainedDiContainer&) = delete;

		virtual ~FChainedDiContainer() = default;

		/**
		 * Sets the chained parent of this DI Container.
		 * @warning This has to happen before any async resolves are performed.
		 */
		void SetParentContainer(TWeakPtr<FChainedDiContainer> DiContainer);

		/** Call this from the owning type to prevent types and bindings to be garbage collected. */
		void AddReferencedObjects(FReferenceCollector& Collector);

		/** Get the Binding API */
		TBindingHelper<FChainedDiContainer> Bind() { return TBindingHelper(*this); }
		/** Get the Resolving API */
		TResolveHelper<FChainedDiContainer> Resolve() const { return TResolveHelper(*this); };
		/** Get the Injection API */
		TInjector<FChainedDiContainer> Inject() const { return TInjector(*this); };

		// - DiContainerConcept
		/** Bind a specific binding. */
		EBindResult BindSpecific(TSharedRef<DI::FBinding> SpecificBinding, EBindConflictBehavior ConflictBehavior);
		/** Find a binding by its ID. */
		TSharedPtr<DI::FBinding> FindBinding(const FBindingId& BindingId) const;

		/**
		 * Get the delegate that will be invoked a single time when the binding with the given ID is bound.
		 * If the binding is already bound the event will never fire.
		 * @param BindingId the ID of the binding to be notified about.
		 */
		FBindingSubscriptionList::FOnInstanceBound& Subscribe(const FBindingId& BindingId) const;
		// --

		/**
		 * Unsubscribe from being notified about a binding.
		 * @param BindingId The ID of the binding where there is a subscription
		 * @param DelegateHandle The handle that was returned when the subscription was created
		 * @return true if there was a subscription and it has been successfully removed.
		 */
		bool Unsubscribe(const FBindingId& BindingId, FDelegateHandle DelegateHandle) const;

	private:
		/** Our own registered Bindings */
		TMap<FBindingId, TSharedRef<DI::FBinding>> Bindings = {};

		// mutable so we can use it in const resolve methods
		mutable FBindingSubscriptionList Subscriptions;

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

// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BindConflictBehavior.h"
#include "BindingHelper.h"
#include "BindingSubscriptionList.h"
#include "BindResult.h"
#include "DependencyBinding.h"
#include "DependencyBindingId.h"
#include "DiContainerConcept.h"
#include "Injector.h"
#include "ResolveHelper.h"
#include "TentacleTemplates.h"
#include "TypeId.h"
#include "WeakFuture.h"
#include "Tentacle.h"

namespace DI
{
	/**
	 * Simple Di Container that does everything locally.
	 */
	class TENTACLE_API FDiContainer
	{
	public:

		EBindResult BindSpecific(TSharedRef<DI::FDependencyBinding> SpecificBinding, EBindConflictBehavior ConflictBehavior);

		TSharedPtr<DI::FDependencyBinding> FindBinding(const FDependencyBindingId& BindingId) const;

		FBindingSubscriptionList::FOnInstanceBound& Subscribe(const FDependencyBindingId& BindingId) const;

		bool Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle) const;

		void AddReferencedObjects(FReferenceCollector& Collector);

		TBindingHelper<FDiContainer> Bind();
		TResolveHelper<FDiContainer> Resolve() const;
		TInjector<FDiContainer> Inject() const;
	protected:
		TMap<FDependencyBindingId, TSharedRef<DI::FDependencyBinding>> Bindings = {};
		mutable FBindingSubscriptionList Subscriptions;
	};

	static_assert(TModels<CDiContainer, FDiContainer>::Value);
	static_assert(TModels<CTypeHasBindSpecific, FDiContainer>::Value);
	static_assert(TModels<CTypeHasFindBinding, FDiContainer>::Value);
	static_assert(TModels<CTypeHasSubscribe, FDiContainer>::Value);
	static_assert(DiContainerConcept<FDiContainer>);
}

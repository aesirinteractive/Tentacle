// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "Container/DiContainer.h"

namespace DI
{
	bool FDiContainer::Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle) const
	{
		return Subscriptions.Unsubscribe(BindingId, DelegateHandle);
	}

	void FDiContainer::AddReferencedObjects(FReferenceCollector& Collector)
	{
		for (auto& [BindingId, Binding] : Bindings)
		{
			Binding->AddReferencedObjects(Collector);
		}
	}

	TSharedPtr<DI::FDependencyBinding> FDiContainer::FindBinding(const FDependencyBindingId& BindingId) const
	{
		if (const TSharedRef<DI::FDependencyBinding>* DependencyBinding = Bindings.Find(BindingId))
		{
			return *DependencyBinding;
		}
		return nullptr;
	}

	FBindingSubscriptionList::FOnInstanceBound& FDiContainer::Subscribe(const FDependencyBindingId& BindingId) const
	{
		return Subscriptions.SubscribeOnce(BindingId);
	}

	TBindingHelper<FDiContainer> FDiContainer::Bind()
	{
		return TBindingHelper<FDiContainer>(*this);
	}

	TResolveHelper<FDiContainer> FDiContainer::Resolve() const
	{
		return TResolveHelper<FDiContainer>(*this);
	}

	TInjector<FDiContainer> FDiContainer::Inject() const
	{
		return TInjector<FDiContainer>(*this);
	}

	EBindResult FDiContainer::BindSpecific(
		TSharedRef<DI::FDependencyBinding> SpecificBinding,
		EBindConflictBehavior ConflictBehavior)
	{
		FDependencyBindingId BindingId = SpecificBinding->GetId();
		if (Bindings.Contains(BindingId))
		{
			if (ConflictBehavior != EBindConflictBehavior::None)
			{
				const FString ErrorMessage = FString::Printf(
					TEXT("An instance for binding %s is already registered!"),
					*BindingId.ToString()
				);
				switch (ConflictBehavior)
				{
				case EBindConflictBehavior::LogWarning:
					UE_LOG(LogDependencyInjection, Warning, TEXT("%s"), *ErrorMessage)
					break;
				case EBindConflictBehavior::LogError:
					UE_LOG(LogDependencyInjection, Error, TEXT("%s"), *ErrorMessage)
					break;
				case EBindConflictBehavior::Ensure:
					ensureAlwaysMsgf(false, TEXT("%s"), *ErrorMessage);
					break;
				default:
				case EBindConflictBehavior::AssertCheck:
					checkf(false, TEXT("%s"), *ErrorMessage);
					break;
				}
			}
			return EBindResult::Conflict;
		}
		Bindings.Emplace(BindingId, SpecificBinding);
		Subscriptions.NotifyInstanceBound(BindingId, *SpecificBinding);
		return EBindResult::Bound;
	}
}

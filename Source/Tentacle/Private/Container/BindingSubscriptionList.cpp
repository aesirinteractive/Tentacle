// Copyright Manuel Wagner https://www.singinwhale.com


#include "Container/BindingSubscriptionList.h"

namespace Tentacle
{
	void FBindingSubscriptionList::NotifyInstanceBound(const FDependencyBindingId& BindingId, const Tentacle::FDependencyBinding& Binding)
	{
		FOnInstanceBound Subscriptions;
		if (!BindingToSubscriptions.RemoveAndCopyValue(BindingId, Subscriptions))
			return;

		Subscriptions.Broadcast(Binding);
	}

	auto FBindingSubscriptionList::Subscribe(const FDependencyBindingId& BindingId) -> FOnInstanceBound&
	{
		return BindingToSubscriptions.FindOrAdd(BindingId);
	}

	bool FBindingSubscriptionList::Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle)
	{
		FOnInstanceBound* Subscriptions = BindingToSubscriptions.Find(BindingId);
		if (!Subscriptions)
			return false;

		return Subscriptions->Remove(DelegateHandle);
	}
}

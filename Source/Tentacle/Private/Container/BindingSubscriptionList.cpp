// Copyright Manuel Wagner https://www.singinwhale.com


#include "Container/BindingSubscriptionList.h"

namespace DI
{
	void FBindingSubscriptionList::NotifyInstanceBound(const FBindingId& BindingId, const DI::FBinding& Binding)
	{
		FOnInstanceBound Subscriptions;
		if (!BindingToSubscriptions.RemoveAndCopyValue(BindingId, Subscriptions))
			return;

		Subscriptions.Broadcast(Binding);
	}

	auto FBindingSubscriptionList::SubscribeOnce(const FBindingId& BindingId) -> FOnInstanceBound&
	{
		return BindingToSubscriptions.FindOrAdd(BindingId);
	}

	bool FBindingSubscriptionList::Unsubscribe(const FBindingId& BindingId, FDelegateHandle DelegateHandle)
	{
		FOnInstanceBound* Subscriptions = BindingToSubscriptions.Find(BindingId);
		if (!Subscriptions)
			return false;

		return Subscriptions->Remove(DelegateHandle);
	}
}

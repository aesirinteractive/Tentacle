// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "Container/DiContainer.h"

namespace Tentacle
{
	bool FDiContainer::Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle)
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
}

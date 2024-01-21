// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "DiContainer.h"

void FDiContainer::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (auto& [BindingId, Binding] : Bindings)
	{
		Binding->AddReferencedObjects(Collector);
	}
}

// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "Container/DiContainerHandle.h"

void FScriptDiContainer::AddStructReferencedObjects(FReferenceCollector& Collector)
{
	DiContainer.AddReferencedObjects(Collector);
}

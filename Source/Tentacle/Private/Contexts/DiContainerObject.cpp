// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "Contexts/DiContainerObject.h"

void UDiContainerObject::AddReferencedObjects(UObject* Self, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Self, Collector);
	static_cast<UDiContainerObject*>(Self)->DiContainer->AddReferencedObjects(Collector);
}

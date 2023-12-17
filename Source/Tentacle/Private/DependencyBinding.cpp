
#include "DependencyBinding.h"

Tentacle::FDependencyBindingStorage::FDependencyBindingStorage(TFunction<void(void*)> InDestructorFunction): DestructorFunction(InDestructorFunction)
{
}

Tentacle::FDependencyBindingStorage::~FDependencyBindingStorage()
{
	if(DestructorFunction)
	{
		DestructorFunction(GetBindingPtr());
		DestructorFunction.Reset();
	}
}

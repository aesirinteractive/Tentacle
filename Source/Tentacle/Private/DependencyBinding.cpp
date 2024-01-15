#include "DependencyBinding.h"

Tentacle::FDependencyBindingStorage::~FDependencyBindingStorage()
{
	GetBinding().~FDependencyBinding();
}

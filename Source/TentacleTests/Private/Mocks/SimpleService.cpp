// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "SimpleService.h"

int32 USimpleInterfaceImplementation::GetA() const
{
	return A;
}

TENTACLE_DEFINE_FREE_NATIVE_TYPEID(FMockEngineType);
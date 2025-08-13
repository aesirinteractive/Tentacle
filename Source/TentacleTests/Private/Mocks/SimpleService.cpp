// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "SimpleService.h"

int32 USimpleInterfaceImplementation::GetA() const
{
	return A;
}

namespace DI
{
	namespace InjectTest
	{
		TObjectPtr<USimpleUService> InjectDependencies(TObjectPtr<USimpleUService> SimpleUService)
		{
			return SimpleUService;
		}
	}
}

DI_DEFINE_FREE_NATIVE_TYPEID(FMockEngineType);

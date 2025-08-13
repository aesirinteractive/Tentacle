#include "ExampleNative.h"

void FExampleNative::Initialize(TSharedPtr<FSimpleNativeService> SimpleNative)
{
	SimpleNativeService = SimpleNative;;
}

void FExampleNative::InitializeWithExtraArgs(TSharedPtr<FSimpleNativeService> SimpleNative, FString InExtraString)
{
	Initialize(SimpleNative);
	ExtraString = InExtraString;
}

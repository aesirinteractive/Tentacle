#pragma once

class FSimpleNativeService;

class TENTACLETESTS_API FExampleNative
{
public:
	void Initialize(TSharedPtr<FSimpleNativeService> SimpleNative);
	void InitializeWithExtraArgs(TSharedPtr<FSimpleNativeService> SimpleNative, FString InExtraString);

	TSharedPtr<FSimpleNativeService> SimpleNativeService;
	FString ExtraString;
};

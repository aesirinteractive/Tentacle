// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "DependencyBinding.h"
#include "DependencyBindingId.h"

namespace DI
{
	/**
	 * 
	 */
	class TENTACLE_API FBindingSubscriptionList
	{
	public:
		using FOnInstanceBound = TMulticastDelegate<void(const DI::FDependencyBinding&)>;
		using FOnInstanceBoundUnicast = FOnInstanceBound::FDelegate;

		bool Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle);

		void NotifyInstanceBound(const FDependencyBindingId& BindingId, const DI::FDependencyBinding& Binding);

		FOnInstanceBound& SubscribeOnce(const FDependencyBindingId& BindingId);

	private:
		TMap<FDependencyBindingId, FOnInstanceBound> BindingToSubscriptions = {};
	};
}

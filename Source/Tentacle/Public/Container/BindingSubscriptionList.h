// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "DependencyBinding.h"
#include "DependencyBindingId.h"

namespace Tentacle
{
	/**
	 * 
	 */
	class TENTACLE_API FBindingSubscriptionList
	{
	public:
		using FOnInstanceBound = TMulticastDelegate<void(const Tentacle::FDependencyBinding&)>;
		using FOnInstanceBoundUnicast = FOnInstanceBound::FDelegate;

		template <class TInstanceType>
		FDelegateHandle Subscribe(const FDependencyBindingId& BindingId, TDelegate<void(Tentacle::TBindingInstRef<TInstanceType>)> Delegate)
		{
			return Subscribe(BindingId).AddLambda([Delegate = MoveTemp(Delegate)](const Tentacle::FDependencyBinding& SafeBinding)
			{
				const auto& TypedBinding = static_cast<const Tentacle::TBindingType<TInstanceType>&>(SafeBinding);
				Delegate.ExecuteIfBound(Tentacle::ToRefType(TypedBinding.Resolve()));
			});
		}

		bool Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle);

	private:
		void NotifyInstanceBound(const FDependencyBindingId& BindingId, const Tentacle::FDependencyBinding& Binding);

		FOnInstanceBound& Subscribe(const FDependencyBindingId& BindingId);

	private:
		TMap<FDependencyBindingId, FOnInstanceBound> BindingToSubscriptions = {};
	};
}

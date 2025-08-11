// Copyright Aesir Interactive, GmbH. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Container/DependencyBinding.h"
#include "WeakFuture.h"
#include "DiContainerConcept.h"

namespace DI
{
	/**
	 * 
	 */
	template <class TDiContainer>
	class TENTACLE_API TResolveHelper
	{
	public:
		TResolveHelper(const TDiContainer& DiContainer): DiContainer(DiContainer)
		{
		}


		TObjectPtr<UObject> TryResolveUObjectByClass(UClass* ObjectType, FName BindingName) const
		{
			FDependencyBindingId BindingId = FDependencyBindingId(FTypeId(ObjectType), BindingName);
			return this->Resolve<UObject>(BindingId);
		}

		bool TryResolveUStruct(UScriptStruct* StructType,
		                       void* OutStructMemory,
		                       FName BindingName) const
		{
			FDependencyBindingId BindingId = FDependencyBindingId(FTypeId(StructType), BindingName);

			static_assert(
				TIsDerivedFrom<TBindingType<FHitResult>, DI::FRawDataBinding>::IsDerived,
				"This code assumes that UStruct bindings inherit from FRawDataBinding"
			);
			if (TSharedPtr<DI::FRawDataBinding> BindingInstance = StaticCastSharedPtr<DI::FRawDataBinding>(
				DiContainer.FindBinding(BindingId)
			))
			{
				BindingInstance->CopyRawData(OutStructMemory, StructType->GetStructureSize());
				return true;
			}
			return false;
		}

		template <class T>
		DI::TBindingInstPtr<T> TryResolveTypeInstance() const
		{
			FDependencyBindingId BindingId = MakeBindingId<T>();
			return this->Resolve<T>(BindingId);
		}

		template <class T>
		DI::TBindingInstPtr<T> TryResolveNamedInstance(const FName& BindingName) const
		{
			FDependencyBindingId BindingId = MakeBindingId<T>(BindingName);
			return this->Resolve<T>(BindingId);
		}

		template <class T>
		using TSubscriptionDelegateType = TDelegate<void(TBindingInstRef<T>)>;

		template <class T>
		TWeakFuture<TBindingInstRef<T>> TryResolveFutureTypeInstance(UObject* WaitingObject = nullptr)
		{
			return this->TryResolveFutureNamedInstance<T>(NAME_None, WaitingObject);
		}

		template <class TInstanceType>
		TWeakFuture<TBindingInstRef<TInstanceType>> TryResolveFutureNamedInstance(
			const FName& BindingName,
			UObject* WaitingObject = nullptr)
		{
			FDependencyBindingId BindingId = MakeBindingId<TInstanceType>(BindingName);
			auto [Promise, Future] = MakeWeakPromisePair<TBindingInstRef<TInstanceType>>();
			TBindingInstPtr<TInstanceType> MaybeInstance = Resolve<TInstanceType>(BindingId);
			if (MaybeInstance)
			{
				Promise.EmplaceValue(ToRefType(MaybeInstance));
			}
			else
			{
				DiContainer.Subscribe(BindingId).AddLambda(
					[Promise](const DI::FDependencyBinding& BindingInstance) mutable
					{
						const TBindingType<TInstanceType>& SpecificBinding = static_cast<const TBindingType<TInstanceType>&>(BindingInstance);
						TBindingInstRef<TInstanceType> Resolved = SpecificBinding.Resolve();
						Promise.EmplaceValue(Resolved);
					}
				);
			}
			return MoveTemp(Future);
		}

	private:
		/**
		 * Private so no one passes in a binding Id that does not match T
		 */
		template <class T>
		DI::TBindingInstPtr<T> Resolve(const FDependencyBindingId& BindingId) const
		{
			if (TSharedPtr<DI::FDependencyBinding> BindingInstance = DiContainer.FindBinding(BindingId))
			{
				return StaticCastSharedPtr<DI::TBindingType<T>>(BindingInstance)->Resolve();
			}
			return {};
		}

		const TDiContainer& DiContainer;
	};
}

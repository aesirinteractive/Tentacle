// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "Container/DependencyBinding.h"
#include "WeakFuture.h"
#include "DiContainerConcept.h"
#include "ResolveErrorBehavior.h"
#include "Tentacle.h"
#include "Blueprint/BlueprintExceptionInfo.h"

namespace DI
{
	/**
	 * 
	 */
	template <class TDiContainer>
	class TResolveHelper
	{
	public:
		TResolveHelper(const TDiContainer& DiContainer): DiContainer(DiContainer)
		{
		}

		TObjectPtr<UObject> TryResolveUObjectByClass(UClass* ObjectType,
		                                             FName BindingName,
		                                             EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			FDependencyBindingId BindingId = FDependencyBindingId(FTypeId(ObjectType), BindingName);
			return this->Resolve<UObject>(BindingId, ErrorBehavior);
		}

		bool TryResolveUStruct(UScriptStruct* StructType,
		                       void* OutStructMemory,
		                       FName BindingName,
		                       EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
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
			else
			{
				HandleResolveError(BindingId, ErrorBehavior);
			}
			return false;
		}

		template <class T>
		DI::TBindingInstPtr<T> TryResolveTypeInstance(EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			FDependencyBindingId BindingId = MakeBindingId<T>();
			return this->Resolve<T>(BindingId, ErrorBehavior);
		}

		template <class... Ts>
		TTuple<DI::TBindingInstPtr<Ts>...> TryResolveTypeInstances(EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			return MakeTuple(this->Resolve<Ts>(MakeBindingId<Ts>(), ErrorBehavior)...);
		}

		template <class T>
		DI::TBindingInstPtr<T> TryResolveNamedInstance(const FName& BindingName, EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			FDependencyBindingId BindingId = MakeBindingId<T>(BindingName);
			return this->Resolve<T>(BindingId, ErrorBehavior);
		}

		template <class T>
		using TSubscriptionDelegateType = TDelegate<void(TBindingInstRef<T>)>;

		template <class T>
		TWeakFuture<TBindingInstRef<T>> TryResolveFutureTypeInstance(UObject* WaitingObject = nullptr,
		                                                             EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			return this->TryResolveFutureNamedInstance<T>(NAME_None, WaitingObject, ErrorBehavior);
		}

		template <class... Ts>
		TWeakFutureSet<TOptional<TBindingInstRef<Ts>>...> TryResolveFutureTypeInstances(UObject* WaitingObject = nullptr,
		                                                                                EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior)
		const
		{
			TTuple<TWeakFuture<TBindingInstRef<Ts>>...> Futures = TTuple<TWeakFuture<TBindingInstRef<Ts>>...>(
				this->TryResolveFutureTypeInstance<Ts>(WaitingObject, ErrorBehavior)...
			);
			return AwaitAllWeak(MoveTemp(Futures));
		}

		template <class TInstanceType>
		TWeakFuture<TBindingInstRef<TInstanceType>> TryResolveFutureNamedInstance(const FName& BindingName,
		                                                                          UObject* WaitingObject = nullptr,
		                                                                          EResolveErrorBehavior ErrorBehavior = GDefaultResolveErrorBehavior) const
		{
			FDependencyBindingId BindingId = MakeBindingId<TInstanceType>(BindingName);
			auto [Promise, Future] = MakeWeakPromisePair<TBindingInstRef<TInstanceType>>();
			TBindingInstPtr<TInstanceType> MaybeInstance = Resolve<TInstanceType>(BindingId, EResolveErrorBehavior::ReturnNull);
			if (MaybeInstance)
			{
				Promise.EmplaceValue(ToRefType(MaybeInstance));
			}
			else
			{
				auto Callback = [Promise, ErrorBehavior](const DI::FDependencyBinding& BindingInstance) mutable
				{
					const TBindingType<TInstanceType>& SpecificBinding = static_cast<const TBindingType<TInstanceType>&>(BindingInstance);
					TBindingInstRef<TInstanceType> Resolved = SpecificBinding.Resolve();
					Promise.EmplaceValue(Resolved);
				};
				if (WaitingObject)
				{
					DiContainer.Subscribe(BindingId).AddWeakLambda(WaitingObject, Callback);
				}
				else
				{
					DiContainer.Subscribe(BindingId).AddLambda(Callback);
				}
			}
			return MoveTemp(Future);
		}

	private:
		/**
		 * Private so no one passes in a binding Id that does not match T
		 */
		template <class T>
		DI::TBindingInstPtr<T> Resolve(const FDependencyBindingId& BindingId, EResolveErrorBehavior ErrorBehavior) const
		{
			if (TSharedPtr<DI::FDependencyBinding> BindingInstance = DiContainer.FindBinding(BindingId))
			{
				return StaticCastSharedPtr<DI::TBindingType<T>>(BindingInstance)->Resolve();
			}
			HandleResolveError(BindingId, ErrorBehavior);
			return {};
		}

		const TDiContainer& DiContainer;
	};
}

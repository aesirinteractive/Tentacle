// Copyright Manuel Wagner https://www.singinwhale.com

#pragma once

#include "CoreMinimal.h"
#include "BindResult.h"
#include "BindConflictBehavior.h"
#include "DiContainerConcept.h"
#include "Binding.h"

namespace DI
{
	/**
	 * DiContainer agnostic implementation of common binding operations.
	 * This helps in keeping the number of functions to be implemented for a DiContainer type to be very minimal
	 * to allow variations without resorting to inheritance.
	 */
	template <class TDiContainer>
	class TBindingHelper
	{
	public:
		TBindingHelper(TDiContainer& DiContainer) : DiContainer(DiContainer)
		{
		}

		/**
		 * Binds an instance as its direct type
		 * Keep in mind that when resolving this type, that you need to use the same type as it has been bound with.
		 * Resolving via its parent class is not supported.
		 * If you need to resolve a binding by multiple types, you can bind it to all required types manually.
		 */
		template <class T>
		EBindResult BindInstance(DI::TBindingInstRef<T> Instance, EBindConflictBehavior ConflictBehavior = GDefaultConflictBehavior)
		{
			FBindingId BindingId = MakeBindingId<T>();
			return this->RegisterBinding<T>(BindingId, Instance, ConflictBehavior);
		}

		/**
		 * Binds an instance as a specific type
		 * Keep in mind that when resolving this type, that you need to use the same type as it has been bound with.
		 * Resolving via its parent class is not supported.
		 * If you need to resolve a binding by multiple types, you can bind it to all required types manually.
		 */
		template <class TBinding, class TInstance>
		EBindResult BindInstanceAs(DI::TBindingInstRef<TInstance> Instance, EBindConflictBehavior ConflictBehavior = GDefaultConflictBehavior)
		{
			return this->BindNamedInstanceAs<TBinding, TInstance>(NAME_None, Instance, ConflictBehavior);
		}

		/**
		 * Binds a named instance as its direct type
		 * Keep in mind that when resolving this type, that you need to use the same type as it has been bound with.
		 * Resolving via its parent class is not supported.
		 * If you need to resolve a binding by multiple types, you can bind it to all required types manually.
		 */
		template <class T>
		EBindResult BindNamedInstance(
			const FName& InstanceName,
			DI::TBindingInstRef<T> Instance,
			EBindConflictBehavior ConflictBehavior = GDefaultConflictBehavior)
		{
			FBindingId BindingId = MakeBindingId<T>(InstanceName);
			return this->RegisterBinding<T>(BindingId, Instance, ConflictBehavior);
		}

		/**
		 * Binds a named instance as a specific type
		 * Keep in mind that when resolving this type, that you need to use the same type as it has been bound with.
		 * Resolving via its parent class is not supported.
		 * If you need to resolve a binding by multiple types, you can bind it to all required types manually.
		 */
		template <class TBinding, class TInstance>
		EBindResult BindNamedInstanceAs(
			const FName& InstanceName,
			DI::TBindingInstRef<TInstance> Instance,
			EBindConflictBehavior ConflictBehavior = GDefaultConflictBehavior)
		{
			static_assert(TIsDerivedFrom<TInstance, TBinding>::Value, "Only derived classes are allowed");
			FBindingId BindingId = MakeBindingId<TBinding>(InstanceName);
			return this->RegisterBinding<TInstance>(BindingId, Instance, ConflictBehavior);
		}

	private:
		template <class T>
		TSharedPtr<DI::TBindingType<T>> FindBinding(const FBindingId& BindingId) const
		{
			if (const TSharedPtr<DI::FBinding> DependencyBinding = DiContainer.FindBinding(BindingId))
			{
				return StaticCastSharedPtr<DI::TBindingType<T>>(DependencyBinding);
			}
			return nullptr;
		}

		/**
		 * Private so no one passes in a binding Id that does not match T
		 */
		template <class T>
		EBindResult RegisterBinding(const FBindingId& BindingId,
		                            DI::TBindingInstRef<T> Instance,
		                            EBindConflictBehavior ConflictBehavior)
		{
			TSharedRef<DI::TBindingType<T>> ConcreteBinding = MakeShared<DI::TBindingType<T>>(BindingId, Instance);
			return DiContainer.BindSpecific(ConcreteBinding, ConflictBehavior);
		}

	private:
		TDiContainer& DiContainer;
	};
}

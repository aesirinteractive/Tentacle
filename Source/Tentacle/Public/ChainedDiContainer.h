// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiContainer.h"

namespace Tentacle
{
	/**
	 * 
	 */
	class TENTACLE_API FChainedDiContainer :
		public TSharedFromThis<FChainedDiContainer>
	{
	public:
		FChainedDiContainer() = default;
		virtual ~FChainedDiContainer() = default;

		void SetParentContainer(TWeakPtr<FChainedDiContainer> DiContainer);

		template <class T>
		void BindInstance(TBindingInstanceReferenceType<T> Instance)
		{
			MyDiContainer.BindInstance<T>(Instance);
		}

		template <class T>
		void BindNamedInstance(const FName& InstanceName, TBindingInstanceReferenceType<T> Instance)
		{
			MyDiContainer.BindNamedInstance<T>(InstanceName, Instance);
		}

		template <class T>
		TBindingInstanceNullableType<T> ResolveTypeInstance() const
		{
			if (TBindingInstanceNullableType<T> Instance = MyDiContainer.ResolveTypeInstance<T>())
			{
				return Instance;
			}

			const TSharedPtr<FChainedDiContainer> ChainedDiContainer = ParentContainer.Pin();
			if (!ChainedDiContainer)
				return {};

			return ChainedDiContainer->ResolveTypeInstance<T>();
		}

		template <class T>
		TBindingInstanceNullableType<T> ResolveNamedInstance(const FName& BindingName = NAME_None) const
		{
			if (TBindingInstanceNullableType<T> Instance = MyDiContainer.ResolveNamedInstance<T>(BindingName))
			{
				return Instance;
			}

			const TSharedPtr<FChainedDiContainer> ChainedDiContainer = ParentContainer.Pin();
			if (!ChainedDiContainer)
				return {};

			return ChainedDiContainer->ResolveNamedInstance<T>(BindingName);
		}

		void AddReferencedObjects(FReferenceCollector& Collector)
		{
			MyDiContainer.AddReferencedObjects(Collector);
		};

	private:
		FDiContainer& GetDiContainer() { return MyDiContainer; };
		const FDiContainer& GetDiContainer() const { return MyDiContainer; };

		FDiContainer MyDiContainer;

		TWeakPtr<FChainedDiContainer> ParentContainer;
	};
}

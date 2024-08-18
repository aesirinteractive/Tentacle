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
		void BindInstance(TBindingInstRef<T> Instance)
		{
			MyDiContainer.BindInstance<T>(Instance);
		}

		template <class T>
		void BindNamedInstance(const FName& InstanceName, TBindingInstRef<T> Instance)
		{
			MyDiContainer.BindNamedInstance<T>(InstanceName, Instance);
		}

		template <class T>
		TBindingInstOpt<T> ResolveTypeInstance() const
		{
			if (TBindingInstOpt<T> Instance = MyDiContainer.ResolveTypeInstance<T>())
			{
				return Instance;
			}

			const TSharedPtr<FChainedDiContainer> ChainedDiContainer = ParentContainer.Pin();
			if (!ChainedDiContainer)
				return {};

			return ChainedDiContainer->ResolveTypeInstance<T>();
		}

		template <class T>
		TBindingInstOpt<T> ResolveNamedInstance(const FName& BindingName = NAME_None) const
		{
			if (TBindingInstOpt<T> Instance = MyDiContainer.ResolveNamedInstance<T>(BindingName))
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

	namespace Tentacle
	{
		static_assert(TModels<CDiContainer, FChainedDiContainer, UObject>::Value);
	}
}

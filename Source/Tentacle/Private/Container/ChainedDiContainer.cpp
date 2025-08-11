// Fill out your copyright notice in the Description page of Project Settings.


#include "Container/ChainedDiContainer.h"

void DI::FChainedDiContainer::SetParentContainer(TWeakPtr<FChainedDiContainer> DiContainer)
{
	ParentContainer = DiContainer;
}

DI::EBindResult DI::FChainedDiContainer::BindSpecific(TSharedRef<FDependencyBinding> SpecificBinding, EBindConflictBehavior ConflictBehavior)
{
	EBindResult OverallResult = EBindResult::Bound;
	for (auto ChildrenContainerIt = ChildrenContainers.CreateIterator(); ChildrenContainerIt; ++ChildrenContainerIt)
	{
		TSharedPtr<FChainedDiContainer> ChainedDiContainer = ChildrenContainerIt->Pin();
		if (!ChainedDiContainer.IsValid())
		{
			ChildrenContainerIt.RemoveCurrent();
			continue;
		}

		const EBindResult BindResult = ChainedDiContainer->MyDiContainer.BindSpecific(SpecificBinding, ConflictBehavior);
		if (BindResult != EBindResult::Bound)
		{
			OverallResult = BindResult;
		}
	}
	return OverallResult;
}

TSharedPtr<DI::FDependencyBinding> DI::FChainedDiContainer::FindBinding(const FDependencyBindingId& BindingId) const
{
	if (TSharedPtr<FDependencyBinding> DependencyBinding = MyDiContainer.FindBinding(BindingId))
	{
		return DependencyBinding;
	}

	if (TSharedPtr<FChainedDiContainer> ParentDiContainer = ParentContainer.Pin())
	{
		return ParentDiContainer->FindBinding(BindingId);
	}
	return {};
}

DI::FBindingSubscriptionList::FOnInstanceBound& DI::FChainedDiContainer::Subscribe(
	const FDependencyBindingId& BindingId) const
{
	return MyDiContainer.Subscribe(BindingId);
}

bool DI::FChainedDiContainer::Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle) const
{
	return MyDiContainer.Unsubscribe(BindingId, DelegateHandle);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Container/ChainedDiContainer.h"

void DI::FChainedDiContainer::SetParentContainer(TWeakPtr<FChainedDiContainer> DiContainer)
{
	ParentContainer = DiContainer;
}

void DI::FChainedDiContainer::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (auto& [BindingId, Binding] : Bindings)
	{
		Binding->AddReferencedObjects(Collector);
	}
}

DI::EBindResult DI::FChainedDiContainer::BindSpecific(TSharedRef<FDependencyBinding> SpecificBinding, EBindConflictBehavior ConflictBehavior)
{
	EBindResult OverallResult = EBindResult::Bound;
	FDependencyBindingId BindingId = SpecificBinding->GetId();
	if (Bindings.Contains(BindingId))
	{
		HandleBindingConflict(BindingId, ConflictBehavior);
		return EBindResult::Conflict;
	}
	Bindings.Emplace(BindingId, SpecificBinding);
	for (auto ChildrenContainerIt = ChildrenContainers.CreateIterator(); ChildrenContainerIt; ++ChildrenContainerIt)
	{
		TSharedPtr<FChainedDiContainer> ChainedDiContainer = ChildrenContainerIt->Pin();
		if (!ChainedDiContainer.IsValid())
		{
			ChildrenContainerIt.RemoveCurrent();
			continue;
		}

		ChainedDiContainer->Subscriptions.NotifyInstanceBound(BindingId, *SpecificBinding);
	}
	return OverallResult;
}

TSharedPtr<DI::FDependencyBinding> DI::FChainedDiContainer::FindBinding(const FDependencyBindingId& BindingId) const
{
	if (const TSharedRef<FDependencyBinding>* DependencyBinding = Bindings.Find(BindingId))
	{
		return *DependencyBinding;
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
	return Subscriptions.SubscribeOnce(BindingId);
}

bool DI::FChainedDiContainer::Unsubscribe(const FDependencyBindingId& BindingId, FDelegateHandle DelegateHandle) const
{
	return Subscriptions.Unsubscribe(BindingId, DelegateHandle);
}

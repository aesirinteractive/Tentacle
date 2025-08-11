// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "BindingSubscriptionList.h"
#include "DependencyBinding.h"
#include "Templates/Models.h"
#include "TentacleTemplates.h"

namespace DI
{
	struct CTypeHasBindSpecific
	{
		template <class TDiContainer>
		auto Requires(TDiContainer& DiContainer,
		              TSharedRef<DI::FDependencyBinding> SpecificBinding,
		              EBindConflictBehavior ConflictBehavior) -> decltype(
			DiContainer.BindSpecific(SpecificBinding, ConflictBehavior)
		);
	};

	struct CTypeHasFindBinding
	{
		template <class TDiContainer>
		auto Requires(TDiContainer& DiContainer,
		              const FDependencyBindingId& BindingId) -> decltype(
			DiContainer.FindBinding(BindingId)
		);
	};

	struct CTypeHasSubscribe
	{
		template <class TDiContainer>
		auto Requires(const TDiContainer& DiContainer,
		              const FDependencyBindingId& BindingId) -> decltype(
			DiContainer.Subscribe(BindingId)
		);
	};

	struct CDiContainer
	{
		template <class TDiContainer>
		auto Requires(TDiContainer& DiContainer) -> decltype(
			Refines<CTypeHasBindSpecific, TDiContainer>(),
			Refines<CTypeHasFindBinding, TDiContainer>(),
			Refines<CTypeHasSubscribe, TDiContainer>()
		);
	};

	template <class T>
	concept DiContainerConcept = requires(T DiContainer)
	{
		{ DiContainer.BindSpecific(DeclVal<TSharedRef<DI::FDependencyBinding>>(), DeclVal<EBindConflictBehavior>()) } -> std::convertible_to<EBindResult>;
		{ DiContainer.FindBinding(DeclVal<const FDependencyBindingId&>()) } -> std::convertible_to<TSharedPtr<DI::FDependencyBinding>>;
		{ DiContainer.Subscribe(DeclVal<const FDependencyBindingId&>()) } -> std::convertible_to<FBindingSubscriptionList::FOnInstanceBound&>;
	};
}

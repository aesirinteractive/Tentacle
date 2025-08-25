﻿// Copyright singinwhale https://www.singinwhale.com and contributors. Distributed under the MIT license.

#pragma once

#include "BindingSubscriptionList.h"
#include "Binding.h"
#include "Templates/Models.h"
#include "TentacleTemplates.h"

namespace DI
{
	struct CTypeHasBindSpecific
	{
		template <class TDiContainer>
		auto Requires(TDiContainer& DiContainer,
		              TSharedRef<DI::FBinding> SpecificBinding,
		              EBindConflictBehavior ConflictBehavior) -> decltype(
			DiContainer.BindSpecific(SpecificBinding, ConflictBehavior)
		);
	};

	struct CTypeHasFindBinding
	{
		template <class TDiContainer>
		auto Requires(TDiContainer& DiContainer,
		              const FBindingId& BindingId) -> decltype(
			DiContainer.FindBinding(BindingId)
		);
	};

	struct CTypeHasSubscribe
	{
		template <class TDiContainer>
		auto Requires(const TDiContainer& DiContainer,
		              const FBindingId& BindingId) -> decltype(
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
		{ DiContainer.BindSpecific(DeclVal<TSharedRef<DI::FBinding>>(), DeclVal<EBindConflictBehavior>()) } -> std::convertible_to<EBindResult>;
		{ DiContainer.FindBinding(DeclVal<const FBindingId&>()) } -> std::convertible_to<TSharedPtr<DI::FBinding>>;
		{ DiContainer.Subscribe(DeclVal<const FBindingId&>()) } -> std::convertible_to<FBindingSubscriptionList::FOnInstanceBound&>;
	};
}

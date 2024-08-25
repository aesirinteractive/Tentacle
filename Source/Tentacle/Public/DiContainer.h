// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DependencyBinding.h"
#include "DependencyBindingId.h"
#include "TentacleTemplates.h"
#include "TypeId.h"


/**
 * 
 */
class TENTACLE_API FDiContainer
{
public:
	/** Binds an instance as its type */
	template <class T>
	void BindInstance(Tentacle::TBindingInstRef<T> Instance)
	{
		FDependencyBindingId BindingId = MakeBindingId<T>();
		RegisterBinding<T>(BindingId, Instance);
	}

	/** Binds a named instance as its type */
	template <class T>
	void BindNamedInstance(const FName& InstanceName, Tentacle::TBindingInstRef<T> Instance)
	{
		FDependencyBindingId BindingId = MakeBindingId<T>(InstanceName);
		RegisterBinding<T>(BindingId, Instance);
	}
	
	template <class T>
	Tentacle::TBindingInstOpt<T> ResolveTypeInstance() const
	{
		FDependencyBindingId BindingId = MakeBindingId<T>();
		return Resolve<T>(BindingId);
	}

	template <class T>
	Tentacle::TBindingInstOpt<T> ResolveNamedInstance(const FName& BindingName) const
	{
		FDependencyBindingId BindingId = MakeBindingId<T>(BindingName);
		return Resolve<T>(BindingId);
	}

	void AddReferencedObjects(FReferenceCollector& Collector);

protected:
	TMap<FDependencyBindingId, TSharedRef<Tentacle::FDependencyBinding>> Bindings = {};

private:
	template <class T>
	static FDependencyBindingId MakeBindingId()
	{
		return FDependencyBindingId(Tentacle::GetTypeId<T>());
	}

	template <class T>
	static FDependencyBindingId MakeBindingId(FName BindingName)
	{
		return FDependencyBindingId(Tentacle::GetTypeId<T>(), MoveTemp(BindingName));
	}

	template <class T>
	TSharedPtr<Tentacle::TBindingType<T>> FindBinding(const FDependencyBindingId& BindingId) const
	{
		if (const TSharedRef<Tentacle::FDependencyBinding>* DependencyBinding = Bindings.Find(BindingId))
		{
			return StaticCastSharedRef<Tentacle::TBindingType<T>>(*DependencyBinding);
		}
		return nullptr;
	}

	/**
	 * Private so no one passes in a binding Id that does not match T
	 */
	template <class T>
	Tentacle::TBindingInstOpt<T> Resolve(const FDependencyBindingId& BindingId) const
	{
		check(BindingId.GetBoundTypeId() == Tentacle::GetTypeId<T>())
		if (TSharedPtr<Tentacle::TBindingType<T>> BindingInstance = FindBinding<T>(BindingId))
		{
			return BindingInstance->Resolve();
		}
		return {};
	}

	/**
	 * Private so no one passes in a binding Id that does not match T
	 */
	template <class T>
	void RegisterBinding(const FDependencyBindingId& BindingId, Tentacle::TBindingInstRef<T> Instance)
	{
		ensureMsgf(!Bindings.Contains(BindingId), TEXT("An instance for this binding is already registered!"));
		TSharedRef<Tentacle::TBindingType<T>> ConcreteBinding = MakeShared<Tentacle::TBindingType<T>>(BindingId, Instance);
		Bindings.Emplace(BindingId, ConcreteBinding);
	}
};


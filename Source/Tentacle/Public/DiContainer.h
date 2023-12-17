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
	template <class T>
	void BindInstance(Tentacle::TBindingInstanceReferenceType<T> Instance)
	{
		FDependencyBindingId BindingId = MakeBindingId<T>();
		ensureMsgf(!Bindings.Contains(BindingId), TEXT("An instance for this binding is already registered!"));
		TSharedRef<Tentacle::FDependencyBindingStorage> BindingStorage = Tentacle::FDependencyBindingStorage::MakeBindingStorage<T>(BindingId, Instance);
		Bindings.Emplace(BindingId, BindingStorage);
	}

	template <class T>
	void BindNamedInstance(const FName& InstanceName, Tentacle::TBindingInstanceReferenceType<T> Instance)
	{
		FDependencyBindingId BindingId = MakeBindingId<T>(InstanceName);
		ensureMsgf(!Bindings.Contains(BindingId), TEXT("An instance for this binding is already registered!"));
		TSharedRef<Tentacle::FDependencyBindingStorage> BindingStorage = Tentacle::FDependencyBindingStorage::MakeBindingStorage<T>(BindingId, Instance);
		Bindings.Emplace(BindingId, BindingStorage);
	}


	template <class T>
	Tentacle::TBindingInstanceNullableType<T> Resolve() const
	{
		FDependencyBindingId BindingId = MakeBindingId<T>();
		if (const TSharedRef<Tentacle::FDependencyBindingStorage>* DependencyBinding = Bindings.Find(BindingId))
		{
			return DependencyBinding->Get().Resolve<T>();
		}
		return {};
	}

	template <class T>
	Tentacle::TBindingInstanceNullableType<T> Resolve(const FName& BindingName) const
	{
		FDependencyBindingId BindingId = MakeBindingId<T>(BindingName);
		if (const TSharedRef<Tentacle::FDependencyBindingStorage>* DependencyBinding = Bindings.Find(BindingId))
		{
			return DependencyBinding->Get().Resolve<T>();
		}
		return {};
	}

protected:
	TMap<FDependencyBindingId, TSharedRef<Tentacle::FDependencyBindingStorage>> Bindings = {};

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
};

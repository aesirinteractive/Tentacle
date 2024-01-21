#pragma once
#include "DependencyBindingId.h"
#include "TypeId.h"

namespace Tentacle
{
	class FDependencyBinding
	{
	public:
		FDependencyBinding(FDependencyBindingId BindingId)
			: Id(MoveTemp(BindingId))
		{
		}

		virtual ~FDependencyBinding() = default;

		FORCEINLINE FDependencyBindingId GetId() const
		{
			return Id;
		}

		virtual void AddReferencedObjects(FReferenceCollector& Collector)
		{
		}

	private:
		FDependencyBindingId Id;
	};


	template <class T>
	class TUObjectDependencyBinding final : public FDependencyBinding
	{
	public:
		using Super = FDependencyBinding;

		TObjectPtr<T> UObjectDependency;

		TUObjectDependencyBinding(FDependencyBindingId BindingId, TObjectPtr<T> InObject)
			: Super(BindingId)
			, UObjectDependency(InObject)
		{
		}

		TObjectPtr<T> Resolve() const
		{
			return UObjectDependency;
		}

		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			Collector.AddReferencedObject(UObjectDependency);
		}
	};


	template <class T>
	class TUInterfaceDependencyBinding final : public FDependencyBinding
	{
	public:
		using Super = FDependencyBinding;

		TScriptInterface<T> InterfaceDependency;

		TUInterfaceDependencyBinding(FDependencyBindingId BindingId, const TScriptInterface<T>& InInterface)
			: Super(BindingId)
			, InterfaceDependency(InInterface)
		{
		}

		const TScriptInterface<T>& Resolve() const
		{
			return InterfaceDependency;
		}
	};

	template <class T>
	class TSharedNativeDependencyBinding final : public FDependencyBinding
	{
	public:
		using Super = FDependencyBinding;

		TSharedRef<T> SharedNativeDependency;

		TSharedNativeDependencyBinding(FDependencyBindingId BindingId, TSharedRef<T> InSharedInstance)
			: Super(BindingId)
			, SharedNativeDependency(InSharedInstance)
		{
		}

		TSharedRef<T> Resolve() const
		{
			return SharedNativeDependency;
		}
	};


	template <class T>
	class TCopiedDependencyBinding final : public FDependencyBinding
	{
	public:
		using Super = FDependencyBinding;

		T CopyOfDependency;

		TCopiedDependencyBinding(FDependencyBindingId BindingId, TOptional<T> InInstance)
			: Super(BindingId)
			, CopyOfDependency(MoveTemp(*InInstance))
		{
		}

		const T& Resolve() const
		{
			return CopyOfDependency;
		}
	};

	template <class T>
	using TBindingType = Tentacle::TBindingInstanceTypeSwitch<T,
		TUObjectDependencyBinding<T>,
		TUInterfaceDependencyBinding<T>,
		TCopiedDependencyBinding<T>,
		TSharedNativeDependencyBinding<T>>;
	
}

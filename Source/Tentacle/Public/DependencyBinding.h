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

	/**
	 * Holds a dependency binding as well as the SharedPtr block in a single block of memory in a typesafe and construct/destruct safe way.
	 */
	class TENTACLE_API FDependencyBindingStorage : public TSharedFromThis<FDependencyBindingStorage>
	{
	public:
		~FDependencyBindingStorage();

		template <class T>
		Tentacle::TBindingInstanceNullableType<T> Resolve() const
		{
			return static_cast<const TBindingType<T>*>(GetBindingPtr())->Resolve();
		}

		FDependencyBinding& GetBinding() { return *static_cast<FDependencyBinding*>(GetBindingPtr()); };
		const FDependencyBinding& GetBinding() const { return *static_cast<const FDependencyBinding*>(GetBindingPtr()); };


		template <class T>
		static TSharedRef<FDependencyBindingStorage> MakeBindingStorage(FDependencyBindingId Id, Tentacle::TBindingInstanceReferenceType<T> Instance)
		{
			constexpr size_t RequiredSize = sizeof(FDependencyBindingStorage) + sizeof(TBindingType<T>);
			uint8* StoragePtr = new uint8[RequiredSize];
			checkf(StoragePtr != nullptr, TEXT("Out of memory"));
			FDependencyBindingStorage* Storage = new(StoragePtr) FDependencyBindingStorage();

			TBindingType<T>* Binding = new(Storage->GetBindingPtr()) TBindingType<T>(Id, Instance);

			return TSharedRef<FDependencyBindingStorage>(Storage);
		}

	private:
		FDependencyBindingStorage() = default;

		FORCEINLINE void* GetBindingPtr()
		{
			return static_cast<void*>(this + 1);
		}

		FORCEINLINE const void* GetBindingPtr() const
		{
			return static_cast<const void*>(this + 1);
		}
	};
};

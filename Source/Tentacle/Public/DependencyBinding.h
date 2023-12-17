#pragma once
#include "DependencyBindingId.h"
#include "TypeId.h"

namespace Tentacle
{
	template <class T>
	class TDependencyBinding
	{
	public:
		TDependencyBinding(FDependencyBindingId BindingId)
			: Id(MoveTemp(BindingId))
		{
		}

		FORCEINLINE FDependencyBindingId GetId() const
		{
			return Id;
		}

	private:
		FDependencyBindingId Id;
	};


	template <class T>
	class TUObjectDependencyBinding : public TDependencyBinding<T>
	{
	public:
		using Super = TDependencyBinding<T>;

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
	};


	template <class T>
	class TUInterfaceDependencyBinding : public TDependencyBinding<T>
	{
	public:
		using Super = TDependencyBinding<T>;

		TScriptInterface<T> InterfaceDependency;

		TUInterfaceDependencyBinding(FDependencyBindingId BindingId, T& InInterface)
			: Super(BindingId)
			, InterfaceDependency(&InInterface)
		{
		}

		T* Resolve() const
		{
			return InterfaceDependency.GetInterface();
		}
	};

	template <class T>
	class TSharedNativeDependencyBinding : public TDependencyBinding<T>
	{
	public:
		using Super = TDependencyBinding<T>;

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
	class TCopiedDependencyBinding : public TDependencyBinding<T>
	{
	public:
		using Super = TDependencyBinding<T>;

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

	class TENTACLE_API FDependencyBindingStorage : public TSharedFromThis<FDependencyBindingStorage, ESPMode::ThreadSafe>
	{
	public:
		~FDependencyBindingStorage();

		template <class T>
		Tentacle::TBindingInstanceNullableType<T> Resolve() const
		{
			return static_cast<const TBindingType<T>*>(GetBindingPtr())->Resolve();
		}


		template <class T>
		static TSharedRef<FDependencyBindingStorage> MakeBindingStorage(FDependencyBindingId Id, Tentacle::TBindingInstanceReferenceType<T> Instance)
		{
			constexpr size_t RequiredSize = sizeof(FDependencyBindingStorage) + sizeof(TBindingType<T>);
			uint8* StoragePtr = new uint8[RequiredSize];
			checkf(StoragePtr != nullptr, TEXT("Out of memory"));
			FDependencyBindingStorage* Storage = new(StoragePtr) FDependencyBindingStorage([](void* InstancePtr)
			{
				if (!TIsTriviallyDestructible<T>::Value)
				{
					static_cast<TBindingType<T>*>(InstancePtr)->~TBindingType<T>();
				}
			});

			TBindingType<T>* Binding = new(Storage->GetBindingPtr()) TBindingType<T>(Id, Instance);

			return TSharedRef<FDependencyBindingStorage>(Storage);
		}

	private:
		TFunction<void(void*)> DestructorFunction;

		FDependencyBindingStorage(TFunction<void(void*)> InDestructorFunction);

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

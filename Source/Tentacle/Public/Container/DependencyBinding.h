#pragma once
#include "DependencyBindingId.h"
#include "TypeId.h"

namespace DI
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
			: Super(BindingId), UObjectDependency(MoveTemp(InObject))
		{
			static_assert(TIsDerivedFrom<T, UObject>::IsDerived);
			checkf(InObject.GetClass()->IsChildOf(static_cast<UClass*>(BindingId.GetBoundTypeId().TryGetUType())), TEXT("%s is not derived from %s"), *InObject.GetClass()->GetName(), *BindingId.GetBoundTypeId().TryGetUType()->GetName());
		}

		TObjectPtr<T> Resolve() const
		{
			check(UObjectDependency);
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
			: Super(BindingId), InterfaceDependency(InInterface)
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
			: Super(BindingId), SharedNativeDependency(InSharedInstance)
		{
		}

		TSharedRef<T> Resolve() const
		{
			return SharedNativeDependency;
		}
	};


	class FRawDataBinding : public FDependencyBinding
	{
	public:
		using Super = FDependencyBinding;

		FRawDataBinding(FDependencyBindingId BindingId)
			: Super(BindingId)
		{
		}

		virtual void CopyRawData(void* OutData, int32 SizeOfOutData) = 0;
	};


	template <class T>
	class TCopiedDependencyBinding final : public FRawDataBinding
	{
	public:
		using Super = FRawDataBinding;

		T CopyOfDependency;

		TCopiedDependencyBinding(FDependencyBindingId BindingId, TOptional<T> InInstance)
			: Super(BindingId), CopyOfDependency(MoveTemp(*InInstance))
		{
		}

		const T& Resolve() const
		{
			return CopyOfDependency;
		}

		virtual void CopyRawData(void* OutData, int32 OutDataSize) override
		{
			UScriptStruct* StructClass = T::StaticStruct();
			check(StructClass->GetStructureSize() == OutDataSize);
			StructClass->CopyScriptStruct(OutData, &CopyOfDependency, 1);
		};
	};

	template <class T>
	using TBindingType = DI::TBindingInstanceTypeSwitch<
		T,
		TUObjectDependencyBinding<T>, // UObject
		TUInterfaceDependencyBinding<T>, // IInterface
		TCopiedDependencyBinding<T>, // UStruct
		TSharedNativeDependencyBinding<T>>; // Native
}

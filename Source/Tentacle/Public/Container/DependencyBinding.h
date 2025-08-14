#pragma once
#include "DependencyBindingId.h"
#include "TypeId.h"
#include "StructUtils/InstancedStruct.h"

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
			GetId().AddReferencedObjects(Collector);
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
			checkf(
				InObject.GetClass()->IsChildOf(static_cast<UClass*>(BindingId.GetBoundTypeId().TryGetUType())),
				TEXT("%s is not derived from %s"),
				*InObject.GetClass()->GetName(),
				*BindingId.GetBoundTypeId().TryGetUType()->GetName()
			);
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


	class FUStructBinding : public FRawDataBinding
	{
	public:
		using Super = FRawDataBinding;

		FUStructBinding(UScriptStruct* StructType, FName BindingName, const uint8* StructMemoryToCopy)
			: Super(FDependencyBindingId(FTypeId(StructType), BindingName))
		{
			StructData.InitializeAs(StructType, StructMemoryToCopy);
		}

		const UScriptStruct* GetStruct()
		{
			return StructData.GetScriptStruct();
		}

		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			Super::AddReferencedObjects(Collector);
			StructData.AddStructReferencedObjects(Collector);
		}

		virtual void CopyRawData(void* OutData, int32 OutDataSize) override
		{
			const UScriptStruct* StructClass = GetStruct();
			check(StructClass->GetStructureSize() <= OutDataSize);
			StructClass->CopyScriptStruct(OutData, StructData.GetMemory(), 1);
		};

	protected:
		/**
		 * Manages the inner struct data.
		 * We could inline the data to avoid the secondary allocation, but we would basically reimplement FInstancedStruct.
		 */
		FInstancedStruct StructData;
	};


	template <class T>
	class TTypedStructBinding final : public FUStructBinding
	{
	public:
		using Super = FUStructBinding;

		TTypedStructBinding(FDependencyBindingId BindingId, const T& InInstance)
			: Super(T::StaticStruct(), BindingId.GetBindingName(), reinterpret_cast<const uint8*>(&InInstance))
		{
			check(T::StaticStruct() == BindingId.GetBoundTypeId().TryGetUType());
		}

		const T& Resolve() const
		{
			return StructData.Get<T>();
		}
	};


	template <class T>
	using TBindingType = DI::TBindingInstanceTypeSwitch<
		T,
		TUObjectDependencyBinding<T>, // UObject
		TUInterfaceDependencyBinding<T>, // IInterface
		TTypedStructBinding<T>, // UStruct
		TSharedNativeDependencyBinding<T>>; // Native
}

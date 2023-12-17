// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TentacleTemplates.h"
#include "UObject/Object.h"

class FTypeId;

uint32 GetTypeHash(const FTypeId&);

namespace Tentacle
{
	namespace Private
	{
		FTypeId MakeNativeTypeId(const TCHAR* ClassName);
	}
}

/**
 * 
 */
class TENTACLE_API FTypeId
{
	friend FTypeId Tentacle::Private::MakeNativeTypeId(const TCHAR* ClassName);
	friend uint32 GetTypeHash(const FTypeId&);

private:
	enum class EIdType : uint8
	{
		Invalid = 255,
		UType = 0,
		NativeType = 1,
	};

	struct FId
	{
		EIdType Type = EIdType::Invalid;
		union
		{
			const UStruct* UType;
			const TCHAR* NativeClassId;
		};
	};

	const FId Id = {};

	explicit FTypeId(const TCHAR* ClassId)
		: Id({EIdType::NativeType, {.NativeClassId = ClassId}})
	{
	}

public:
	FTypeId() = default;

	explicit FTypeId(const UStruct* TypeClass)
		: Id({EIdType::UType, {.UType = TypeClass}})
	{
	}

	FName GetName() const;

	const UStruct* TryGetUType()
	{
		if (Id.Type == EIdType::UType)
		{
			return Id.UType;
		}
		return nullptr;
	}

	const void* GetTypeIdAddress() const
	{
		switch (Id.Type)
		{
		case EIdType::Invalid:
		default:
			return nullptr;
		case EIdType::UType:
			return Id.UType;
		case EIdType::NativeType:
			return Id.NativeClassId;
		}
	}

	FORCEINLINE bool operator==(const FTypeId& Other) const
	{
		return
			Id.Type == Other.Id.Type
			&& GetTypeIdAddress() == Other.GetTypeIdAddress();
	}
};

FORCEINLINE uint32 GetTypeHash(const FTypeId& TypeId)
{
	return HashCombine(GetTypeHash(TypeId.Id.Type), GetTypeHash(TypeId.GetTypeIdAddress()));
}


#define TENTACLE_DEFINE_NATIVE_TYPEID(ClassName)\
		FORCEINLINE static const FTypeId& GetTypeId()\
		{ \
			static_assert(sizeof(ClassName) != 0, #ClassName " does not name a type.");\
			static const TCHAR* ClassName ## TypeName = TEXT(PREPROCESSOR_TO_STRING(ClassName)); \
			static const FTypeId ClassName ## TypeId = Tentacle::Private::MakeNativeTypeId(ClassName ## TypeName);\
			return ClassName ## TypeId; \
		}

namespace Tentacle
{
	template <class T>
	typename TEnableIf<THasUStruct<T>::Value, FTypeId>::Type
	GetTypeId() /* -> FTypeId */
	{
		return FTypeId(GetStaticClass<T>());
	}

	template <class T>
	typename TEnableIf<!THasUStruct<T>::Value, FTypeId>::Type
	GetTypeId() /* -> FTypeId */
	{
		return T::GetTypeId();
	}

	struct CNativeTypeIdProvider
	{
		template <typename T>
		auto Requires(FTypeId& OutTypeId) -> decltype(
			OutTypeId = T::GetTypeId()
		);
	};

	namespace Private
	{
		FORCEINLINE FTypeId MakeNativeTypeId(const TCHAR* ClassName)
		{
			return FTypeId(ClassName);
		}
	}
}

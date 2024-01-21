// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TentacleTemplates.h"
#include "UObject/Object.h"
#include <concepts>

class FTypeId;

uint32 GetTypeHash(const FTypeId&);

namespace Tentacle
{
	namespace Private
	{
		FTypeId MakeNativeTypeId(const TCHAR* TypeName);
	}
}

/**
 * RTTI-like type ID that uses either UObject information or manually defined type names for native types.
 * You can make a typeID for a type by using Tentacle::GetTypeId<T>();
 * Example:
 *	class MyType {
 *		TENTACLE_DEFINE_NATIVE_TYPEID(MyType)
 *	}
 *
 *	void Main(){
 *		FTypeId MyTypeId = Tentacle::GetTypeId<MyType>();
 *	}
 */
class TENTACLE_API FTypeId
{
	friend FTypeId Tentacle::Private::MakeNativeTypeId(const TCHAR* TypeName);
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

/**
 * Use this to define a typeID inside a type.
 * Use for types that you wrote yourself and can edit the source files.
 * @param TypeName Type of the class. Can include namespace declarations.
 */
#define TENTACLE_DEFINE_NATIVE_TYPEID_MEMBER(TypeName)\
		FORCEINLINE static FTypeId GetTypeId()\
		{ \
			static_assert(sizeof(TypeName) != 0, #TypeName " does not name a type.");\
			static const TCHAR* TypeName ## TypeName = TEXT(PREPROCESSOR_TO_STRING(TypeName)); \
			static const FTypeId TypeName ## TypeId = Tentacle::Private::MakeNativeTypeId(TypeName ## TypeName);\
			return TypeName ## TypeId; \
		}

/**
 * Use this to define a typeID outside the type.
 * Use for types that are defined in foreign code where you can't define the typeID as a member function.
 * @param TypeName Type of the class. Can include namespace declarations.
 */
#define TENTACLE_DEFINE_FREE_NATIVE_TYPEID(TypeName)\
	namespace Tentacle {\
		template<>\
		const FTypeId& GetFreeTypeId<TypeName>()\
		{ \
			static_assert(sizeof(TypeName) != 0, #TypeName " does not name a type.");\
			static const TCHAR* TypeName ## TypeName = TEXT(PREPROCESSOR_TO_STRING(TypeName)); \
			static const FTypeId TypeName ## TypeId = Tentacle::Private::MakeNativeTypeId(TypeName ## TypeName);\
			return TypeName ## TypeId; \
		}\
	}
			

namespace Tentacle
{
	struct CNativeMemberTypeIdProvider
	{
		template <typename T>
		auto Requires() -> decltype(
			FTypeId(T::GetTypeId())
		);
	};

	template <class T>
	FTypeId GetFreeTypeId();

	template <class T>
	typename TEnableIf<THasUStruct<T>::Value, FTypeId>::Type
	GetTypeId() /* -> FTypeId */
	{
		return FTypeId(GetStaticClass<T>());
	}

	template <class T>
	typename TEnableIf<TModels<CNativeMemberTypeIdProvider, T>::Value, FTypeId>::Type
	GetTypeId() /* -> FTypeId */
	{
		return T::GetTypeId();
	}

	template <class T>
	typename TEnableIf<!TOr<THasUStruct<T>,TModels<CNativeMemberTypeIdProvider, T>>::Value, FTypeId>::Type
	GetTypeId() /* -> FTypeId */
	{
		return ::Tentacle::GetFreeTypeId<T>();
	}

	namespace Private
	{
		FORCEINLINE FTypeId MakeNativeTypeId(const TCHAR* TypeName)
		{
			return FTypeId(TypeName);
		}
	}
}

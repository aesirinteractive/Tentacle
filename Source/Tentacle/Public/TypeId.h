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

		//@see https://stackoverflow.com/a/38637849
		template<typename ... Ts>
		struct TAlwaysFalse : std::false_type {};
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

	EIdType Type = EIdType::Invalid;
	union
	{
		const UStruct* UType;
		const TCHAR* NativeClassId;
	};

	explicit FTypeId(const TCHAR* ClassId)
		: Type(EIdType::NativeType), NativeClassId(ClassId)
	{
	}

public:
	FTypeId() = default;

	explicit FTypeId(const UStruct* TypeClass)
		: Type(EIdType::NativeType), UType(TypeClass)
	{
	}

	FName GetName() const;

	const UStruct* TryGetUType()
	{
		if (Type == EIdType::UType)
		{
			return UType;
		}
		return nullptr;
	}

	const void* GetTypeIdAddress() const
	{
		switch (Type)
		{
			case EIdType::Invalid:
			default:
				return nullptr;
			case EIdType::UType:
				return UType;
			case EIdType::NativeType:
				return NativeClassId;
		}
	}

	FORCEINLINE bool operator==(const FTypeId& Other) const
	{
		return
			Type == Other.Type
			&& GetTypeIdAddress() == Other.GetTypeIdAddress();
	}
};

FORCEINLINE uint32 GetTypeHash(const FTypeId& TypeId)
{
	return HashCombine(GetTypeHash(TypeId.Type), GetTypeHash(TypeId.GetTypeIdAddress()));
}

#define TENTACLE_TYPEID_BODY(TypeName)\
	static_assert(sizeof(TypeName) != 0, #TypeName " does not name a type.");\
	static const TCHAR* TypeName ## TypeName = TEXT(PREPROCESSOR_TO_STRING(TypeName)); \
	static const FTypeId TypeName ## TypeId = Tentacle::Private::MakeNativeTypeId(TypeName ## TypeName);\
	return TypeName ## TypeId; 

/**
 * Use this to define a typeID inside a type.
 * Use for types that you wrote yourself and where you can edit the source files.
 * @param TypeName Type of the class. Can include namespace declarations.
 */
#define TENTACLE_DEFINE_NATIVE_TYPEID_MEMBER(TypeName)\
		FORCEINLINE static const FTypeId& GetTypeId()\
		{ \
			TENTACLE_TYPEID_BODY(TypeName)\
		}


#define TENTACLE_DECLARE_FREE_NATIVE_TYPEID(API_MACRO, TypeName)\
	namespace Tentacle {\
		template<>\
		const FTypeId& GetFreeTypeId<TypeName>();\
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
			TENTACLE_TYPEID_BODY(TypeName)\
		}\
	}
			

namespace Tentacle
{
	/**
	 * Concept for types that have a member-style typeID provider
	 * i.e. the ones that use TENTACLE_DEFINE_NATIVE_TYPEID_MEMBERa
	 */
	struct CNativeMemberTypeIdProvider
	{
		template <typename T>
		auto Requires() -> decltype(
			FTypeId(T::GetTypeId())
		);
	};

	template <class T>
	const FTypeId& GetFreeTypeId()
	{
		static_assert(Private::TAlwaysFalse<T>::value, "Your type does not provide a Type Use `TENTACLE_DEFINE_NATIVE_TYPEID_MEMBER(YourClass)` in your class"
		" or use `TENTACLE_DEFINE_FREE_NATIVE_TYPEID(SomeClass)` to define the type ID for a foreign type.");
		return {};
	}

	template <class T>
	typename TEnableIf<THasUStruct<T>::Value, const FTypeId&>::Type
	GetTypeId() /* -> FTypeId */
	{
		static const FTypeId StaticUStructId = FTypeId(GetStaticClass<T>());
		return StaticUStructId;
	}

	template <class T>
	typename TEnableIf<TModels<CNativeMemberTypeIdProvider, T>::Value, const FTypeId&>::Type
	GetTypeId() /* -> FTypeId */
	{
		return T::GetTypeId();
	}

	template <class T>
	typename TEnableIf<!TOr<THasUStruct<T>,TModels<CNativeMemberTypeIdProvider, T>>::Value, const FTypeId&>::Type
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

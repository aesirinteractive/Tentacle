// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "TypeId.h"

FName FTypeId::GetName() const
{
	switch (Type)
	{
	case EIdType::Invalid:
	default:
		return TEXT("Invalid");
	case EIdType::UType:
		return UType->GetFName();
		break;
	case EIdType::NativeType:
		return NativeClassId;
	}
}

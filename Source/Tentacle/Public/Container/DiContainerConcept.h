// TODO COpyright

#include "Templates/Models.h"

namespace Tentacle
{
	struct CTypeBinder
	{
		template <class TDiContainer, class TSomeOtherType>
		auto Requires(TDiContainer& DiContainer, TBindingInstanceReferenceType<TSomeOtherType> OtherTypeInstance) -> decltype(
			DiContainer.template BindInstance<TSomeOtherType>(OtherTypeInstance)
		);
	};

	struct CNamedTypeBinder
	{
		template <class TDiContainer, class TSomeOtherType>
		auto Requires(TDiContainer& DiContainer, TBindingInstanceReferenceType<TSomeOtherType> OtherTypeInstance) -> decltype(
			DiContainer.template BindNamedInstance<TSomeOtherType>(DeclVal<FName>(), OtherTypeInstance)
		);
	};

	struct CTypeResolver
	{
		template <class TDiContainer, class TSomeOtherType>
		auto Requires(TDiContainer& DiContainer) -> decltype(
			DiContainer.template ResolveTypeInstance<TSomeOtherType>()
		);
	};

	struct CNamedTypeResolver
	{
		template <class TDiContainer, class TSomeOtherType>
		auto Requires(TDiContainer& DiContainer) -> decltype(
			DiContainer.template ResolveNamedInstance<TSomeOtherType>(DeclVal<FName>())
		);
	};


	struct CDependencyResolver
	{
		template <class TDiContainer, class TSomeOtherType>
		auto Requires(TDiContainer& DiContainer) -> decltype(
			Refines<CTypeResolver, TDiContainer, TSomeOtherType>(),
			Refines<CNamedTypeResolver, TDiContainer, TSomeOtherType>()
		);
	};

	struct CDependencyBinder
	{
		template <class TDiContainer, class TSomeOtherType>
		auto Requires(TDiContainer& DiContainer) -> decltype(
			Refines<CTypeBinder, TDiContainer, TSomeOtherType>(),
			Refines<CNamedTypeBinder, TDiContainer, TSomeOtherType>()
		);
	};

	struct CDiContainer
	{
		template <class TDiContainer, class TSomeOtherType>
		auto Requires(TDiContainer& DiContainer) -> decltype(
			Refines<CDependencyResolver, TDiContainer, TSomeOtherType>(),
			Refines<CDependencyBinder, TDiContainer, TSomeOtherType>()
		);
	};
}

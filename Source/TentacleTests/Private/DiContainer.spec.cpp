// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "DiContainer.h"
#include "Mocks\SimpleService.h"

BEGIN_DEFINE_SPEC(DiContainerSpec, "Tentacle.DiContainer",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

	FDiContainer DiContainer;
END_DEFINE_SPEC(DiContainerSpec)

void DiContainerSpec::Define()
{
	BeforeEach([this]
	{
		DiContainer = FDiContainer();
	});
	Describe("Constructor", [this]
	{
	});
	Describe("BindInstance", [this]
	{
		It("should bind UObjects", [this]
		{
			const TObjectPtr<USimpleUService> Service = NewObject<USimpleUService>();
			DiContainer.BindInstance<USimpleUService>(*Service);
			TestEqual("DiContainer.Resolve<USimpleUService>()", DiContainer.Resolve<USimpleUService>(), Service);
		});
		It("should bind UInterfaces", [this]
		{
			const TObjectPtr<USimpleInterfaceImplementation> Service = NewObject<USimpleInterfaceImplementation>();
			DiContainer.BindInstance<ISimpleInterface>(*Service);
			const ISimpleInterface* ResolvedInterface = DiContainer.Resolve<ISimpleInterface>();
			TestSame("DiContainer.Resolve<ISimpleInterface>()", *ResolvedInterface, static_cast<ISimpleInterface&>(*Service));
		});
		It("should bind native classes", [this]
		{
			const TSharedRef<FSimpleNativeService> Service = MakeShared<FSimpleNativeService>(20);
			DiContainer.BindInstance<FSimpleNativeService>(Service);
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve<FSimpleNativeService>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should bind ustructs", [this]
		{
			const FSimpleUStructService Service = FSimpleUStructService{20};
			DiContainer.BindInstance<FSimpleUStructService>(Service);
			TOptional<FSimpleUStructService> Resolved = DiContainer.Resolve<FSimpleUStructService>();
			if (TestTrue("Resolved.IsSet()", Resolved.IsSet()))
			{
				TestEqual("Resolved->A", Resolved->A, 20);
			}
		});
	});

	Describe("Resolve", [this]
	{
		BeforeEach([this]
		{
			const char* InstanceName = "SomeName";

			TObjectPtr<USimpleUService> UService = NewObject<USimpleUService>();
			UService->A = 20;
			DiContainer.BindInstance<USimpleUService>(*UService);
			UService = NewObject<USimpleUService>();
			UService->A = 22;
			DiContainer.BindNamedInstance<USimpleUService>(InstanceName, *UService);

			TObjectPtr<USimpleInterfaceImplementation> SimpleInterfaceImplementation = NewObject<USimpleInterfaceImplementation>();
			SimpleInterfaceImplementation->A = 20;
			DiContainer.BindInstance<ISimpleInterface>(*SimpleInterfaceImplementation);
			SimpleInterfaceImplementation = NewObject<USimpleInterfaceImplementation>();
			SimpleInterfaceImplementation->A = 22;
			DiContainer.BindNamedInstance<ISimpleInterface>(InstanceName, *SimpleInterfaceImplementation);

			TSharedRef<FSimpleNativeService> NativeService = MakeShared<FSimpleNativeService>(20);
			DiContainer.BindInstance<FSimpleNativeService>(NativeService);
			NativeService = MakeShared<FSimpleNativeService>(22);
			DiContainer.BindNamedInstance<FSimpleNativeService>(InstanceName, NativeService);

			FSimpleUStructService StructService = FSimpleUStructService{20};
			DiContainer.BindInstance<FSimpleUStructService>(StructService);
			StructService = FSimpleUStructService{22};
			DiContainer.BindNamedInstance<FSimpleUStructService>(InstanceName, StructService);
		});

		It("should resolve UObjects", [this]
		{
			TestEqual("DiContainer.Resolve<USimpleUService>()", DiContainer.Resolve<USimpleUService>()->A, 20);
		});
		It("should resolve UInterfaces", [this]
		{
			const ISimpleInterface* ResolvedInterface = DiContainer.Resolve<ISimpleInterface>();
			TestEqual("DiContainer.Resolve<ISimpleInterface>()->GetA()", ResolvedInterface->GetA(), 20);
		});
		It("should resolve native classes", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve<FSimpleNativeService>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should resolve ustructs", [this]
		{
			TOptional<FSimpleUStructService> Resolved = DiContainer.Resolve<FSimpleUStructService>();
			if (TestTrue("Resolved.IsSet()", Resolved.IsSet()))
			{
				TestEqual("Resolved->A", Resolved->A, 20);
			}
		});

		It("should resolve named UObjects", [this]
		{
			TestEqual("DiContainer.Resolve<USimpleUService>()", DiContainer.Resolve<USimpleUService>("SomeName")->A, 22);
		});

		It("should resolve named UInterfaces", [this]
		{
			const ISimpleInterface* ResolvedInterface = DiContainer.Resolve<ISimpleInterface>("SomeName");
			TestEqual("DiContainer.Resolve<ISimpleInterface>()->GetA()", ResolvedInterface->GetA(), 20);
		});
		It("should resolve named UInterfaces", [this]
		{
			TestEqual("DiContainer.Resolve<USimpleUService>()", DiContainer.Resolve<ISimpleInterface>("SomeName")->GetA(), 22);
		});
		It("should resolve named native classes", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve<FSimpleNativeService>("SomeName");
			TestEqual("Resolved->A", Resolved->A, 22);
		});
		It("should resolve named UStructs", [this]
		{
			TOptional<FSimpleUStructService> Resolved = DiContainer.Resolve<FSimpleUStructService>("SomeName");
			if (TestTrue("Resolved.IsSet()", Resolved.IsSet()))
			{
				TestEqual("Resolved->A", Resolved->A, 22);
			}
		});

		It("should not resolve named UObjects with wrong name", [this]
		{
			TestFalse("DiContainer.Resolve<USimpleUService>()", !!DiContainer.Resolve<USimpleUService>("SomeWrongName"));
		});

		It("should not resolve named UInterfaces with wrong name", [this]
		{
			const ISimpleInterface* ResolvedInterface = DiContainer.Resolve<ISimpleInterface>("SomeWrongName");
			TestNull("DiContainer.Resolve<ISimpleInterface>()", ResolvedInterface);
		});
		It("should not resolve named native classes with wrong name", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve<FSimpleNativeService>("SomeWrongName");
			TestFalse("Resolved.IsValid()", Resolved.IsValid());
		});
		It("should resolve named UStructs with wrong name", [this]
		{
			const TOptional<FSimpleUStructService> Resolved = DiContainer.Resolve<FSimpleUStructService>("SomeWrongName");
			TestFalse("Resolved.IsSet()", Resolved.IsSet());
		});
	});
}

// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "Container/DiContainer.h"
#include "Mocks/SimpleService.h"

BEGIN_DEFINE_SPEC(DiContainerSpec, "Tentacle.DiContainer",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

	Tentacle::FDiContainer DiContainer;
END_DEFINE_SPEC(DiContainerSpec)

void DiContainerSpec::Define()
{
	BeforeEach([this]
	{
		DiContainer = Tentacle::FDiContainer();
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
			TestEqual("DiContainer.ResolveTypeInstance<USimpleUService>()", DiContainer.ResolveTypeInstance<USimpleUService>(), Service);
		});
		It("should bind UInterfaces", [this]
		{
			const TObjectPtr<USimpleInterfaceImplementation> Service = NewObject<USimpleInterfaceImplementation>();
			DiContainer.BindInstance<ISimpleInterface>(Service);
			const TScriptInterface<ISimpleInterface>& ResolvedInterface = DiContainer.ResolveTypeInstance<ISimpleInterface>();
			TestSame("DiContainer.ResolveTypeInstance<ISimpleInterface>()", *ResolvedInterface, static_cast<ISimpleInterface&>(*Service));
		});
		It("should bind native classes", [this]
		{
			const TSharedRef<FSimpleNativeService> Service = MakeShared<FSimpleNativeService>(20);
			DiContainer.BindInstance<FSimpleNativeService>(Service);
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.ResolveTypeInstance<FSimpleNativeService>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should bind ustructs", [this]
		{
			const FSimpleUStructService Service = FSimpleUStructService{20};
			DiContainer.BindInstance<FSimpleUStructService>(Service);
			TOptional<FSimpleUStructService> Resolved = DiContainer.ResolveTypeInstance<FSimpleUStructService>();
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
			DiContainer.BindInstance<ISimpleInterface>(SimpleInterfaceImplementation);
			SimpleInterfaceImplementation = NewObject<USimpleInterfaceImplementation>();
			SimpleInterfaceImplementation->A = 22;
			DiContainer.BindNamedInstance<ISimpleInterface>(InstanceName, SimpleInterfaceImplementation);

			TSharedRef<FSimpleNativeService> NativeService = MakeShared<FSimpleNativeService>(20);
			DiContainer.BindInstance<FSimpleNativeService>(NativeService);
			NativeService = MakeShared<FSimpleNativeService>(22);
			DiContainer.BindNamedInstance<FSimpleNativeService>(InstanceName, NativeService);

			FSimpleUStructService StructService = FSimpleUStructService{20};
			DiContainer.BindInstance<FSimpleUStructService>(StructService);
			StructService = FSimpleUStructService{22};
			DiContainer.BindNamedInstance<FSimpleUStructService>(InstanceName, StructService);

			TSharedRef<FMockEngineType> MockEngineType = MakeShared<FMockEngineType>(20);
			DiContainer.BindInstance<FMockEngineType>(MockEngineType);
			MockEngineType = MakeShared<FMockEngineType>(22);
			DiContainer.BindNamedInstance<FMockEngineType>(InstanceName, MockEngineType);
		});

		It("should resolve UObjects", [this]
		{
			TestEqual("DiContainer.ResolveTypeInstance<USimpleUService>()", DiContainer.ResolveTypeInstance<USimpleUService>()->A, 20);
		});
		It("should resolve UInterfaces", [this]
		{
			TScriptInterface<ISimpleInterface> ResolvedInterface = DiContainer.ResolveTypeInstance<ISimpleInterface>();
			TestEqual("DiContainer.ResolveTypeInstance<ISimpleInterface>()->GetA()", ResolvedInterface->GetA(), 20);
		});
		It("should resolve native classes", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.ResolveTypeInstance<FSimpleNativeService>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should resolve foreign native classes", [this]
		{
			const TSharedPtr<FMockEngineType> Resolved = DiContainer.ResolveTypeInstance<FMockEngineType>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should resolve ustructs", [this]
		{
			TOptional<FSimpleUStructService> Resolved = DiContainer.ResolveTypeInstance<FSimpleUStructService>();
			if (TestTrue("Resolved.IsSet()", Resolved.IsSet()))
			{
				TestEqual("Resolved->A", Resolved->A, 20);
			}
		});

		It("should resolve named UObjects", [this]
		{
			TestEqual("DiContainer.ResolveNamedInstance<USimpleUService>()", DiContainer.ResolveNamedInstance<USimpleUService>("SomeName")->A, 22);
		});
		It("should resolve named UInterfaces", [this]
		{
			TestEqual("DiContainer.ResolveNamedInstance<USimpleUService>()", DiContainer.ResolveNamedInstance<ISimpleInterface>("SomeName")->GetA(), 22);
		});
		It("should resolve named native classes", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.ResolveNamedInstance<FSimpleNativeService>("SomeName");
			TestEqual("Resolved->A", Resolved->A, 22);
		});
		It("should resolve named foreign native classes", [this]
		{
			const TSharedPtr<FMockEngineType> Resolved = DiContainer.ResolveNamedInstance<FMockEngineType>("SomeName");
			TestEqual("Resolved->A", Resolved->A, 22);
		});
		It("should resolve named UStructs", [this]
		{
			TOptional<FSimpleUStructService> Resolved = DiContainer.ResolveNamedInstance<FSimpleUStructService>("SomeName");
			if (TestTrue("Resolved.IsSet()", Resolved.IsSet()))
			{
				TestEqual("Resolved->A", Resolved->A, 22);
			}
		});

		It("should not resolve named UObjects with wrong name", [this]
		{
			TestFalse("DiContainer.ResolveNamedInstance<USimpleUService>()", !!DiContainer.ResolveNamedInstance<USimpleUService>("SomeWrongName"));
		});
		It("should not resolve named UInterfaces with wrong name", [this]
		{
			TScriptInterface<ISimpleInterface> ResolvedInterface = DiContainer.ResolveNamedInstance<ISimpleInterface>("SomeWrongName");
			TestNull("DiContainer.ResolveNamedInstance<ISimpleInterface>()", ResolvedInterface.GetObject());
		});
		It("should not resolve named native classes with wrong name", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.ResolveNamedInstance<FSimpleNativeService>("SomeWrongName");
			TestFalse("Resolved.IsValid()", Resolved.IsValid());
		});
		It("should not resolve named native foreign classes with wrong name", [this]
		{
			const TSharedPtr<FMockEngineType> Resolved = DiContainer.ResolveNamedInstance<FMockEngineType>("SomeWrongName");
			TestFalse("Resolved.IsValid()", Resolved.IsValid());
		});
		It("should resolve named UStructs with wrong name", [this]
		{
			const TOptional<FSimpleUStructService> Resolved = DiContainer.ResolveNamedInstance<FSimpleUStructService>("SomeWrongName");
			TestFalse("Resolved.IsSet()", Resolved.IsSet());
		});
	});

	Describe("Resolve", [this]
	{
		LatentIt("should resolve UObjects when already provided", [this](const FDoneDelegate& DoneDelegate)
		{
			TObjectPtr<USimpleUService> UService = NewObject<USimpleUService>();
			DiContainer.BindInstance<USimpleUService>(*UService);
			DiContainer.ResolveFutureTypeInstance<USimpleUService>().Next([DoneDelegate, this, UService](TOptional<USimpleUService&> Instance)
			{
				TestEqual("DiContainer.ResolveFutureTypeInstance<USimpleUService>()", &*Instance, UService.Get());
				DoneDelegate.Execute();
			});
		});
		LatentIt("should resolve UObjects when provided later", [this](const FDoneDelegate& DoneDelegate)
		{
			TObjectPtr<USimpleUService> UService = NewObject<USimpleUService>();
			DiContainer.ResolveFutureTypeInstance<USimpleUService>().Next([DoneDelegate, this, UService](TOptional<USimpleUService&> Instance)
			{
				TestEqual("DiContainer.ResolveFutureTypeInstance<USimpleUService>()", &*Instance, UService.Get());
				DoneDelegate.Execute();
			});
			DiContainer.BindInstance<USimpleUService>(*UService);
		});

		It("should invoke with unset optional when di container goes out of scope", [this]()
		{
			auto TempDiContainer = Tentacle::FDiContainer();
			TempDiContainer.ResolveFutureTypeInstance<USimpleUService>().Next([this](TOptional<USimpleUService&> Instance)
			{
				TestFalse("DiContainer.ResolveTypeInstance<USimpleUService>().Next.Instance", Instance.IsSet());
			});
		});
	});
}

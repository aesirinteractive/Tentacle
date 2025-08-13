// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "Container/DiContainer.h"
#include "Examples/ExampleComponent.h"
#include "Examples/ExampleNative.h"
#include "Misc/TypeContainer.h"
#include "Mocks/SimpleService.h"

BEGIN_DEFINE_SPEC(DiContainerSpec, "Tentacle.DiContainer",
                  EAutomationTestFlags::EngineFilter | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProgramContext)

	DI::FDiContainer DiContainer;
END_DEFINE_SPEC(DiContainerSpec)

void DiContainerSpec::Define()
{
	BeforeEach([this]
	{
		DiContainer = DI::FDiContainer();
	});
	AfterEach([this]
	{
		DiContainer = DI::FDiContainer();
	});
	Describe("Constructor", [this]
	{
	});
	Describe("BindInstance", [this]
	{
		It("should bind UObjects", [this]
		{
			const TObjectPtr<USimpleUService> Service = NewObject<USimpleUService>();
			DiContainer.Bind().BindInstance<USimpleUService>(Service);
			TestEqual("DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>()", DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>(), Service);
		});
		It("should bind UInterfaces", [this]
		{
			const TObjectPtr<USimpleInterfaceImplementation> Service = NewObject<USimpleInterfaceImplementation>();
			DiContainer.Bind().BindInstance<ISimpleInterface>(Service);
			const TScriptInterface<ISimpleInterface>& ResolvedInterface = DiContainer.Resolve().TryResolveTypeInstance<ISimpleInterface>();
			TestSame("DiContainer.Resolve().TryResolveTypeInstance<ISimpleInterface>()", *ResolvedInterface, static_cast<ISimpleInterface&>(*Service));
		});
		It("should bind native classes", [this]
		{
			const TSharedRef<FSimpleNativeService> Service = MakeShared<FSimpleNativeService>(20);
			DiContainer.Bind().BindInstance<FSimpleNativeService>(Service);
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve().TryResolveTypeInstance<FSimpleNativeService>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should bind ustructs", [this]
		{
			FSimpleUStructService Service = FSimpleUStructService{20};
			DiContainer.Bind().BindInstance<FSimpleUStructService>(Service);
			TOptional<const FSimpleUStructService&> Resolved = DiContainer.Resolve().TryResolveTypeInstance<FSimpleUStructService>();
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
			DiContainer.Bind().BindInstance<USimpleUService>(UService);
			UService = NewObject<USimpleUService>();
			UService->A = 22;
			DiContainer.Bind().BindNamedInstance<USimpleUService>(InstanceName, UService);

			TObjectPtr<USimpleInterfaceImplementation> SimpleInterfaceImplementation = NewObject<USimpleInterfaceImplementation>();
			SimpleInterfaceImplementation->A = 20;
			DiContainer.Bind().BindInstance<ISimpleInterface>(SimpleInterfaceImplementation);
			SimpleInterfaceImplementation = NewObject<USimpleInterfaceImplementation>();
			SimpleInterfaceImplementation->A = 22;
			DiContainer.Bind().BindNamedInstance<ISimpleInterface>(InstanceName, SimpleInterfaceImplementation);

			TSharedRef<FSimpleNativeService> NativeService = MakeShared<FSimpleNativeService>(20);
			DiContainer.Bind().BindInstance<FSimpleNativeService>(NativeService);
			NativeService = MakeShared<FSimpleNativeService>(22);
			DiContainer.Bind().BindNamedInstance<FSimpleNativeService>(InstanceName, NativeService);

			FSimpleUStructService StructService = FSimpleUStructService{20};
			DiContainer.Bind().BindInstance<FSimpleUStructService>(StructService);
			StructService = FSimpleUStructService{22};
			DiContainer.Bind().BindNamedInstance<FSimpleUStructService>(InstanceName, StructService);

			TSharedRef<FMockEngineType> MockEngineType = MakeShared<FMockEngineType>(20);
			DiContainer.Bind().BindInstance<FMockEngineType>(MockEngineType);
			MockEngineType = MakeShared<FMockEngineType>(22);
			DiContainer.Bind().BindNamedInstance<FMockEngineType>(InstanceName, MockEngineType);
		});

		It("should resolve UObjects", [this]
		{
			TestEqual("DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>()", DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>()->A, 20);
		});
		It("should resolve UInterfaces", [this]
		{
			TScriptInterface<ISimpleInterface> ResolvedInterface = DiContainer.Resolve().TryResolveTypeInstance<ISimpleInterface>();
			TestEqual("DiContainer.Resolve().TryResolveTypeInstance<ISimpleInterface>()->GetA()", ResolvedInterface->GetA(), 20);
		});
		It("should resolve native classes", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve().TryResolveTypeInstance<FSimpleNativeService>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should resolve foreign native classes", [this]
		{
			const TSharedPtr<FMockEngineType> Resolved = DiContainer.Resolve().TryResolveTypeInstance<FMockEngineType>();
			TestEqual("Resolved->A", Resolved->A, 20);
		});
		It("should resolve ustructs", [this]
		{
			TOptional<const FSimpleUStructService&> Resolved = DiContainer.Resolve().TryResolveTypeInstance<FSimpleUStructService>();
			if (TestTrue("Resolved.IsSet()", Resolved.IsSet()))
			{
				TestEqual("Resolved->A", Resolved->A, 20);
			}
		});

		It("should resolve named UObjects", [this]
		{
			TestEqual("DiContainer.Resolve().ResolveNamedInstance<USimpleUService>()", DiContainer.Resolve().TryResolveNamedInstance<USimpleUService>("SomeName")->A, 22);
		});
		It("should resolve named UInterfaces", [this]
		{
			TestEqual("DiContainer.Resolve().ResolveNamedInstance<USimpleUService>()", DiContainer.Resolve().TryResolveNamedInstance<ISimpleInterface>("SomeName")->GetA(), 22);
		});
		It("should resolve named native classes", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve().TryResolveNamedInstance<FSimpleNativeService>("SomeName");
			TestEqual("Resolved->A", Resolved->A, 22);
		});
		It("should resolve named foreign native classes", [this]
		{
			const TSharedPtr<FMockEngineType> Resolved = DiContainer.Resolve().TryResolveNamedInstance<FMockEngineType>("SomeName");
			TestEqual("Resolved->A", Resolved->A, 22);
		});
		It("should resolve named UStructs", [this]
		{
			TOptional<const FSimpleUStructService&> Resolved = DiContainer.Resolve().TryResolveNamedInstance<FSimpleUStructService>("SomeName");
			if (TestTrue("Resolved.IsSet()", Resolved.IsSet()))
			{
				TestEqual("Resolved->A", Resolved->A, 22);
			}
		});

		It("should not resolve named UObjects with wrong name", [this]
		{
			TestFalse("DiContainer.Resolve().ResolveNamedInstance<USimpleUService>()",
				bool(DiContainer.Resolve().TryResolveNamedInstance<USimpleUService>("SomeWrongName", DI::EResolveErrorBehavior::ReturnNull)));
		});
		It("should not resolve named UInterfaces with wrong name", [this]
		{
			TScriptInterface<ISimpleInterface> ResolvedInterface = DiContainer.Resolve().TryResolveNamedInstance<ISimpleInterface>("SomeWrongName", DI::EResolveErrorBehavior::ReturnNull);
			TestNull("DiContainer.Resolve().ResolveNamedInstance<ISimpleInterface>()", ResolvedInterface.GetObject());
		});
		It("should not resolve named native classes with wrong name", [this]
		{
			const TSharedPtr<FSimpleNativeService> Resolved = DiContainer.Resolve().TryResolveNamedInstance<FSimpleNativeService>("SomeWrongName", DI::EResolveErrorBehavior::ReturnNull);
			TestFalse("Resolved.IsValid()", Resolved.IsValid());
		});
		It("should not resolve named native foreign classes with wrong name", [this]
		{
			const TSharedPtr<FMockEngineType> Resolved = DiContainer.Resolve().TryResolveNamedInstance<FMockEngineType>("SomeWrongName", DI::EResolveErrorBehavior::ReturnNull);
			TestFalse("Resolved.IsValid()", Resolved.IsValid());
		});
		It("should not resolve named UStructs with wrong name", [this]
		{
			const TOptional<const FSimpleUStructService&> Resolved = DiContainer.Resolve().TryResolveNamedInstance<FSimpleUStructService>("SomeWrongName", DI::EResolveErrorBehavior::ReturnNull);
			TestFalse("Resolved.IsSet()", Resolved.IsSet());
		});
	});

	Describe("Resolve", [this]
	{
		LatentIt("should resolve UObjects when already provided", [this](const FDoneDelegate& DoneDelegate)
		{
			TObjectPtr<USimpleUService> UService = NewObject<USimpleUService>();
			DiContainer.Bind().BindInstance<USimpleUService>(UService);
			DiContainer.Resolve().TryResolveFutureTypeInstance<USimpleUService>().Next([DoneDelegate, this, UService](TOptional<TObjectPtr<USimpleUService>> Instance)
			{
				TestEqual("DiContainer.Resolve().ResolveFutureTypeInstance<USimpleUService>()", Instance->Get(), UService.Get());
				DoneDelegate.Execute();
			});
		});
		LatentIt("should resolve UObjects when provided later", [this](const FDoneDelegate& DoneDelegate)
		{
			TObjectPtr<USimpleUService> UService = NewObject<USimpleUService>();
			DiContainer.Resolve().TryResolveFutureTypeInstance<USimpleUService>().Next([DoneDelegate, this, UService](TOptional<TObjectPtr<USimpleUService>> Instance)
			{
				if (TestTrue("Instance.IsSet()", Instance.IsSet()))
				{
					TestEqual("DiContainer.Resolve().ResolveFutureTypeInstance<USimpleUService>()", Instance->Get(), UService.Get());
				}
				DoneDelegate.Execute();
			});
			DiContainer.Bind().BindInstance<USimpleUService>(UService);
		});

		LatentIt("should resolve multiple UObjects when provided later", FTimespan::FromSeconds(1), [this](const FDoneDelegate& DoneDelegate)
		{
			TObjectPtr<USimpleUService> UService = NewObject<USimpleUService>();
			TObjectPtr<USimpleInterfaceImplementation> UInterfaceService = NewObject<USimpleInterfaceImplementation>();
			FSimpleUStructService UStructService = {};
			TSharedRef<FSimpleNativeService> NativeServiceSharedPtr = MakeShared<FSimpleNativeService>();
			DiContainer.Resolve().TryResolveFutureTypeInstances<USimpleUService, ISimpleInterface, FSimpleUStructService, FSimpleNativeService>()
			.ExpandNext([DoneDelegate, this, UService, UInterfaceService, UStructService, NativeServiceSharedPtr](TOptional<TObjectPtr<USimpleUService>> ObjectService, TOptional<TScriptInterface<ISimpleInterface>> InterfaceService, TOptional<const FSimpleUStructService&> StructService, TOptional<TSharedRef<FSimpleNativeService>> NativeService)
			{
				if (TestTrue("ObjectService.IsSet()", ObjectService.IsSet()))
				{
					TestEqual("ObjectService", ObjectService->Get(), UService.Get());
				}
				if (TestTrue("InterfaceService.IsSet()", InterfaceService.IsSet()))
				{
					TestEqual<UObject*>("InterfaceService", InterfaceService->GetObject(), UInterfaceService.Get());
				}
				if (TestTrue("StructService.IsSet()", StructService.IsSet()))
				{
					TestEqual("StructService", *StructService, UStructService);
				}
				if (TestTrue("NativeService.IsSet()", NativeService.IsSet()))
				{
					TestEqual("NativeService", *NativeService, NativeServiceSharedPtr);
				}
				DoneDelegate.Execute();
			});
			DiContainer.Bind().BindInstance<USimpleUService>(UService);
			DiContainer.Bind().BindInstance<ISimpleInterface>(UInterfaceService);
			DiContainer.Bind().BindInstance<FSimpleUStructService>(UStructService);
			DiContainer.Bind().BindInstance<FSimpleNativeService>(NativeServiceSharedPtr);
		});

		It("should invoke with unset optional when di container goes out of scope", [this]()
		{
			auto TempDiContainer = DI::FDiContainer();
			TempDiContainer.Resolve().TryResolveFutureTypeInstance<USimpleUService>().Next([this](TOptional<TObjectPtr<USimpleUService>> Instance)
			{
				TestFalse("DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>().Next.Instance", Instance.IsSet());
			});
		});
	});

	Describe("Inject", [this]
	{
		BeforeEach([this]
		{
			DiContainer.Bind().BindInstance<USimpleUService>(NewObject<USimpleUService>());
			DiContainer.Bind().BindInstance<FSimpleNativeService>(MakeShared<FSimpleNativeService>());
		});
		It("should inject into free functions", [this]
		{
			TestEqual("Service", DiContainer.Inject().IntoFunctionByType(&DI::InjectTest::InjectDependencies), DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>());
		});
		It("should inject into member functions", [this]
		{
			FExampleNative Native = {};
			DiContainer.Inject().IntoFunctionByType(Native, &FExampleNative::Initialize);
			TestEqual("NativeService", Native.SimpleNativeService, DiContainer.Resolve().TryResolveTypeInstance<FSimpleNativeService>());
		});
		It("should inject into uobject member functions", [this]
		{
			UExampleComponent* ExampleComponent = NewObject<UExampleComponent>();
			DiContainer.Inject().IntoFunctionByType(*ExampleComponent, &UExampleComponent::InjectDependencies);
			TestEqual("NativeService", ExampleComponent->SimpleUService, DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>());
		});

		It("should inject into member functions with extra args", [this]
		{
			FExampleNative Native = {};
			FString ExtraString("test");
			DiContainer.Inject().IntoLambda([&](TSharedPtr<FSimpleNativeService> NativeService)
			{
				Native.InitializeWithExtraArgs(NativeService, ExtraString);
			});
			TestEqual("NativeService", Native.SimpleNativeService, DiContainer.Resolve().TryResolveTypeInstance<FSimpleNativeService>());
		});

		It( "should inject into uobject member functions with extra args", [this] {
			UExampleComponent* ExampleComponent = NewObject<UExampleComponent>();
			FString ExtraString("test");
			DiContainer.Inject().IntoLambda(
				[&](TObjectPtr<USimpleUService> SimpleUService)
				{
					ExampleComponent->InjectDependenciesWithExtraArgs(SimpleUService, ExtraString);
				}
			);
			TestEqual("NativeService", ExampleComponent->SimpleUService, DiContainer.Resolve().TryResolveTypeInstance<USimpleUService>());
			TestEqual("ExtraString", ExampleComponent->ExtraString, ExtraString);
		});
	});
}

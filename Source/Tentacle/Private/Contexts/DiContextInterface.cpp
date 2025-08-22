// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "TentacleSettings.h"
#include "Contexts/DiContextComponent.h"
#include "Contexts/DIContextInterface.h"
#include "Contexts/DiEngineSubsystem.h"
#include "Contexts/DiGameInstanceSubsystem.h"
#include "Contexts/DiLocalPlayerSubsystem.h"
#include "Contexts/DiWorldSubsystem.h"

TScriptInterface<IDiContextInterface> DI::TryFindDiContext(UObject* StartObject)
{
	if (!StartObject)
		return nullptr;

	if (TScriptInterface<IDiContextInterface> LocalDiContext = TryGetLocalDiContext(StartObject))
		return LocalDiContext;

	if (UActorComponent* ActorComponent = Cast<UActorComponent>(StartObject))
	{
		if (AActor* OwnerActor = ActorComponent->GetOwner())
		{
			return TryFindDiContext(OwnerActor);
		}
	}

	if (UObject* Outer = StartObject->GetOuter())
	{
		return TryFindDiContext(Outer);
	}

	return nullptr;
}

TScriptInterface<IDiContextInterface> DI::TryGetLocalDiContext(UObject* StartObject)
{
	if (!StartObject)
		return nullptr;

	if (StartObject->Implements<UDiContextInterface>())
		return TScriptInterface<IDiContextInterface>(StartObject);

	if (AActor* Actor = Cast<AActor>(StartObject))
	{
		if (UDiContextComponent* DiContextComponent = Actor->FindComponentByClass<UDiContextComponent>())
		{
			return DiContextComponent;
		}
	}

	if (GetDefault<UTentacleSettings>()->bEnableScopeSubsystems)
	{
		if (UWorld* World = Cast<UWorld>(StartObject))
		{
			return World->GetSubsystem<UDiWorldSubsystem>();
		}
		if (UGameInstance* GameInstance = Cast<UGameInstance>(StartObject))
		{
			return GameInstance->GetSubsystem<UDiGameInstanceSubsystem>();
		}
		if (UEngine* Engine = Cast<UEngine>(StartObject))
		{
			return Engine->GetEngineSubsystem<UDiEngineSubsystem>();
		}
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(StartObject))
		{
			return LocalPlayer->GetSubsystem<UDiLocalPlayerSubsystem>();
		}
	}

	return nullptr;
}

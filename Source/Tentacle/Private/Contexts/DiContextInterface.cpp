// Copyright Manuel Wagner (singinwhale.com). All Rights Reserved.


#include "TentacleSettings.h"
#include "Contexts/DIContextInterface.h"
#include "Contexts/DiEngineSubsystem.h"
#include "Contexts/DiGameInstanceSubsystem.h"
#include "Contexts/DiLocalPlayerSubsystem.h"
#include "Contexts/DiWorldSubsystem.h"

void IDiContextInterface::RequestInitialize(const TScriptInterface<IAutoInjectable>& InitializationTarget)
{
	InitializationTarget->AutoInject(reinterpret_cast<UObject*>(this));
}

TScriptInterface<IDiContextInterface> DI::TryFindDiContext(UObject* StartObject)
{
	if (!StartObject)
		return nullptr;

	if (StartObject->Implements<UDiContextInterface>())
		return TScriptInterface<IDiContextInterface>(StartObject);

	if (UActorComponent* ActorComponent = Cast<UActorComponent>(StartObject))
	{
		if (AActor* OwnerActor = ActorComponent->GetOwner())
		{
			return TryFindDiContext(OwnerActor);
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

	if (UObject* Outer = StartObject->GetOuter())
	{
		return TryFindDiContext(Outer);
	}

	return nullptr;
}

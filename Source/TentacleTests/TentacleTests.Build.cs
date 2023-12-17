// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TentacleTests : ModuleRules
{
	public TentacleTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Tentacle",
				"Core",
				"CoreUObject",
				"Engine",
			}
		);
	}
}
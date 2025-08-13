﻿using UnrealBuildTool;

public class TentacleEditor : ModuleRules
{
    public TentacleEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Tentacle",
                "BlueprintGraph",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "KismetCompiler",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "ToolMenus",
            }
        );
    }
}
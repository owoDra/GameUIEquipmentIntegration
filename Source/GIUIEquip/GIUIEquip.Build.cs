// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GIUIEquip : ModuleRules
{
	public GIUIEquip(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
           new string[]
           {
                ModuleDirectory,
                ModuleDirectory + "/GIUIEquip",
           }
       );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine",

                "UMG", "GameplayTags",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GUIExt", "GEEquip",
            }
        );
    }
}

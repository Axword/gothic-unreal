// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PopiolISol : ModuleRules
{
	public PopiolISol(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"Json",
			"JsonUtilities",
			"AIModule",
			"NavigationSystem",
			"GameplayTasks",
			"Slate",
			"SlateCore"
		});
	}
}

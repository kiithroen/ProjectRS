// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProjectRS : ModuleRules
{
	public ProjectRS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.AddRange(new string[] {
			"ProjectRS"
		});
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"Niagara",
			"GameplayTags",
			"AIModule",
			"Slate",
			"SlateCore",
			"UMG",
			"GameplayAbilities",
			"DeveloperSettings",
			"NavigationSystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

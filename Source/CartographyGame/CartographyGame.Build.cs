// Copyright (c) 2026 The Last Cartographer.

using UnrealBuildTool;

public class CartographyGame : ModuleRules
{
	public CartographyGame(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
			"SlateCore",
			"RenderCore",
			"RHI",
			"ImageWrapper",
			"GameplayTags",
			"AudioMixer",
			"DeveloperSettings",
			"NavigationSystem",
			"AIModule",
			"ProceduralMeshComponent"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"Niagara"
		});
	}
}

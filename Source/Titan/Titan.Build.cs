// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Titan : ModuleRules
{
	public Titan(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"GameplayTags",
				"EnhancedInput",
				"GameplayAbilities",
				"GameplayTasks",
				"AIModule",
				"Water",
				"PhysicsCore",
				"Slate",
				"CommonUI",
				"ImageCore",
				"SampleFramework",
				"DeveloperSettings",
				"PlayerInteraction",
				"StateTreeDialogue",
				"InputWidgets",
				"TitanCamera",
				"TitanMovement",
				"TitanRaft",
				"InventorySystem",
				"QuestSystem",
				"PhotoAlbum",
				"Chaos",
				"UMG",
                "Mover"
            });

		PublicIncludePaths.AddRange(
			new string[] {
				"Titan",
				"Titan/Logging",
				"Titan/Character",
				"Titan/Development"
			});

        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"ChunkDownloader"
			});
    }
}

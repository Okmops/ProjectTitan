// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TitanRaft : ModuleRules
{
	public TitanRaft(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Mover",
				"PhysicsCore",
				"Chaos",
				"Water",
				"GameplayTags",
				"GameplayAbilities",
				"TitanMovement",
				"TitanCamera"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}

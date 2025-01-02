// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Runeward : ModuleRules
{
	public Runeward(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Eigen", "AIModule", "MatchFinder" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}

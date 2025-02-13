// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UdmyOSubSysCpp : ModuleRules
{
	public UdmyOSubSysCpp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem" });
	}
}

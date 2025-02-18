// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UdmyOSubSysCppUSrc : ModuleRules
{
	public UdmyOSubSysCppUSrc(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem", "AdvancedSessions", "AdvancedSteamSessions" });
	}
}

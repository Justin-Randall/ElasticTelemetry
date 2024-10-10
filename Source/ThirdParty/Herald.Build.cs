// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

using UnrealBuildTool;
using System.IO;

public class Herald : ModuleRules
{
	public Herald(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		string IncludePath = Path.Combine(ModuleDirectory, "Herald", "include");
		string LibPath = Path.Combine(ModuleDirectory, "Herald", "lib");

		// Include paths
		PublicIncludePaths.Add(IncludePath);

		// Platform-specific library paths and libraries
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string PlatformLibPath = Path.Combine(LibPath, "Windows", "x64");

			// DebugGame is optimized, so use RelWithDebugInfo, otherwise Debug if it is truly a debug build, otherwise release
			string Config = "Release";
			if (Target.Configuration == UnrealTargetConfiguration.Debug)
			{
				Config = "Debug";
			}
			else if (Target.Configuration == UnrealTargetConfiguration.DebugGame)
			{
				Config = "RelWithDebInfo";
			}

			string FinalLibPath = Path.Combine(PlatformLibPath, Config);
			PublicAdditionalLibraries.Add(Path.Combine(FinalLibPath, "herald_library.lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			string PlatformLibPath = Path.Combine(LibPath, "Linux", "x64");
			string Config = (Target.Configuration == UnrealTargetConfiguration.Debug) ? "Debug" : "Release";
			string FinalLibPath = Path.Combine(PlatformLibPath, Config);

			PublicAdditionalLibraries.Add(Path.Combine(FinalLibPath, "libherald_library.a"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			string PlatformLibPath = Path.Combine(LibPath, "macOS", "x64");
			string Config = (Target.Configuration == UnrealTargetConfiguration.Debug) ? "Debug" : "Release";
			string FinalLibPath = Path.Combine(PlatformLibPath, Config);

			PublicAdditionalLibraries.Add(Path.Combine(FinalLibPath, "libherald_library.a"));
		}

		// Add any additional platform support as needed

		// Definitions (if any)
		PublicDefinitions.Add("HERALD_LIB");
	}
}

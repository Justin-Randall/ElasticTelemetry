// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ElasticTelemetry : ModuleRules
{
	public ElasticTelemetry(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "ThirdParty");

		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory,
				// ... add public include paths required here ...
			}
			);
		PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Herald", "include"));


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				Path.Combine(ModuleDirectory, "Private")
			}
			);



		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Herald"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Herald",
				"Json",
				"DeveloperSettings",
				"HTTP"
				
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

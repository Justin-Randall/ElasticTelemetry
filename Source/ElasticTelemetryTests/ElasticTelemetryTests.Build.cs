using UnrealBuildTool;
using System.IO;

public class ElasticTelemetryTests : ModuleRules
{
	public ElasticTelemetryTests(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.CPlusPlus;

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Public and Private Include Paths
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory)
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory),
				Path.Combine(ModuleDirectory, "../ElasticTelemetry/Private")
			}
		);

		// Dependencies
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"ElasticTelemetry"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"AutomationController",
				"AutomationTest",
				"ElasticTelemetry"
			}
		);

		// If you have editor-only dependencies
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd"
				}
			);
		}

		// Disable optimization for faster compilation during testing
		MinFilesUsingPrecompiledHeaderOverride = 1;
	}
}

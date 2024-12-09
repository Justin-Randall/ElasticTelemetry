// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

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
				Path.Combine(ModuleDirectory),
				Path.Combine(ModuleDirectory, "../ElasticTelemetryEditor")
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
				"Core",
				"CoreUObject",
				"Engine",
				"Herald",
				"AutomationController",
				"AutomationTest",
				"ElasticTelemetry",
				"ElasticTelemetryEditor"
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

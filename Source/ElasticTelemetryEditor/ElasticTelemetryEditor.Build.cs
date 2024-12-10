// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

using UnrealBuildTool;

public class ElasticTelemetryEditor : ModuleRules
{
	public ElasticTelemetryEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Make sure this module only loads in the Editor
		if (Target.Type == TargetType.Editor)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"Json",
					"HTTP",
					"DeveloperSettings",
					"UnrealEd",
					"Slate",
					"SlateCore",
					"InputCore",
					"UMG",
					"PropertyEditor",
					"ElasticTelemetry"
				}
			);

			PrivateIncludePaths.Add("ElasticTelemetry/Private");
		}
	}
}

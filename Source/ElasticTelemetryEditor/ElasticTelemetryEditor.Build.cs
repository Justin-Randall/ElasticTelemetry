using UnrealBuildTool;

public class ElasticTelemetryEditor : ModuleRules
{
	public ElasticTelemetryEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Make sure this module only loads in the Editor
		if (Target.bBuildEditor == true)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
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

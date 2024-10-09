using UnrealBuildTool;
using System.IO;

public class Herald : ModuleRules
{
	public Herald(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		string IncludePath = Path.Combine(ModuleDirectory, "Herald", "include");
		string LibPath = Path.Combine(ModuleDirectory, "Herald", "lib");
		System.Console.WriteLine(System.String.Format("Herald: IncludePath={0}, LibPath={1}", IncludePath, LibPath));

		// Include paths
		PublicIncludePaths.Add(IncludePath);

		// Platform-specific library paths and libraries
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string PlatformLibPath = Path.Combine(LibPath, "Windows", "x64");
			System.Console.WriteLine(System.String.Format("Herald: PlatformLibPath={0}", PlatformLibPath));

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

			//string Config = (Target.Configuration == UnrealTargetConfiguration.Debug) ? "Debug" : "Release";
			System.Console.WriteLine(System.String.Format("Target.Configuration={0}, Herald: Config={1}", Target.Configuration, Config));

			string FinalLibPath = Path.Combine(PlatformLibPath, Config);
			System.Console.WriteLine(System.String.Format("Herald: FinalLibPath={0}", FinalLibPath));

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

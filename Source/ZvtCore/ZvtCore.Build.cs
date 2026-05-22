using UnrealBuildTool;
using System.IO;

public class ZvtCore : ModuleRules
{
	public ZvtCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[]
		{
			Path.Combine(ModuleDirectory, "Public", "Logging"),
			Path.Combine(ModuleDirectory, "Public", "Utilities"),
			Path.Combine(ModuleDirectory, "Public", "Debug"),
			Path.Combine(ModuleDirectory, "Public", "Settings")
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"RenderCore"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"Slate",
				"SlateCore"
			});
		}
	}
}

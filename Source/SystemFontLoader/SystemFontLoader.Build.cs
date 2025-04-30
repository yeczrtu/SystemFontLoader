using UnrealBuildTool;

public class SystemFontLoader : ModuleRules
{
    public SystemFontLoader(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "SlateCore", 
				"Slate",
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
			}
            );

        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add("Gdi32.lib");
            PublicAdditionalLibraries.Add("Advapi32.lib"); 
            PublicAdditionalLibraries.Add("Shell32.lib");
        }
    }
}
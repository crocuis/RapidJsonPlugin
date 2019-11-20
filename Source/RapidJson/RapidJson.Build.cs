using UnrealBuildTool;

public class RapidJson : ModuleRules
{
    private string ThirdPartyPath
    {
        get
        {
            return System.IO.Path.GetFullPath(
                System.IO.Path.Combine(ModuleDirectory, "../../ThirdParty")
            );
        }
    }

    public RapidJson(ReadOnlyTargetRules Target) : base(Target)
    {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core", "CoreUObject", "Engine",
            }
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
			}
		);

        PublicIncludePaths.Add(ThirdPartyPath);

    }
}

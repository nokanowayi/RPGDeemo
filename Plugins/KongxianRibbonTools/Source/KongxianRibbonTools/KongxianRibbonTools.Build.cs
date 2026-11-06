using UnrealBuildTool;

public class KongxianRibbonTools : ModuleRules
{
    public KongxianRibbonTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] {
            "Core", "CoreUObject", "Engine", "UnrealEd", "AssetRegistry", "PhysicsCore", "Json"
        });
    }
}

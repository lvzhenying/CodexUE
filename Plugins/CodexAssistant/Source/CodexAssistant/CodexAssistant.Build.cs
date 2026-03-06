using UnrealBuildTool;

public class CodexAssistant : ModuleRules
{
    public CodexAssistant(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "DeveloperSettings"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "LevelEditor",
            "ToolMenus",
            "Projects",
            "HTTP",
            "Json",
            "JsonUtilities"
        });
    }
}

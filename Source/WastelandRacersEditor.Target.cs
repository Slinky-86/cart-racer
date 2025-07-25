using UnrealBuildTool;
using System.Collections.Generic;

public class WastelandRacersEditorTarget : TargetRules
{
	public WastelandRacersEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		CppStandard = CppStandardVersion.Cpp20;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		bOverrideBuildEnvironment = true;  // allows property overrides with installed engine.
		bLegacyPublicIncludePaths = true;
		WindowsPlatform.bStrictConformanceMode = false;
		ExtraModuleNames.Add("WastelandRacers");
	}
}

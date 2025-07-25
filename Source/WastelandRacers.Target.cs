using UnrealBuildTool;
using System.Collections.Generic;

public class WastelandRacersTarget : TargetRules
{
	public WastelandRacersTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		CppStandard = CppStandardVersion.Cpp20;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		bLegacyPublicIncludePaths = true;
		WindowsPlatform.bStrictConformanceMode = false;
		ExtraModuleNames.Add("WastelandRacers");
	}
}

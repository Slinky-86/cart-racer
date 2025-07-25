using UnrealBuildTool;

public class WastelandRacers : ModuleRules
{
	public WastelandRacers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"Networking",
			"Sockets",
			"Niagara",
			"PhysicsCore",
			"ChaosVehicles",
			"AudioMixer",
			"NavigationSystem",
			"AIModule",
			"ProceduralMeshComponent",
			"Landscape",
			"Foliage",
			"RenderCore",
			"MaterialUtilities",
			"Json",
			"JsonUtilities",
			"EnhancedInput",
			"AssetRegistry"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"RenderCore",
			"RHI",
			"ApplicationCore",
			"Json",
			"JsonUtilities",
			"GeometryCore"
		});

		// Mobile optimizations
		if (Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicDefinitions.Add("MOBILE_OPTIMIZED=1");
		}
	}
}

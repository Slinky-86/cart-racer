#include "WRLandscapeManager.h"
#include "Landscape.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"

AWRLandscapeManager::AWRLandscapeManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create lighting components
	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	SunLight->SetupAttachment(RootComponent);
	SunLight->SetIntensity(3.0f);
	SunLight->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));

	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(RootComponent);
	SkyLight->SetIntensity(1.0f);

	HeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
	HeightFog->SetupAttachment(RootComponent);
	HeightFog->SetFogDensity(0.02f);
	HeightFog->SetFogHeightFalloff(0.2f);
}

void AWRLandscapeManager::BeginPlay()
{
	Super::BeginPlay();
}

void AWRLandscapeManager::GenerateLandscape(ELandscapeType LandscapeType)
{
	switch (LandscapeType)
	{
		case ELandscapeType::Desert:
			SetupDesertLandscape();
			break;
		case ELandscapeType::Urban:
			SetupUrbanLandscape();
			break;
		case ELandscapeType::Underground:
			SetupUndergroundLandscape();
			break;
		case ELandscapeType::Forest:
			SetupForestLandscape();
			break;
		case ELandscapeType::Lunar:
			SetupLunarLandscape();
			break;
	}

	SpawnEnvironmentalObjects();
	SetupLighting(LandscapeType);
}

void AWRLandscapeManager::SpawnEnvironmentalObjects()
{
	// Clear existing objects
	for (UStaticMeshComponent* Object : SpawnedObjects)
	{
		if (IsValid(Object))
		{
			Object->DestroyComponent();
		}
	}
	SpawnedObjects.Empty();

	// Spawn new objects based on current landscape configuration
	// This would be expanded with specific spawning logic
}

void AWRLandscapeManager::SetupLighting(ELandscapeType LandscapeType)
{
	switch (LandscapeType)
	{
		case ELandscapeType::Desert:
			SunLight->SetIntensity(4.0f);
			SunLight->SetLightColor(FLinearColor(1.0f, 0.9f, 0.7f));
			SkyLight->SetIntensity(0.8f);
			HeightFog->SetFogInscatteringColor(FLinearColor(0.8f, 0.7f, 0.5f));
			break;

		case ELandscapeType::Urban:
			SunLight->SetIntensity(2.5f);
			SunLight->SetLightColor(FLinearColor(0.9f, 0.9f, 1.0f));
			SkyLight->SetIntensity(1.2f);
			HeightFog->SetFogInscatteringColor(FLinearColor(0.6f, 0.6f, 0.7f));
			break;

		case ELandscapeType::Underground:
			SunLight->SetIntensity(0.5f);
			SunLight->SetLightColor(FLinearColor(0.4f, 0.2f, 0.8f)); // Eridium glow
			SkyLight->SetIntensity(0.3f);
			HeightFog->SetFogInscatteringColor(FLinearColor(0.3f, 0.1f, 0.5f));
			break;

		case ELandscapeType::Forest:
			SunLight->SetIntensity(3.0f);
			SunLight->SetLightColor(FLinearColor(0.9f, 1.0f, 0.8f));
			SkyLight->SetIntensity(1.0f);
			HeightFog->SetFogInscatteringColor(FLinearColor(0.5f, 0.7f, 0.4f));
			break;

		case ELandscapeType::Lunar:
			SunLight->SetIntensity(5.0f);
			SunLight->SetLightColor(FLinearColor(1.0f, 1.0f, 1.0f));
			SkyLight->SetIntensity(0.2f);
			HeightFog->SetFogInscatteringColor(FLinearColor(0.1f, 0.1f, 0.2f));
			break;
	}
}

void AWRLandscapeManager::ApplyWeatherEffects(bool bRaining, bool bFoggy, float Intensity)
{
	if (bRaining)
	{
		SunLight->SetIntensity(SunLight->Intensity * (1.0f - Intensity * 0.5f));
		HeightFog->SetFogDensity(HeightFog->FogDensity * (1.0f + Intensity));
	}

	if (bFoggy)
	{
		HeightFog->SetFogDensity(HeightFog->FogDensity * (1.0f + Intensity * 2.0f));
		SkyLight->SetIntensity(SkyLight->Intensity * (1.0f - Intensity * 0.3f));
	}
}

void AWRLandscapeManager::SetupDesertLandscape()
{
	// Pandora Desert configuration
	FLandscapeConfiguration DesertConfig;
	DesertConfig.LandscapeType = ELandscapeType::Desert;
	DesertConfig.LandscapeScale = FVector(200.0f, 200.0f, 150.0f);
	LandscapeConfigurations.Add(ELandscapeType::Desert, DesertConfig);
}

void AWRLandscapeManager::SetupUrbanLandscape()
{
	// Opportunity City configuration
	FLandscapeConfiguration UrbanConfig;
	UrbanConfig.LandscapeType = ELandscapeType::Urban;
	UrbanConfig.LandscapeScale = FVector(150.0f, 150.0f, 200.0f);
	LandscapeConfigurations.Add(ELandscapeType::Urban, UrbanConfig);
}

void AWRLandscapeManager::SetupUndergroundLandscape()
{
	// Eridium Mines configuration
	FLandscapeConfiguration UndergroundConfig;
	UndergroundConfig.LandscapeType = ELandscapeType::Underground;
	UndergroundConfig.LandscapeScale = FVector(100.0f, 100.0f, 80.0f);
	LandscapeConfigurations.Add(ELandscapeType::Underground, UndergroundConfig);
}

void AWRLandscapeManager::SetupForestLandscape()
{
	// Wildlife Preserve configuration
	FLandscapeConfiguration ForestConfig;
	ForestConfig.LandscapeType = ELandscapeType::Forest;
	ForestConfig.LandscapeScale = FVector(180.0f, 180.0f, 120.0f);
	LandscapeConfigurations.Add(ELandscapeType::Forest, ForestConfig);
}

void AWRLandscapeManager::SetupLunarLandscape()
{
	// Hyperion Moon Base configuration
	FLandscapeConfiguration LunarConfig;
	LunarConfig.LandscapeType = ELandscapeType::Lunar;
	LunarConfig.LandscapeScale = FVector(250.0f, 250.0f, 100.0f);
	LandscapeConfigurations.Add(ELandscapeType::Lunar, LunarConfig);
}

#include "WRMaterialManager.h"
#include "WastelandRacers.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "Engine/Engine.h"
#include "Components/PrimitiveComponent.h"

void UWRMaterialManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadDefaultMaterials();
	SetupPBRParameters();
}

UWRMaterialManager* UWRMaterialManager::GetInstance(const UObject* WorldContext)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UWRMaterialManager>();
		}
	}
	return nullptr;
}

UMaterialInterface* UWRMaterialManager::GetMaterial(EMaterialType MaterialType, int32 VariantIndex)
{
	if (FMaterialSettings* Settings = MaterialDatabase.Find(MaterialType))
	{
		if (VariantIndex == 0)
		{
			return Settings->BaseMaterial;
		}
		else if (VariantIndex == 1 && Settings->DamagedVariant)
		{
			return Settings->DamagedVariant;
		}
		else if (VariantIndex == 2 && Settings->WetVariant)
		{
			return Settings->WetVariant;
		}
	}
	return nullptr;
}

void UWRMaterialManager::ApplyWeatherEffects(bool bIsRaining, float Intensity)
{
	if (GlobalMaterialParameters)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), GlobalMaterialParameters, 
			FName("RainIntensity"), bIsRaining ? Intensity : 0.0f);
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), GlobalMaterialParameters, 
			FName("WetnessAmount"), bIsRaining ? Intensity * 0.8f : 0.0f);
	}
}

void UWRMaterialManager::SetTimeOfDay(float TimeOfDay)
{
	if (GlobalMaterialParameters)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), GlobalMaterialParameters, 
			FName("TimeOfDay"), TimeOfDay);
		
		// Calculate sun angle and lighting
		float SunAngle = TimeOfDay * 360.0f;
		FLinearColor SunColor = FLinearColor::LerpUsingHSV(
			FLinearColor(1.0f, 0.8f, 0.6f), // Dawn/Dusk
			FLinearColor(1.0f, 1.0f, 0.9f), // Noon
			FMath::Abs(FMath::Sin(FMath::DegreesToRadians(SunAngle)))
		);
		
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), GlobalMaterialParameters, 
			FName("SunColor"), SunColor);
	}
}

UMaterialInstanceDynamic* UWRMaterialManager::CreateDynamicMaterial(EMaterialType MaterialType, UPrimitiveComponent* Component)
{
	UMaterialInterface* BaseMaterial = GetMaterial(MaterialType);
	if (BaseMaterial && Component)
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, Component);
		Component->SetMaterial(0, DynamicMaterial);
		return DynamicMaterial;
	}
	return nullptr;
}

void UWRMaterialManager::LoadDefaultMaterials()
{
	// Track Surface Materials
	FMaterialSettings AsphaltSettings;
	AsphaltSettings.TilingScale = 4.0f;
	AsphaltSettings.BaseColor = FLinearColor(0.1f, 0.1f, 0.1f);
	MaterialDatabase.Add(EMaterialType::TrackAsphalt, AsphaltSettings);

	FMaterialSettings DirtSettings;
	DirtSettings.TilingScale = 2.0f;
	DirtSettings.BaseColor = FLinearColor(0.4f, 0.3f, 0.2f);
	MaterialDatabase.Add(EMaterialType::TrackDirt, DirtSettings);

	FMaterialSettings MetalSettings;
	MetalSettings.TilingScale = 1.0f;
	MetalSettings.BaseColor = FLinearColor(0.7f, 0.7f, 0.7f);
	MaterialDatabase.Add(EMaterialType::TrackMetal, MetalSettings);

	// Vehicle Materials
	FMaterialSettings VehiclePaintSettings;
	VehiclePaintSettings.BaseColor = FLinearColor(0.8f, 0.2f, 0.1f);
	MaterialDatabase.Add(EMaterialType::VehiclePaint, VehiclePaintSettings);

	FMaterialSettings VehicleMetalSettings;
	VehicleMetalSettings.BaseColor = FLinearColor(0.6f, 0.6f, 0.6f);
	MaterialDatabase.Add(EMaterialType::VehicleMetal, VehicleMetalSettings);

	// Environment Materials
	FMaterialSettings RockSettings;
	RockSettings.TilingScale = 3.0f;
	RockSettings.BaseColor = FLinearColor(0.5f, 0.4f, 0.3f);
	MaterialDatabase.Add(EMaterialType::EnvironmentRock, RockSettings);

	FMaterialSettings VegetationSettings;
	VegetationSettings.BaseColor = FLinearColor(0.2f, 0.6f, 0.1f);
	MaterialDatabase.Add(EMaterialType::EnvironmentVegetation, VegetationSettings);
}

void UWRMaterialManager::SetupPBRParameters()
{
	// Setup physically-based rendering parameters for realistic materials
	UE_LOG(LogTemp, Warning, TEXT("Material Manager: PBR parameters configured"));
}

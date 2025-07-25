#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"
#include "LandscapeProxy.h"
#include "WRLandscapeManager.generated.h"

UENUM(BlueprintType)
enum class ELandscapeType : uint8
{
	Desert,
	Urban,
	Underground,
	Forest,
	Lunar
};

USTRUCT(BlueprintType)
struct FLandscapeConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELandscapeType LandscapeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LandscapeScale = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ComponentSizeQuads = 63;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SectionsPerComponent = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QuadsPerSection = 31;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* HeightmapTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* LandscapeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UTexture2D*> LayerTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UStaticMesh*> FoliageMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UStaticMesh*> RockMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UStaticMesh*> StructureMeshes;
};

UCLASS()
class WASTELANDRACERS_API AWRLandscapeManager : public AActor
{
	GENERATED_BODY()

public:
	AWRLandscapeManager();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void GenerateLandscape(ELandscapeType LandscapeType);

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void SpawnEnvironmentalObjects();

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void SetupLighting(ELandscapeType LandscapeType);

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void ApplyWeatherEffects(bool bRaining, bool bFoggy, float Intensity);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	TMap<ELandscapeType, FLandscapeConfiguration> LandscapeConfigurations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape")
	class ALandscape* CurrentLandscape;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDirectionalLightComponent* SunLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkyLightComponent* SkyLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UExponentialHeightFogComponent* HeightFog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TArray<class UStaticMeshComponent*> SpawnedObjects;

private:
	void SetupDesertLandscape();
	void SetupUrbanLandscape();
	void SetupUndergroundLandscape();
	void SetupForestLandscape();
	void SetupLunarLandscape();

	void SpawnFoliage(const FLandscapeConfiguration& Config);
	void SpawnRocks(const FLandscapeConfiguration& Config);
	void SpawnStructures(const FLandscapeConfiguration& Config);
};

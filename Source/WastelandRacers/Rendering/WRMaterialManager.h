#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameterCollection.h"
#include "WRMaterialManager.generated.h"

UENUM(BlueprintType)
enum class EMaterialType : uint8
{
	TrackAsphalt,
	TrackDirt,
	TrackMetal,
	TrackConcrete,
	VehiclePaint,
	VehicleMetal,
	VehicleRubber,
	EnvironmentRock,
	EnvironmentVegetation,
	EnvironmentBuilding,
	WeaponMetal,
	EffectParticle
};

USTRUCT(BlueprintType)
struct FMaterialSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* DamagedVariant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* WetVariant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UTexture2D*> DiffuseTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UTexture2D*> NormalTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UTexture2D*> RoughnessTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UTexture2D*> MetallicTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TilingScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BaseColor = FLinearColor::White;
};

UCLASS()
class WASTELANDRACERS_API UWRMaterialManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Materials")
	static UWRMaterialManager* GetInstance(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "Materials")
	class UMaterialInterface* GetMaterial(EMaterialType MaterialType, int32 VariantIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Materials")
	void ApplyWeatherEffects(bool bIsRaining, float Intensity);

	UFUNCTION(BlueprintCallable, Category = "Materials")
	void SetTimeOfDay(float TimeOfDay);

	UFUNCTION(BlueprintCallable, Category = "Materials")
	class UMaterialInstanceDynamic* CreateDynamicMaterial(EMaterialType MaterialType, class UPrimitiveComponent* Component);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TMap<EMaterialType, FMaterialSettings> MaterialDatabase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialParameterCollection* GlobalMaterialParameters;

private:
	void LoadDefaultMaterials();
	void SetupPBRParameters();
};

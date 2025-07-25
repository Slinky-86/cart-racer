#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Materials/MaterialInterface.h"
#include "WRAssetManager.generated.h"

USTRUCT(BlueprintType)
struct FAssetMaterialMapping : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AssetPattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInterface> MaterialPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bApplyToStaticMesh = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bApplyToSkeletalMesh = true;
};

USTRUCT(BlueprintType)
struct FEnvironmentLighting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SunIntensity = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor SunColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkyIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor FogColor = FLinearColor::Gray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FogDensity = 0.02f;
};

UCLASS()
class WASTELANDRACERS_API UWRAssetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Asset Management")
	static UWRAssetManager* GetInstance(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "Asset Management")
	void ScanAndApplyMaterials();

	UFUNCTION(BlueprintCallable, Category = "Asset Management")
	void ApplyMaterialToMesh(class UMeshComponent* MeshComponent, const FString& AssetName);

	UFUNCTION(BlueprintCallable, Category = "Asset Management")
	void SetupEnvironmentLighting(const FString& EnvironmentName);

	UFUNCTION(BlueprintCallable, Category = "Asset Management")
	void OptimizeAssetsForMobile();

	UFUNCTION(BlueprintCallable, Category = "Asset Management")
	void LoadTextureLibrary();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Management")
	class UDataTable* MaterialMappingTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Management")
	class UDataTable* LightingPresetsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Management")
	TMap<FString, TSoftObjectPtr<UMaterialInterface>> LoadedMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Management")
	TMap<FString, TSoftObjectPtr<UTexture2D>> TextureLibrary;

private:
	void ScanForAssets();
	void ApplyRealisticMaterials();
	void SetupLODSystems();
	bool MatchesPattern(const FString& AssetName, const FString& Pattern) const;
};

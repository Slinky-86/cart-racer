#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WastelandRacers/Tracks/WRTrackManager.h"
#include "WastelandRacers/Tracks/WRShortcutSystem.h"
#include "WRTrackVariations.generated.h"

UENUM(BlueprintType)
enum class ETrackShape : uint8
{
	Oval,
	Figure8,
	Circuit,
	PointToPoint,
	Spiral,
	Serpentine,
	Complex
};

UENUM(BlueprintType)
enum class ETrackSurface : uint8
{
	Asphalt,
	Dirt,
	Sand,
	Metal,
	Concrete,
	Ice,
	Mud,
	Rock
};

USTRUCT(BlueprintType)
struct FTrackVariation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VariationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETrackShape TrackShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETrackSurface PrimarySurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETrackSurface> SurfaceVariations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TrackWidth = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfTurns = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ElevationChange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasJumps = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasTunnels = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBridges = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> ControlPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> AlternateRoutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FShortcutData> TrackShortcuts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasSecretAreas = false;
};

USTRUCT(BlueprintType)
struct FTrackVariationList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTrackVariation> Variations;
};

UCLASS()
class WASTELANDRACERS_API AWRTrackVariations : public AActor
{
	GENERATED_BODY()

public:
	AWRTrackVariations();

	UFUNCTION(BlueprintCallable, Category = "Track Generation")
	void GenerateTrackVariation(ETrackType BaseTrack, ETrackShape Shape);

	UFUNCTION(BlueprintCallable, Category = "Track Generation")
	TArray<FTrackVariation> GetAllVariationsForTrack(ETrackType TrackType);

	UFUNCTION(BlueprintCallable, Category = "Track Generation")
	void CreateSplineFromControlPoints(const TArray<FVector>& ControlPoints);

	UFUNCTION(BlueprintCallable, Category = "Track Generation")
	void GenerateTrackMesh(const FTrackVariation& Variation);

	UFUNCTION(BlueprintCallable, Category = "Track Generation")
	void SetupShortcutsForVariation(const FTrackVariation& Variation);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Variations")
	TMap<ETrackType, FTrackVariationList> TrackVariations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineComponent* TrackSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProceduralMeshComponent* TrackMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TMap<ETrackSurface, class UMaterialInterface*> SurfaceMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shortcuts")
	class AWRShortcutSystem* ShortcutSystem;

private:
	void InitializePandoraVariations();
	void InitializeOpportunityVariations();
	void InitializeEridiumVariations();
	void InitializeWildlifeVariations();
	void InitializeHyperionVariations();

	void GenerateOvalTrack(const FTrackVariation& Variation);
	void GenerateFigure8Track(const FTrackVariation& Variation);
	void GenerateCircuitTrack(const FTrackVariation& Variation);
	void GeneratePointToPointTrack(const FTrackVariation& Variation);
	void GenerateSpiralTrack(const FTrackVariation& Variation);
	void GenerateSerpentineTrack(const FTrackVariation& Variation);
	void GenerateComplexTrack(const FTrackVariation& Variation);

	void ApplySurfaceMaterial(ETrackSurface Surface);
	void AddTrackDetails(const FTrackVariation& Variation);
};

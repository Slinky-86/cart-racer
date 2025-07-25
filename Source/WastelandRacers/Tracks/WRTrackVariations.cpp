#include "WRTrackVariations.h"
#include "Components/SplineComponent.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "WastelandRacers/Tracks/WRShortcutSystem.h"
#include "Engine/Engine.h"

AWRTrackVariations::AWRTrackVariations()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create spline component for track path
	TrackSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrackSpline"));
	RootComponent = TrackSpline;

	// Create procedural mesh for track surface
	TrackMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TrackMesh"));
	TrackMesh->SetupAttachment(RootComponent);

	// Initialize all track variations
	InitializePandoraVariations();
	InitializeOpportunityVariations();
	InitializeEridiumVariations();
	InitializeWildlifeVariations();
	InitializeHyperionVariations();

	// Create shortcut system
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	ShortcutSystem = GetWorld()->SpawnActor<AWRShortcutSystem>(AWRShortcutSystem::StaticClass(), SpawnParams);
}

void AWRTrackVariations::GenerateTrackVariation(ETrackType BaseTrack, ETrackShape Shape)
{
	FTrackVariationList* VariationList = TrackVariations.Find(BaseTrack);
	if (!VariationList)
		return;

	// Find variation with matching shape
	for (const FTrackVariation& Variation : VariationList->Variations)
	{
		if (Variation.TrackShape == Shape)
		{
			switch (Shape)
			{
				case ETrackShape::Oval:
					GenerateOvalTrack(Variation);
					break;
				case ETrackShape::Figure8:
					GenerateFigure8Track(Variation);
					break;
				case ETrackShape::Circuit:
					GenerateCircuitTrack(Variation);
					break;
				case ETrackShape::PointToPoint:
					GeneratePointToPointTrack(Variation);
					break;
				case ETrackShape::Spiral:
					GenerateSpiralTrack(Variation);
					break;
				case ETrackShape::Serpentine:
					GenerateSerpentineTrack(Variation);
					break;
				case ETrackShape::Complex:
					GenerateComplexTrack(Variation);
					break;
			}

			GenerateTrackMesh(Variation);
			ApplySurfaceMaterial(Variation.PrimarySurface);
			AddTrackDetails(Variation);
			SetupShortcutsForVariation(Variation);
			break;
		}
	}
}

TArray<FTrackVariation> AWRTrackVariations::GetAllVariationsForTrack(ETrackType TrackType)
{
	if (FTrackVariationList* VariationList = TrackVariations.Find(TrackType))
	{
		return VariationList->Variations;
	}
	return TArray<FTrackVariation>();
}

void AWRTrackVariations::CreateSplineFromControlPoints(const TArray<FVector>& ControlPoints)
{
	TrackSpline->ClearSplinePoints();
	
	for (int32 i = 0; i < ControlPoints.Num(); i++)
	{
		TrackSpline->AddSplinePoint(ControlPoints[i], ESplineCoordinateSpace::World);
	}

	TrackSpline->SetClosedLoop(true);
	TrackSpline->UpdateSpline();
}

void AWRTrackVariations::GenerateTrackMesh(const FTrackVariation& Variation)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;

	float TrackHalfWidth = Variation.TrackWidth * 0.5f;
	int32 SplinePoints = TrackSpline->GetNumberOfSplinePoints();

	// Generate vertices along spline
	for (int32 i = 0; i < SplinePoints; i++)
	{
		float Distance = TrackSpline->GetDistanceAlongSplineAtSplinePoint(i);
		FVector Location = TrackSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		FVector Right = TrackSpline->GetRightVectorAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

		// Left edge
		Vertices.Add(Location - Right * TrackHalfWidth);
		// Right edge
		Vertices.Add(Location + Right * TrackHalfWidth);

		// UVs
		float U = (float)i / (float)(SplinePoints - 1);
		UVs.Add(FVector2D(U, 0.0f));
		UVs.Add(FVector2D(U, 1.0f));

		// Normals
		Normals.Add(FVector::UpVector);
		Normals.Add(FVector::UpVector);
	}

	// Generate triangles
	for (int32 i = 0; i < SplinePoints - 1; i++)
	{
		int32 Current = i * 2;
		int32 Next = (i + 1) * 2;

		// First triangle
		Triangles.Add(Current);
		Triangles.Add(Next);
		Triangles.Add(Current + 1);

		// Second triangle
		Triangles.Add(Current + 1);
		Triangles.Add(Next);
		Triangles.Add(Next + 1);
	}

	TrackMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void AWRTrackVariations::SetupShortcutsForVariation(const FTrackVariation& Variation)
{
	if (ShortcutSystem)
	{
		for (const FShortcutData& Shortcut : Variation.TrackShortcuts)
		{
			ShortcutSystem->RegisterShortcut(Shortcut);
		}
	}
}

void AWRTrackVariations::InitializePandoraVariations()
{
	FTrackVariationList PandoraVariationList;

	// Oval Track
	FTrackVariation OvalVariation;
	OvalVariation.VariationName = TEXT("Pandora Oval Speedway");
	OvalVariation.TrackShape = ETrackShape::Oval;
	OvalVariation.PrimarySurface = ETrackSurface::Asphalt;
	OvalVariation.TrackWidth = 1000.0f;
	OvalVariation.NumberOfTurns = 4;
	OvalVariation.bHasJumps = true;
	PandoraVariationList.Variations.Add(OvalVariation);

	// Figure-8 Track
	FTrackVariation Figure8Variation;
	Figure8Variation.VariationName = TEXT("Pandora Crossroads");
	Figure8Variation.TrackShape = ETrackShape::Figure8;
	Figure8Variation.PrimarySurface = ETrackSurface::Dirt;
	Figure8Variation.TrackWidth = 800.0f;
	Figure8Variation.NumberOfTurns = 8;
	Figure8Variation.bHasBridges = true;
	PandoraVariationList.Variations.Add(Figure8Variation);

	// Complex Circuit
	FTrackVariation ComplexVariation;
	ComplexVariation.VariationName = TEXT("Pandora Wasteland Circuit");
	ComplexVariation.TrackShape = ETrackShape::Complex;
	ComplexVariation.PrimarySurface = ETrackSurface::Sand;
	ComplexVariation.SurfaceVariations = {ETrackSurface::Asphalt, ETrackSurface::Dirt, ETrackSurface::Rock};
	ComplexVariation.TrackWidth = 900.0f;
	ComplexVariation.NumberOfTurns = 12;
	ComplexVariation.ElevationChange = 300.0f;
	ComplexVariation.bHasJumps = true;
	ComplexVariation.bHasTunnels = true;
	ComplexVariation.bHasSecretAreas = true;
	
	// Add shortcuts to complex variation
	FShortcutData CaveShortcut;
	CaveShortcut.ShortcutType = EShortcutType::Hidden;
	CaveShortcut.Difficulty = EShortcutDifficulty::Medium;
	CaveShortcut.ShortcutName = TEXT("Bandit Cave Network");
	CaveShortcut.TimeSaveSeconds = 4.2f;
	CaveShortcut.EntryPoint = FVector(800, 600, 100);
	CaveShortcut.ExitPoint = FVector(1200, 1000, 100);
	ComplexVariation.TrackShortcuts.Add(CaveShortcut);
	
	PandoraVariationList.Variations.Add(ComplexVariation);

	TrackVariations.Add(ETrackType::PandoraDesert, PandoraVariationList);
}

void AWRTrackVariations::InitializeOpportunityVariations()
{
	FTrackVariationList OpportunityVariationList;

	// Urban Circuit
	FTrackVariation UrbanCircuit;
	UrbanCircuit.VariationName = TEXT("Opportunity Street Circuit");
	UrbanCircuit.TrackShape = ETrackShape::Circuit;
	UrbanCircuit.PrimarySurface = ETrackSurface::Asphalt;
	UrbanCircuit.TrackWidth = 700.0f;
	UrbanCircuit.NumberOfTurns = 16;
	UrbanCircuit.ElevationChange = 400.0f;
	UrbanCircuit.bHasBridges = true;
	OpportunityVariationList.Variations.Add(UrbanCircuit);

	// Spiral Tower
	FTrackVariation SpiralTower;
	SpiralTower.VariationName = TEXT("Hyperion Tower Spiral");
	SpiralTower.TrackShape = ETrackShape::Spiral;
	SpiralTower.PrimarySurface = ETrackSurface::Metal;
	SpiralTower.TrackWidth = 600.0f;
	SpiralTower.NumberOfTurns = 20;
	SpiralTower.ElevationChange = 800.0f;
	SpiralTower.bHasSecretAreas = true;
	
	// Add elevator shortcut
	FShortcutData ElevatorShortcut;
	ElevatorShortcut.ShortcutType = EShortcutType::Environmental;
	ElevatorShortcut.Difficulty = EShortcutDifficulty::Expert;
	ElevatorShortcut.ShortcutName = TEXT("Express Elevator");
	ElevatorShortcut.TimeSaveSeconds = 8.5f;
	ElevatorShortcut.EntryPoint = FVector(1200, 400, 200);
	ElevatorShortcut.ExitPoint = FVector(800, 800, 600);
	SpiralTower.TrackShortcuts.Add(ElevatorShortcut);
	
	OpportunityVariationList.Variations.Add(SpiralTower);

	TrackVariations.Add(ETrackType::OpportunityRuins, OpportunityVariationList);
}

void AWRTrackVariations::InitializeEridiumVariations()
{
	FTrackVariationList EridiumVariationList;

	// Underground Serpentine
	FTrackVariation UndergroundSerpentine;
	UndergroundSerpentine.VariationName = TEXT("Eridium Cavern Serpentine");
	UndergroundSerpentine.TrackShape = ETrackShape::Serpentine;
	UndergroundSerpentine.PrimarySurface = ETrackSurface::Rock;
	UndergroundSerpentine.TrackWidth = 750.0f;
	UndergroundSerpentine.NumberOfTurns = 24;
	UndergroundSerpentine.bHasTunnels = true;
	UndergroundSerpentine.bHasSecretAreas = true;
	
	// Add crystal cavern shortcut
	FShortcutData CrystalShortcut;
	CrystalShortcut.ShortcutType = EShortcutType::Hidden;
	CrystalShortcut.Difficulty = EShortcutDifficulty::Hard;
	CrystalShortcut.ShortcutName = TEXT("Eridium Crystal Chamber");
	CrystalShortcut.TimeSaveSeconds = 5.8f;
	CrystalShortcut.EntryPoint = FVector(600, 0, 50);
	CrystalShortcut.ExitPoint = FVector(200, 600, 25);
	CrystalShortcut.bHasHazards = true;
	UndergroundSerpentine.TrackShortcuts.Add(CrystalShortcut);
	
	EridiumVariationList.Variations.Add(UndergroundSerpentine);

	TrackVariations.Add(ETrackType::EridiumMines, EridiumVariationList);
}

void AWRTrackVariations::InitializeWildlifeVariations()
{
	FTrackVariationList WildlifeVariationList;

	// Forest Circuit
	FTrackVariation ForestCircuit;
	ForestCircuit.VariationName = TEXT("Wildlife Preserve Circuit");
	ForestCircuit.TrackShape = ETrackShape::Circuit;
	ForestCircuit.PrimarySurface = ETrackSurface::Dirt;
	ForestCircuit.SurfaceVariations = {ETrackSurface::Mud, ETrackSurface::Concrete};
	ForestCircuit.TrackWidth = 850.0f;
	ForestCircuit.NumberOfTurns = 14;
	ForestCircuit.ElevationChange = 250.0f;
	ForestCircuit.bHasJumps = true;
	ForestCircuit.bHasSecretAreas = true;
	
	// Add canopy shortcut
	FShortcutData CanopyShortcut;
	CanopyShortcut.ShortcutType = EShortcutType::Elevated;
	CanopyShortcut.Difficulty = EShortcutDifficulty::Medium;
	CanopyShortcut.ShortcutName = TEXT("Treetop Highway");
	CanopyShortcut.TimeSaveSeconds = 3.2f;
	CanopyShortcut.RequiredSpeed = 850.0f;
	CanopyShortcut.EntryPoint = FVector(-600, 700, 120);
	CanopyShortcut.ExitPoint = FVector(-200, 350, 180);
	ForestCircuit.TrackShortcuts.Add(CanopyShortcut);
	
	WildlifeVariationList.Variations.Add(ForestCircuit);

	TrackVariations.Add(ETrackType::WildlifePreserve, WildlifeVariationList);
}

void AWRTrackVariations::InitializeHyperionVariations()
{
	FTrackVariationList HyperionVariationList;

	// Lunar Circuit
	FTrackVariation LunarCircuit;
	LunarCircuit.VariationName = TEXT("Elpis Low-Gravity Circuit");
	LunarCircuit.TrackShape = ETrackShape::Circuit;
	LunarCircuit.PrimarySurface = ETrackSurface::Metal;
	LunarCircuit.TrackWidth = 900.0f;
	LunarCircuit.NumberOfTurns = 10;
	LunarCircuit.ElevationChange = 150.0f;
	LunarCircuit.bHasJumps = true;
	LunarCircuit.bHasBridges = true;
	LunarCircuit.bHasSecretAreas = true;
	
	// Add zero-gravity tube shortcut
	FShortcutData ZeroGShortcut;
	ZeroGShortcut.ShortcutType = EShortcutType::Environmental;
	ZeroGShortcut.Difficulty = EShortcutDifficulty::Expert;
	ZeroGShortcut.ShortcutName = TEXT("Zero-G Transport Tube");
	ZeroGShortcut.TimeSaveSeconds = 7.5f;
	ZeroGShortcut.EntryPoint = FVector(1200, 0, 300);
	ZeroGShortcut.ExitPoint = FVector(600, 1400, 500);
	LunarCircuit.TrackShortcuts.Add(ZeroGShortcut);
	
	HyperionVariationList.Variations.Add(LunarCircuit);

	TrackVariations.Add(ETrackType::HyperionMoonBase, HyperionVariationList);
}

void AWRTrackVariations::GenerateOvalTrack(const FTrackVariation& Variation)
{
	TArray<FVector> ControlPoints;
	float Radius = 2000.0f;
	int32 Points = 32;

	for (int32 i = 0; i < Points; i++)
	{
		float Angle = (float)i / (float)Points * 2.0f * PI;
		FVector Point = FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius * 0.6f, 0.0f);
		ControlPoints.Add(Point);
	}

	CreateSplineFromControlPoints(ControlPoints);
}

void AWRTrackVariations::GenerateFigure8Track(const FTrackVariation& Variation)
{
	TArray<FVector> ControlPoints;
	float Radius = 1500.0f;
	int32 Points = 64;

	for (int32 i = 0; i < Points; i++)
	{
		float t = (float)i / (float)Points * 2.0f * PI;
		float x = Radius * FMath::Sin(t);
		float y = Radius * FMath::Sin(t) * FMath::Cos(t);
		FVector Point = FVector(x, y, 0.0f);
		ControlPoints.Add(Point);
	}

	CreateSplineFromControlPoints(ControlPoints);
}

void AWRTrackVariations::GenerateCircuitTrack(const FTrackVariation& Variation)
{
	// Generate a complex circuit with multiple turns and elevation changes
	TArray<FVector> ControlPoints;
	
	// This would be expanded with more sophisticated track generation
	float BaseRadius = 1800.0f;
	int32 Points = Variation.NumberOfTurns * 4;

	for (int32 i = 0; i < Points; i++)
	{
		float Angle = (float)i / (float)Points * 2.0f * PI;
		float RadiusVariation = BaseRadius + FMath::Sin(Angle * 3.0f) * 400.0f;
		float Height = FMath::Sin(Angle * 2.0f) * Variation.ElevationChange;
		
		FVector Point = FVector(
			FMath::Cos(Angle) * RadiusVariation,
			FMath::Sin(Angle) * RadiusVariation,
			Height
		);
		ControlPoints.Add(Point);
	}

	CreateSplineFromControlPoints(ControlPoints);
}

void AWRTrackVariations::GeneratePointToPointTrack(const FTrackVariation& Variation)
{
	// Point-to-point rally-style track
	TArray<FVector> ControlPoints;
	
	FVector Start = FVector::ZeroVector;
	FVector End = FVector(5000.0f, 2000.0f, 500.0f);
	
	int32 Points = 20;
	for (int32 i = 0; i < Points; i++)
	{
		float Alpha = (float)i / (float)(Points - 1);
		FVector Point = FMath::Lerp(Start, End, Alpha);
		
		// Add some randomness for interesting turns
		Point.Y += FMath::Sin(Alpha * PI * 4.0f) * 800.0f;
		Point.Z += FMath::Sin(Alpha * PI * 2.0f) * 300.0f;
		
		ControlPoints.Add(Point);
	}

	TrackSpline->ClearSplinePoints();
	for (int32 i = 0; i < ControlPoints.Num(); i++)
	{
		TrackSpline->AddSplinePoint(ControlPoints[i], ESplineCoordinateSpace::World);
	}
	TrackSpline->SetClosedLoop(false);
	TrackSpline->UpdateSpline();
}

void AWRTrackVariations::GenerateSpiralTrack(const FTrackVariation& Variation)
{
	TArray<FVector> ControlPoints;
	float MaxRadius = 2000.0f;
	int32 Points = 100;
	float HeightPerLoop = 200.0f;

	for (int32 i = 0; i < Points; i++)
	{
		float t = (float)i / (float)Points;
		float Angle = t * PI * 8.0f; // 4 loops
		float Radius = MaxRadius * (1.0f - t * 0.8f);
		float Height = t * HeightPerLoop * 4.0f;
		
		FVector Point = FVector(
			FMath::Cos(Angle) * Radius,
			FMath::Sin(Angle) * Radius,
			Height
		);
		ControlPoints.Add(Point);
	}

	CreateSplineFromControlPoints(ControlPoints);
}

void AWRTrackVariations::GenerateSerpentineTrack(const FTrackVariation& Variation)
{
	TArray<FVector> ControlPoints;
	int32 Points = 50;
	float Length = 4000.0f;

	for (int32 i = 0; i < Points; i++)
	{
		float t = (float)i / (float)(Points - 1);
		float x = t * Length;
		float y = FMath::Sin(t * PI * 6.0f) * 1000.0f;
		float z = FMath::Sin(t * PI * 3.0f) * Variation.ElevationChange;
		
		ControlPoints.Add(FVector(x, y, z));
	}

	CreateSplineFromControlPoints(ControlPoints);
}

void AWRTrackVariations::GenerateComplexTrack(const FTrackVariation& Variation)
{
	// Combine multiple track elements for a complex layout
	GenerateCircuitTrack(Variation);
	
	// Add additional complexity with alternate routes
	// This would be expanded with more sophisticated generation
}

void AWRTrackVariations::ApplySurfaceMaterial(ETrackSurface Surface)
{
	if (UMaterialInterface** Material = SurfaceMaterials.Find(Surface))
	{
		TrackMesh->SetMaterial(0, *Material);
	}
}

void AWRTrackVariations::AddTrackDetails(const FTrackVariation& Variation)
{
	// Add barriers, decorations, and other track details
	// This would spawn additional mesh components for track details
}

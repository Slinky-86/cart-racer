#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WRTrackManager.generated.h"

UENUM(BlueprintType)
enum class ETrackType : uint8
{
	// Pandora Desert shapes
	PandoraDesert_Oval,
	PandoraDesert_Figure8,
	PandoraDesert_Circuit,
	PandoraDesert_PointToPoint,
	PandoraDesert_Spiral,
	PandoraDesert_Serpentine,
	PandoraDesert_Complex,

	// Opportunity Ruins shapes
	OpportunityRuins_Oval,
	OpportunityRuins_Figure8,
	OpportunityRuins_Circuit,
	OpportunityRuins_PointToPoint,
	OpportunityRuins_Spiral,
	OpportunityRuins_Serpentine,
	OpportunityRuins_Complex,

	// Eridium Mines shapes
	EridiumMines_Oval,
	EridiumMines_Figure8,
	EridiumMines_Circuit,
	EridiumMines_PointToPoint,
	EridiumMines_Spiral,
	EridiumMines_Serpentine,
	EridiumMines_Complex,

	// Wildlife Preserve shapes
	WildlifePreserve_Oval,
	WildlifePreserve_Figure8,
	WildlifePreserve_Circuit,
	WildlifePreserve_PointToPoint,
	WildlifePreserve_Spiral,
	WildlifePreserve_Serpentine,
	WildlifePreserve_Complex,

	// Hyperion Moon Base shapes
	HyperionMoonBase_Oval,
	HyperionMoonBase_Figure8,
	HyperionMoonBase_Circuit,
	HyperionMoonBase_PointToPoint,
	HyperionMoonBase_Spiral,
	HyperionMoonBase_Serpentine,
	HyperionMoonBase_Complex,

	// Aliases for backward compatibility (these point to the first variation in each set)
	PandoraDesert = PandoraDesert_Oval,
	OpportunityRuins = OpportunityRuins_Oval,
	EridiumMines = EridiumMines_Oval,
	WildlifePreserve = WildlifePreserve_Oval,
	HyperionMoonBase = HyperionMoonBase_Oval
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaceCompleted, ETrackType, Track, bool, bPlayerWon);

UCLASS()
class WASTELANDRACERS_API AWRTrackManager : public AActor
{
	GENERATED_BODY()

public:
	AWRTrackManager();

	UFUNCTION(BlueprintCallable)
	void LoadTrack(ETrackType TrackType);

	UFUNCTION(BlueprintCallable)
	void FinishRace(bool bPlayerWon);

	UPROPERTY(BlueprintAssignable)
	FOnRaceCompleted OnRaceCompleted;

	ETrackType CurrentTrack;

	// Optionally, you may wish to expose starting positions
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> StartingPositions;
};

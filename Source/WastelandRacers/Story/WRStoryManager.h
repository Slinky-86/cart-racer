#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WRStoryManager.generated.h"

UENUM(BlueprintType)
enum class EStoryWorld : uint8
{
	Pandora UMETA(DisplayName = "Pandora"),
	Opportunity UMETA(DisplayName = "Opportunity"),
	EridiumMines UMETA(DisplayName = "Eridium Mines"),
	WildlifePreserve UMETA(DisplayName = "Wildlife Preserve"),
	HyperionMoonBase UMETA(DisplayName = "Hyperion Moon Base")
};

UENUM(BlueprintType)
enum class ETrackType : uint8
{
	PandoraDesert UMETA(DisplayName = "Pandora Desert"),
	OpportunityRuins UMETA(DisplayName = "Opportunity Ruins"),
	EridiumMines UMETA(DisplayName = "Eridium Mines"),
	WildlifePreserve UMETA(DisplayName = "Wildlife Preserve"),
	HyperionMoonBase UMETA(DisplayName = "Hyperion Moon Base")
};

UCLASS(BlueprintType)
class WASTELANDRACERS_API UWRStoryManager : public UDataAsset
{
	GENERATED_BODY()

public:
	UWRStoryManager();

	UFUNCTION(BlueprintCallable, Category = "Story")
	void UnlockWorld(EStoryWorld World);

	UFUNCTION(BlueprintCallable, Category = "Story")
	bool IsWorldUnlocked(EStoryWorld World) const;

	UFUNCTION(BlueprintCallable, Category = "Story")
	void UnlockTrack(ETrackType Track);

	UFUNCTION(BlueprintCallable, Category = "Story")
	bool IsTrackUnlocked(ETrackType Track) const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Story")
	TArray<EStoryWorld> UnlockedWorlds;

	UPROPERTY(BlueprintReadOnly, Category = "Story")
	TArray<ETrackType> UnlockedTracks;
};

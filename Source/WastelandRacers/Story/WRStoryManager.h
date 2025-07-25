#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WastelandRacers/Tracks/WRTrackManager.h"
#include "WRStoryTypes.h"
#include "WRStorySaveGame.h"
#include "WRStoryManager.generated.h"

USTRUCT(BlueprintType)
struct FStoryWorldData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStoryWorld WorldType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WorldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WorldDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> WorldLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* WorldPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETrackType> AvailableTracks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECupSeries RequiredCup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UnlockRequirement = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UnlockDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PlayerSpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> RacePortalLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* WorldMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* AmbientSound;
};

USTRUCT(BlueprintType)
struct FCupSeriesData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECupSeries CupType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CupName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CupDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETrackType> CupTracks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredWins = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RacePointsReward = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EridiumReward = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStoryWorld UnlocksWorld;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldUnlocked, EStoryWorld, UnlockedWorld);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCupCompleted, ECupSeries, CompletedCup);

UCLASS()
class WASTELANDRACERS_API AWRStoryManager : public AActor
{
	GENERATED_BODY()

public:
	AWRStoryManager();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Story")
	void LoadWorld(EStoryWorld WorldType);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void UnlockWorld(EStoryWorld WorldType);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void CompleteCup(ECupSeries CupType);

	UFUNCTION(BlueprintPure, Category = "Story")
	bool IsWorldUnlocked(EStoryWorld WorldType) const;

	UFUNCTION(BlueprintPure, Category = "Story")
	bool IsCupCompleted(ECupSeries CupType) const;

	UFUNCTION(BlueprintPure, Category = "Story")
	FStoryWorldData GetWorldData(EStoryWorld WorldType) const;

	UFUNCTION(BlueprintPure, Category = "Story")
	FCupSeriesData GetCupData(ECupSeries CupType) const;

	UFUNCTION(BlueprintPure, Category = "Story")
	TArray<FStoryWorldData> GetAllWorlds() const;

	UFUNCTION(BlueprintPure, Category = "Story")
	TArray<FCupSeriesData> GetAllCups() const;

	UFUNCTION(BlueprintCallable, Category = "Story")
	void TravelToRace(ETrackType TrackType);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void ReturnToWorld();

	UPROPERTY(BlueprintAssignable)
	FOnWorldUnlocked OnWorldUnlocked;

	UPROPERTY(BlueprintAssignable)
	FOnCupCompleted OnCupCompleted;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	TMap<EStoryWorld, FStoryWorldData> StoryWorlds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	TMap<ECupSeries, FCupSeriesData> CupSeries;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Story")
	EStoryWorld CurrentWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Story")
	bool bInFreeRoam = true;

private:
	void InitializeStoryData();
	void SetupPandoraWorld();
	void SetupOpportunityWorld();
	void SetupEridiumWorld();
	void SetupWildlifeWorld();
	void SetupHyperionWorld();
	void SetupCupSeries();

	// Save-game persistence
	void LoadProgress();
	void SaveProgress() const;

	// Race flow handler
	UFUNCTION()
	void HandleRaceCompleted(ETrackType Track, bool bPlayerWon);
};

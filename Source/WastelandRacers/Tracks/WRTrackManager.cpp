#include "WRTrackManager.h"
#include "WastelandRacers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "EngineUtils.h"
#include "Logging/LogMacros.h"

AWRTrackManager::AWRTrackManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentTrack = ETrackType::PandoraDesert;
}

void AWRTrackManager::LoadTrack(ETrackType TrackType)
{
	CurrentTrack = TrackType;
	// TODO: Actually load the track assets/level here if needed
	UE_LOG(LogWastelandRacers, Log, TEXT("WRTrackManager: Loaded track %d"), (int32)TrackType);
}

void AWRTrackManager::FinishRace(bool bPlayerWon)
{
	UE_LOG(LogWastelandRacers, Log, TEXT("WRTrackManager: Race finished. PlayerWon=%d, Track=%d"), bPlayerWon ? 1 : 0, (int32)CurrentTrack);
	OnRaceCompleted.Broadcast(CurrentTrack, bPlayerWon);
}

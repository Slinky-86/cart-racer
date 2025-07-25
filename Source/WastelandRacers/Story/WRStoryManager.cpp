#include "WRStoryManager.h"
#include "WastelandRacers/WastelandRacers.h"

UWRStoryManager::UWRStoryManager()
{
	// Start with Pandora unlocked
	UnlockedWorlds.Add(EStoryWorld::Pandora);
	UnlockedTracks.Add(ETrackType::PandoraDesert);
}

void UWRStoryManager::UnlockWorld(EStoryWorld World)
{
	if (!UnlockedWorlds.Contains(World))
	{
		UnlockedWorlds.Add(World);
		UE_LOG(LogWastelandRacers, Log, TEXT("Unlocked world: %d"), (int32)World);
	}
}

bool UWRStoryManager::IsWorldUnlocked(EStoryWorld World) const
{
	return UnlockedWorlds.Contains(World);
}

void UWRStoryManager::UnlockTrack(ETrackType Track)
{
	if (!UnlockedTracks.Contains(Track))
	{
		UnlockedTracks.Add(Track);
		UE_LOG(LogWastelandRacers, Log, TEXT("Unlocked track: %d"), (int32)Track);
	}
}

bool UWRStoryManager::IsTrackUnlocked(ETrackType Track) const
{
	return UnlockedTracks.Contains(Track);
}

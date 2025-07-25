#include "WRStoryManager.h"
#include "WastelandRacers.h"
#include "WastelandRacers/Audio/WRAudioManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "EngineUtils.h"

AWRStoryManager::AWRStoryManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentWorld = EStoryWorld::Pandora;
}

void AWRStoryManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeStoryData();
	LoadProgress();
}

void AWRStoryManager::LoadWorld(EStoryWorld WorldType)
{
	if (!IsWorldUnlocked(WorldType))
	{
		UE_LOG(LogTemp, Warning, TEXT("World %d is not unlocked"), (int32)WorldType);
		return;
	}

	CurrentWorld = WorldType;
	bInFreeRoam = true;

	FStoryWorldData WorldData = GetWorldData(WorldType);
	
	// Load the world level
	if (WorldData.WorldLevel.IsValid())
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*WorldData.WorldLevel.GetAssetName()));
	}

	// Setup audio
	if (UWRAudioManager* AudioManager = UWRAudioManager::GetInstance(GetWorld()))
	{
		if (WorldData.WorldMusic)
		{
			AudioManager->PlayMusic(WorldData.WorldMusic);
		}
		if (WorldData.AmbientSound)
		{
			AudioManager->PlayAmbientSound(WorldData.AmbientSound);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Loaded world: %s"), *WorldData.WorldName);
}

void AWRStoryManager::UnlockWorld(EStoryWorld WorldType)
{
	if (FStoryWorldData* WorldData = StoryWorlds.Find(WorldType))
	{
		if (!WorldData->bIsUnlocked)
		{
			WorldData->bIsUnlocked = true;
			OnWorldUnlocked.Broadcast(WorldType);
			UE_LOG(LogWastelandRacers, Log, TEXT("Unlocked world: %s"), *WorldData->WorldName);

			SaveProgress();
		}
	}
}

void AWRStoryManager::CompleteCup(ECupSeries CupType)
{
	if (FCupSeriesData* CupData = CupSeries.Find(CupType))
	{
		if (!CupData->bIsCompleted)
		{
			CupData->bIsCompleted = true;

			// Unlock associated world
			UnlockWorld(CupData->UnlocksWorld);

			OnCupCompleted.Broadcast(CupType);
			UE_LOG(LogWastelandRacers, Log, TEXT("Completed cup: %s"), *CupData->CupName);

			SaveProgress();
		}
	}
}

bool AWRStoryManager::IsWorldUnlocked(EStoryWorld WorldType) const
{
	if (const FStoryWorldData* WorldData = StoryWorlds.Find(WorldType))
	{
		return WorldData->bIsUnlocked;
	}
	return false;
}

bool AWRStoryManager::IsCupCompleted(ECupSeries CupType) const
{
	if (const FCupSeriesData* CupData = CupSeries.Find(CupType))
	{
		return CupData->bIsCompleted;
	}
	return false;
}

FStoryWorldData AWRStoryManager::GetWorldData(EStoryWorld WorldType) const
{
	if (const FStoryWorldData* WorldData = StoryWorlds.Find(WorldType))
	{
		return *WorldData;
	}
	return FStoryWorldData();
}

FCupSeriesData AWRStoryManager::GetCupData(ECupSeries CupType) const
{
	if (const FCupSeriesData* CupData = CupSeries.Find(CupType))
	{
		return *CupData;
	}
	return FCupSeriesData();
}

TArray<FStoryWorldData> AWRStoryManager::GetAllWorlds() const
{
	TArray<FStoryWorldData> AllWorlds;
	for (const auto& WorldPair : StoryWorlds)
	{
		AllWorlds.Add(WorldPair.Value);
	}
	return AllWorlds;
}

TArray<FCupSeriesData> AWRStoryManager::GetAllCups() const
{
	TArray<FCupSeriesData> AllCups;
	for (const auto& CupPair : CupSeries)
	{
		AllCups.Add(CupPair.Value);
	}
	return AllCups;
}

void AWRStoryManager::TravelToRace(ETrackType TrackType)
{
	bInFreeRoam = false;

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("TravelToRace: World is null"));
		return;
	}

	AWRTrackManager* TrackManager = nullptr;
	for (TActorIterator<AWRTrackManager> It(World); It; ++It)
	{
		TrackManager = *It;
		break;
	}
	if (!TrackManager)
	{
		FActorSpawnParameters SpawnParams;
		TrackManager = World->SpawnActor<AWRTrackManager>(AWRTrackManager::StaticClass(), SpawnParams);
		if (!TrackManager)
		{
			UE_LOG(LogWastelandRacers, Error, TEXT("TravelToRace: Failed to spawn AWRTrackManager!"));
			return;
		}
		else
		{
			UE_LOG(LogWastelandRacers, Log, TEXT("TravelToRace: Spawned new AWRTrackManager"));
		}
	}
	else
	{
		UE_LOG(LogWastelandRacers, Log, TEXT("TravelToRace: Found existing AWRTrackManager"));
	}

	TrackManager->OnRaceCompleted.AddDynamic(this, &AWRStoryManager::HandleRaceCompleted);

	TrackManager->LoadTrack(TrackType);

	// Optionally reposition player at Track's StartingPositions[0]
	// (Assuming AWRTrackManager exposes a method to get start position, pseudo-code follows)
	/*
	FVector StartPosition;
	if (TrackManager->GetStartingPosition(0, StartPosition))
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (PlayerPawn)
		{
			PlayerPawn->SetActorLocation(StartPosition);
		}
	}
	*/
	UE_LOG(LogWastelandRacers, Log, TEXT("Traveling to race: %d"), (int32)TrackType);
}

void AWRStoryManager::ReturnToWorld()
{
	bInFreeRoam = true;
	LoadWorld(CurrentWorld);
}

void AWRStoryManager::InitializeStoryData()
{
	SetupPandoraWorld();
	SetupOpportunityWorld();
	SetupEridiumWorld();
	SetupWildlifeWorld();
	SetupHyperionWorld();
	SetupCupSeries();
}

void AWRStoryManager::SetupPandoraWorld()
{
	FStoryWorldData PandoraWorld;
	PandoraWorld.WorldType = EStoryWorld::Pandora;
	PandoraWorld.WorldName = TEXT("Pandora Wastelands");
	PandoraWorld.WorldDescription = TEXT("The harsh desert planet where your racing career begins. Explore bandit camps and scrapyards.");
	PandoraWorld.AvailableTracks = {ETrackType::PandoraDesert};
	PandoraWorld.RequiredCup = ECupSeries::PandoraCup;
	PandoraWorld.bIsUnlocked = true; // Starting world
	PandoraWorld.PlayerSpawnLocation = FVector(0, 0, 100);
	PandoraWorld.RacePortalLocations = {
		FVector(1000, 500, 100),
		FVector(-800, 800, 100),
		FVector(500, -1200, 100)
	};
	StoryWorlds.Add(EStoryWorld::Pandora, PandoraWorld);
}

void AWRStoryManager::SetupOpportunityWorld()
{
	FStoryWorldData OpportunityWorld;
	OpportunityWorld.WorldType = EStoryWorld::Opportunity;
	OpportunityWorld.WorldName = TEXT("Opportunity City");
	OpportunityWorld.WorldDescription = TEXT("The corporate wasteland of Hyperion's failed city. Navigate through urban decay and construction zones.");
	OpportunityWorld.AvailableTracks = {ETrackType::OpportunityRuins};
	OpportunityWorld.RequiredCup = ECupSeries::OpportunityCup;
	OpportunityWorld.bIsUnlocked = false;
	OpportunityWorld.UnlockRequirement = 1;
	OpportunityWorld.UnlockDescription = TEXT("Complete Pandora Cup");
	OpportunityWorld.PlayerSpawnLocation = FVector(0, 0, 200);
	OpportunityWorld.RacePortalLocations = {
		FVector(800, 600, 250),
		FVector(-600, 400, 200),
		FVector(200, -900, 300)
	};
	StoryWorlds.Add(EStoryWorld::Opportunity, OpportunityWorld);
}

void AWRStoryManager::SetupEridiumWorld()
{
	FStoryWorldData EridiumWorld;
	EridiumWorld.WorldType = EStoryWorld::EridiumMines;
	EridiumWorld.WorldName = TEXT("Eridium Blight");
	EridiumWorld.WorldDescription = TEXT("Dark underground mines filled with glowing Eridium. Race through crystal caverns and unstable tunnels.");
	EridiumWorld.AvailableTracks = {ETrackType::EridiumMines};
	EridiumWorld.RequiredCup = ECupSeries::EridiumCup;
	EridiumWorld.bIsUnlocked = false;
	EridiumWorld.UnlockRequirement = 2;
	EridiumWorld.UnlockDescription = TEXT("Complete Opportunity Cup");
	EridiumWorld.PlayerSpawnLocation = FVector(0, 0, 50);
	EridiumWorld.RacePortalLocations = {
		FVector(600, 400, 25),
		FVector(-500, 300, 75),
		FVector(200, -600, 0)
	};
	StoryWorlds.Add(EStoryWorld::EridiumMines, EridiumWorld);
}

void AWRStoryManager::SetupWildlifeWorld()
{
	FStoryWorldData WildlifeWorld;
	WildlifeWorld.WorldType = EStoryWorld::WildlifePreserve;
	WildlifeWorld.WorldName = TEXT("Wildlife Preserve");
	WildlifeWorld.WorldDescription = TEXT("Overgrown research facilities with dangerous creatures. Navigate through lush vegetation and abandoned labs.");
	WildlifeWorld.AvailableTracks = {ETrackType::WildlifePreserve};
	WildlifeWorld.RequiredCup = ECupSeries::WildlifeCup;
	WildlifeWorld.bIsUnlocked = false;
	WildlifeWorld.UnlockRequirement = 3;
	WildlifeWorld.UnlockDescription = TEXT("Complete Eridium Cup");
	WildlifeWorld.PlayerSpawnLocation = FVector(0, 0, 120);
	WildlifeWorld.RacePortalLocations = {
		FVector(900, 700, 140),
		FVector(-650, 350, 135),
		FVector(400, -1000, 160)
	};
	StoryWorlds.Add(EStoryWorld::WildlifePreserve, WildlifeWorld);
}

void AWRStoryManager::SetupHyperionWorld()
{
	FStoryWorldData HyperionWorld;
	HyperionWorld.WorldType = EStoryWorld::HyperionMoonBase;
	HyperionWorld.WorldName = TEXT("Elpis Moon Base");
	HyperionWorld.WorldDescription = TEXT("Hyperion's lunar facility with low gravity racing. Experience the ultimate challenge in space.");
	HyperionWorld.AvailableTracks = {ETrackType::HyperionMoonBase};
	HyperionWorld.RequiredCup = ECupSeries::HyperionCup;
	HyperionWorld.bIsUnlocked = false;
	HyperionWorld.UnlockRequirement = 4;
	HyperionWorld.UnlockDescription = TEXT("Complete Wildlife Cup");
	HyperionWorld.PlayerSpawnLocation = FVector(0, 0, 300);
	HyperionWorld.RacePortalLocations = {
		FVector(1200, 900, 400),
		FVector(-900, 450, 375),
		FVector(600, -1200, 500)
	};
	StoryWorlds.Add(EStoryWorld::HyperionMoonBase, HyperionWorld);
}

void AWRStoryManager::SetupCupSeries()
{
	// Pandora Cup
	FCupSeriesData PandoraCup;
	PandoraCup.CupType = ECupSeries::PandoraCup;
	PandoraCup.CupName = TEXT("Pandora Wasteland Cup");
	PandoraCup.CupDescription = TEXT("Prove yourself in the harsh desert wastelands");
	PandoraCup.CupTracks = {ETrackType::PandoraDesert};
	PandoraCup.RequiredWins = 3;
	PandoraCup.RacePointsReward = 1000;
	PandoraCup.EridiumReward = 50;
	PandoraCup.UnlocksWorld = EStoryWorld::Opportunity;
	CupSeries.Add(ECupSeries::PandoraCup, PandoraCup);

	// Opportunity Cup
	FCupSeriesData OpportunityCup;
	OpportunityCup.CupType = ECupSeries::OpportunityCup;
	OpportunityCup.CupName = TEXT("Opportunity Corporate Cup");
	OpportunityCup.CupDescription = TEXT("Navigate the corporate wasteland");
	OpportunityCup.CupTracks = {ETrackType::OpportunityRuins};
	OpportunityCup.RequiredWins = 4;
	OpportunityCup.RacePointsReward = 1500;
	OpportunityCup.EridiumReward = 75;
	OpportunityCup.UnlocksWorld = EStoryWorld::EridiumMines;
	CupSeries.Add(ECupSeries::OpportunityCup, OpportunityCup);

	// Eridium Cup
	FCupSeriesData EridiumCup;
	EridiumCup.CupType = ECupSeries::EridiumCup;
	EridiumCup.CupName = TEXT("Eridium Crystal Cup");
	EridiumCup.CupDescription = TEXT("Master the underground crystal caverns");
	EridiumCup.CupTracks = {ETrackType::EridiumMines};
	EridiumCup.RequiredWins = 5;
	EridiumCup.RacePointsReward = 2000;
	EridiumCup.EridiumReward = 100;
	EridiumCup.UnlocksWorld = EStoryWorld::WildlifePreserve;
	CupSeries.Add(ECupSeries::EridiumCup, EridiumCup);

	// Wildlife Cup
	FCupSeriesData WildlifeCup;
	WildlifeCup.CupType = ECupSeries::WildlifeCup;
	WildlifeCup.CupName = TEXT("Wildlife Preserve Cup");
	WildlifeCup.CupDescription = TEXT("Survive the dangerous creature habitats");
	WildlifeCup.CupTracks = {ETrackType::WildlifePreserve};
	WildlifeCup.RequiredWins = 6;
	WildlifeCup.RacePointsReward = 2500;
	WildlifeCup.EridiumReward = 125;
	WildlifeCup.UnlocksWorld = EStoryWorld::HyperionMoonBase;
	CupSeries.Add(ECupSeries::WildlifeCup, WildlifeCup);

	// Hyperion Cup
	FCupSeriesData HyperionCup;
	HyperionCup.CupType = ECupSeries::HyperionCup;
	HyperionCup.CupName = TEXT("Hyperion Championship Cup");
	HyperionCup.CupDescription = TEXT("The ultimate low-gravity racing challenge");
	HyperionCup.CupTracks = {ETrackType::HyperionMoonBase};
	HyperionCup.RequiredWins = 8;
	HyperionCup.RacePointsReward = 5000;
	HyperionCup.EridiumReward = 250;
	HyperionCup.UnlocksWorld = EStoryWorld::Pandora; // Unlocks championship mode
	CupSeries.Add(ECupSeries::HyperionCup, HyperionCup);
}

// --- Story SaveGame Persistence and Race Flow ---

void AWRStoryManager::LoadProgress()
{
	const FString Slot = TEXT("StoryProgress");
	const int32 Idx = 0;
	if (UGameplayStatics::DoesSaveGameExist(Slot, Idx))
	{
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(Slot, Idx);
		UWRStorySaveGame* SG = Cast<UWRStorySaveGame>(LoadedGame);
		if (SG)
		{
			for (auto& Elem : SG->UnlockedWorlds)
			{
				if (FStoryWorldData* WorldData = StoryWorlds.Find(Elem.Key))
				{
					WorldData->bIsUnlocked = Elem.Value;
				}
			}
			for (auto& Elem : SG->CompletedCups)
			{
				if (FCupSeriesData* CupData = CupSeries.Find(Elem.Key))
				{
					CupData->bIsCompleted = Elem.Value;
				}
			}
			UE_LOG(LogWastelandRacers, Log, TEXT("Loaded story progress from slot."));
		}
		else
		{
			UE_LOG(LogWastelandRacers, Warning, TEXT("Failed to cast loaded save game to UWRStorySaveGame."));
		}
	}
	else
	{
		UE_LOG(LogWastelandRacers, Log, TEXT("No existing save game found for story progress."));
	}
}

void AWRStoryManager::SaveProgress() const
{
	const FString Slot = TEXT("StoryProgress");
	const int32 Idx = 0;
	UWRStorySaveGame* SG = Cast<UWRStorySaveGame>(UGameplayStatics::CreateSaveGameObject(UWRStorySaveGame::StaticClass()));
	if (!SG)
	{
		UE_LOG(LogWastelandRacers, Error, TEXT("SaveProgress: Could not create save game object!"));
		return;
	}

	for (const auto& Elem : StoryWorlds)
	{
		SG->UnlockedWorlds.Add(Elem.Key, Elem.Value.bIsUnlocked);
	}
	for (const auto& Elem : CupSeries)
	{
		SG->CompletedCups.Add(Elem.Key, Elem.Value.bIsCompleted);
	}

	if (UGameplayStatics::SaveGameToSlot(SG, Slot, Idx))
	{
		UE_LOG(LogWastelandRacers, Log, TEXT("Saved story progress to slot."));
	}
	else
	{
		UE_LOG(LogWastelandRacers, Error, TEXT("Failed to save story progress to slot!"));
	}
}

void AWRStoryManager::HandleRaceCompleted(ETrackType Track, bool bPlayerWon)
{
	if (!bPlayerWon)
	{
		UE_LOG(LogWastelandRacers, Log, TEXT("Player lost the race on track %d."), (int32)Track);
		ReturnToWorld();
		return;
	}

	// Find which cup the track belongs to
	for (auto& CupPair : CupSeries)
	{
		if (CupPair.Value.CupTracks.Contains(Track))
		{
			CompleteCup(CupPair.Key);
			break;
		}
	}

	UE_LOG(LogWastelandRacers, Log, TEXT("Player won the race on track %d. Returning to world."), (int32)Track);
	ReturnToWorld();
}

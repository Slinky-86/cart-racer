#include "WRProShop.h"
#include "Kismet/GameplayStatics.h"
#include "WRPlayerSaveGame.h"
#include "SaveGameSystem.h" // Only needed if your project uses custom save game systems
#include "WastelandRacers/Vehicles/WRKart.h"
#include "Engine/SkeletalMesh.h"

DEFINE_LOG_CATEGORY(LogWastelandRacers);

UWRProShop::UWRProShop()
{
	UnlockedEngineClasses.Add(EEngineClass::EC_100cc);
	RacePoints = 0;

	// Default ShowroomLevel path
	ShowroomLevel = TSoftObjectPtr<UWorld>(FSoftObjectPath(TEXT("/Game/CarDealership/Maps/Showroom.Showroom")));
}

void UWRProShop::AddRacePoints(int32 Points)
{
	RacePoints += Points;
	UE_LOG(LogWastelandRacers, Log, TEXT("Added %d RacePoints. New total: %d"), Points, RacePoints);
	SavePlayerProgress();
}

bool UWRProShop::PurchaseEngineClass(EEngineClass ClassToBuy, int32 Cost)
{
	if (UnlockedEngineClasses.Contains(ClassToBuy))
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Attempted to purchase already unlocked engine class."));
		return false;
	}
	if (RacePoints < Cost)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Not enough RacePoints to purchase engine class."));
		return false;
	}
	RacePoints -= Cost;
	UnlockedEngineClasses.Add(ClassToBuy);
	UE_LOG(LogWastelandRacers, Log, TEXT("Purchased engine class. Remaining RacePoints: %d"), RacePoints);
	SavePlayerProgress();
	return true;
}

bool UWRProShop::IsEngineClassUnlocked(EEngineClass Class) const
{
	return UnlockedEngineClasses.Contains(Class);
}

bool UWRProShop::PurchaseUpgrade(FName UpgradeId, int32 Cost)
{
	if (OwnedUpgradeIds.Contains(UpgradeId))
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Attempted to purchase already owned upgrade %s."), *UpgradeId.ToString());
		return false;
	}
	if (RacePoints < Cost)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Not enough RacePoints to purchase upgrade %s."), *UpgradeId.ToString());
		return false;
	}
	RacePoints -= Cost;
	OwnedUpgradeIds.Add(UpgradeId);
	UE_LOG(LogWastelandRacers, Log, TEXT("Purchased upgrade %s. Remaining RacePoints: %d"), *UpgradeId.ToString(), RacePoints);
	SavePlayerProgress();
	return true;
}

bool UWRProShop::IsUpgradeOwned(FName UpgradeId) const
{
	return OwnedUpgradeIds.Contains(UpgradeId);
}

void UWRProShop::ApplyPaintUpgrade(AWRKart* Kart, UMaterialInterface* PaintMat)
{
	if (!Kart || !PaintMat)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("ApplyPaintUpgrade failed: Invalid kart or material."));
		return;
	}
	USkeletalMeshComponent* Mesh = Kart->GetMesh();
	if (Mesh)
	{
		Mesh->SetMaterial(0, PaintMat);
		UE_LOG(LogWastelandRacers, Log, TEXT("Applied paint upgrade to kart %s."), *Kart->GetName());
	}
	else
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("ApplyPaintUpgrade failed: No mesh found on kart %s."), *Kart->GetName());
	}
}

bool UWRProShop::EnterDealership(const UObject* WorldContext)
{
	if (!ShowroomLevel.IsNull())
	{
		FName LevelName = FName(*ShowroomLevel.GetAssetName());
		UE_LOG(LogWastelandRacers, Log, TEXT("Loading dealership level %s"), *LevelName.ToString());
		return UGameplayStatics::OpenLevel(WorldContext, LevelName);
	}
	return false;
}

// ---------------- Persistence ----------------

void UWRProShop::LoadPlayerProgress()
{
	UWRPlayerSaveGame* Loaded = Cast<UWRPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerProgress"), 0));
	if (Loaded)
	{
		RacePoints = Loaded->RacePoints;
		UnlockedEngineClasses = Loaded->UnlockedEngineClasses;
		OwnedUpgradeIds = Loaded->OwnedUpgradeIds;
		UE_LOG(LogWastelandRacers, Log, TEXT("Loaded player progress: RacePoints=%d, EngineClasses=%d, Upgrades=%d"),
			RacePoints, UnlockedEngineClasses.Num(), OwnedUpgradeIds.Num());
	}
	else
	{
		// First-time/new player: start with defaults
		RacePoints = 0;
		UnlockedEngineClasses.Empty();
		UnlockedEngineClasses.Add(EEngineClass::EC_100cc);
		OwnedUpgradeIds.Empty();
		UE_LOG(LogWastelandRacers, Log, TEXT("No save found, initialized new player progress."));
		SavePlayerProgress();
	}
}

void UWRProShop::SavePlayerProgress() const
{
	UWRPlayerSaveGame* SaveGameInstance = Cast<UWRPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UWRPlayerSaveGame::StaticClass()));
	if (SaveGameInstance)
	{
		SaveGameInstance->RacePoints = RacePoints;
		SaveGameInstance->UnlockedEngineClasses = UnlockedEngineClasses;
		SaveGameInstance->OwnedUpgradeIds = OwnedUpgradeIds;
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerProgress"), 0))
		{
			UE_LOG(LogWastelandRacers, Log, TEXT("Player progress saved successfully."));
		}
		else
		{
			UE_LOG(LogWastelandRacers, Error, TEXT("Failed to save player progress."));
		}
	}
	else
	{
		UE_LOG(LogWastelandRacers, Error, TEXT("Could not create save game object for saving player progress."));
	}
}

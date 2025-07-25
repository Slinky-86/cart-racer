#include "WRGameMode.h"
#include "WastelandRacers/Player/WRPlayerController.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Gameplay/WRRaceManager.h"
#include "WastelandRacers/Story/WRStoryManager.h"
#include "WastelandRacers/Story/WRFreeRoamController.h"
#include "Engine/World.h"
#include "TimerManager.h"

AWRGameMode::AWRGameMode()
{
	DefaultPawnClass = AWRKart::StaticClass();
	
	// Set controller class based on mode
	if (bIsFreeRoam)
	{
		PlayerControllerClass = AWRFreeRoamController::StaticClass();
	}
	else
	{
		PlayerControllerClass = AWRPlayerController::StaticClass();
	}

	bUseSeamlessTravel = true;
}

void AWRGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Spawn Race Manager
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	RaceManager = GetWorld()->SpawnActor<AWRRaceManager>(AWRRaceManager::StaticClass(), SpawnParams);

	if (RaceManager)
	{
		RaceManager->Initialize(TotalLaps, MaxPlayers);
	}

	// Spawn Story Manager if in story mode
	if (bIsStoryMode)
	{
		StoryManager = GetWorld()->SpawnActor<AWRStoryManager>(AWRStoryManager::StaticClass(), SpawnParams);
		if (StoryManager)
		{
			// Initialize story progression
			UE_LOG(LogTemp, Warning, TEXT("Story Manager initialized"));
		}
	}
}

void AWRGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ConnectedPlayers++;
	
	if (AWRPlayerController* WRPlayerController = Cast<AWRPlayerController>(NewPlayer))
	{
		WRPlayerController->SetupInputComponent();
	}

	// Auto-start race when enough players join (for testing)
	if (ConnectedPlayers >= 2 && !bRaceActive)
	{
		FTimerHandle CountdownTimer;
		GetWorldTimerManager().SetTimer(CountdownTimer, this, &AWRGameMode::OnCountdownFinished, RaceCountdownTime, false);
	}
}

void AWRGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	ConnectedPlayers--;
}

void AWRGameMode::StartRace()
{
	if (!bRaceActive && RaceManager)
	{
		bRaceActive = true;
		RaceManager->StartRace();
	}
}

void AWRGameMode::EndRace()
{
	if (bRaceActive && RaceManager)
	{
		bRaceActive = false;
		RaceManager->EndRace();
	}
}

void AWRGameMode::OnCountdownFinished()
{
	StartRace();
}

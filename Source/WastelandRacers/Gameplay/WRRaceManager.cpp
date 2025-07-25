#include "WRRaceManager.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Core/WRGameInstance.h"
#include "WastelandRacers/Shop/WRProShop.h"
#include "Engine/Engine.h"

AWRRaceManager::AWRRaceManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWRRaceManager::BeginPlay()
{
	Super::BeginPlay();
}

void AWRRaceManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentRaceState)
	{
		case ERaceState::Countdown:
			CountdownTimer -= DeltaTime;
			if (CountdownTimer <= 0.0f)
			{
				UpdateRaceState(ERaceState::Racing);
			}
			break;

		case ERaceState::Racing:
			RaceTime += DeltaTime;
			UpdateKartPositions();
			break;
	}
}

void AWRRaceManager::Initialize(int32 InTotalLaps, int32 InMaxPlayers)
{
	TotalLaps = InTotalLaps;
	MaxPlayers = InMaxPlayers;
	UpdateRaceState(ERaceState::Waiting);
}

void AWRRaceManager::StartRace()
{
	if (CurrentRaceState == ERaceState::Waiting)
	{
		CountdownTimer = CountdownTime;
		UpdateRaceState(ERaceState::Countdown);
		UE_LOG(LogTemp, Warning, TEXT("Race countdown started"));
	}
}

void AWRRaceManager::EndRace()
{
	if (CurrentRaceState == ERaceState::Racing)
	{
		UpdateRaceState(ERaceState::Finished);
		UE_LOG(LogTemp, Warning, TEXT("Race finished"));
	}
}

void AWRRaceManager::RegisterKart(AWRKart* Kart)
{
	if (Kart && !RegisteredKarts.Contains(Kart))
	{
		RegisteredKarts.Add(Kart);
		UE_LOG(LogTemp, Warning, TEXT("Registered kart: %s"), *Kart->GetName());
	}
}

void AWRRaceManager::OnKartPassedCheckpoint(AWRKart* Kart, int32 CheckpointIndex)
{
	if (!Kart || CurrentRaceState != ERaceState::Racing)
		return;

	// Update kart progress tracking
	UE_LOG(LogTemp, Warning, TEXT("Kart %s passed checkpoint %d"), *Kart->GetName(), CheckpointIndex);
}

void AWRRaceManager::OnKartCompletedLap(AWRKart* Kart)
{
	if (!Kart || CurrentRaceState != ERaceState::Racing)
		return;

	int32 CurrentLap = Kart->GetCurrentLap();
	UE_LOG(LogTemp, Warning, TEXT("Kart %s completed lap %d"), *Kart->GetName(), CurrentLap);

	// Check if kart finished the race
	if (CurrentLap >= TotalLaps)
	{
		FRaceResult Result;
		Result.Kart = Kart;
		Result.Position = ++FinishedKarts;
		Result.FinishTime = RaceTime;
		Result.LapsCompleted = CurrentLap;

		RaceResults.Add(Result);
		OnKartFinished.Broadcast(Result);

		UE_LOG(LogTemp, Warning, TEXT("Kart %s finished in position %d"), *Kart->GetName(), Result.Position);

		// End race if all karts finished or first place is determined
		if (FinishedKarts >= RegisteredKarts.Num() || FinishedKarts == 1)
		{
			// Give some time for others to finish
			FTimerHandle EndRaceTimer;
			GetWorldTimerManager().SetTimer(EndRaceTimer, this, &AWRRaceManager::EndRace, 10.0f, false);
			
			// Award race points and handle story progression
			OnRaceCompleted();
		}
	}
}

void AWRRaceManager::OnRaceCompleted()
{
	// Award race points to all participants
	for (const FRaceResult& Result : RaceResults)
	{
		if (Result.Kart)
		{
			AwardRacePoints(Result.Kart, Result.Position);
		}
	}
	
	// Update story progression if in story mode
	if (UWRGameInstance* GameInstance = Cast<UWRGameInstance>(GetGameInstance()))
	{
		if (GameInstance->GetCurrentGameMode() == EGameMode::Race)
		{
			// Check if player won and update cup progression
			for (const FRaceResult& Result : RaceResults)
			{
				if (Result.Position == 1) // Winner
				{
					// Update story progression
					UE_LOG(LogTemp, Warning, TEXT("Player won race - updating story progression"));
					break;
				}
			}
		}
	}
}

void AWRRaceManager::AwardRacePoints(AWRKart* Kart, int32 Position)
{
	if (!Kart)
		return;

	if (Position < 1 || Position > 5)
	{
		UE_LOG(LogWastelandRacers, Log, TEXT("Position %d does not receive points."), Position);
		return;
	}

	int32 PointsAwarded = PositionRewards.IsValidIndex(Position - 1) ? PositionRewards[Position - 1] : 0;

	if (PointsAwarded > 0)
	{
		// Award points to player's pro shop account
		if (UWRGameInstance* GameInstance = Cast<UWRGameInstance>(GetGameInstance()))
		{
			if (UWRProShop* ProShop = GameInstance->ProShopData)
			{
				ProShop->AddRacePoints(PointsAwarded);
				UE_LOG(LogWastelandRacers, Log, TEXT("Awarded %d race points for position %d"), PointsAwarded, Position);
			}
		}
	}
	else
	{
		UE_LOG(LogWastelandRacers, Log, TEXT("No points awarded for position %d."), Position);
	}
}

int32 AWRRaceManager::GetKartPosition(AWRKart* Kart) const
{
	if (!Kart)
		return -1;

	return Kart->GetPosition();
}

void AWRRaceManager::UpdateRaceState(ERaceState NewState)
{
	if (CurrentRaceState != NewState)
	{
		CurrentRaceState = NewState;
		OnRaceStateChanged.Broadcast(NewState);
	}
}

void AWRRaceManager::UpdateKartPositions()
{
	// Sort karts by race progress
	TArray<TPair<AWRKart*, float>> KartProgress;

	for (AWRKart* Kart : RegisteredKarts)
	{
		if (Kart)
		{
			float Progress;
			CalculateKartProgress(Kart, Progress);
			KartProgress.Add(TPair<AWRKart*, float>(Kart, Progress));
		}
	}

	// Sort by progress (highest first)
	KartProgress.Sort([](const TPair<AWRKart*, float>& A, const TPair<AWRKart*, float>& B) {
		return A.Value > B.Value;
	});

	// Update positions
	for (int32 i = 0; i < KartProgress.Num(); i++)
	{
		if (KartProgress[i].Key)
		{
			// Set position on kart (this would need to be implemented in AWRKart)
			// KartProgress[i].Key->SetRacePosition(i + 1);
		}
	}
}

void AWRRaceManager::CalculateKartProgress(AWRKart* Kart, float& Progress) const
{
	if (!Kart)
	{
		Progress = 0.0f;
		return;
	}

	// Calculate progress based on laps completed and current position
	// This is a simplified calculation - in a real implementation,
	// you'd use checkpoint system and track distance
	float LapProgress = (float)Kart->GetCurrentLap() / (float)TotalLaps;
	Progress = LapProgress * 100.0f;
}

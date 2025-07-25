#include "WRTrackCheckpoint.h"
#include "WastelandRacers.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Gameplay/WRRaceManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "EngineUtils.h"
#include "EngineUtils.h"

AWRTrackCheckpoint::AWRTrackCheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create trigger box
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 500.0f, 200.0f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	RootComponent = TriggerBox;

	// Create checkpoint mesh
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(RootComponent);
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create checkpoint effect
	CheckpointEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CheckpointEffect"));
	CheckpointEffect->SetupAttachment(RootComponent);

	// Bind overlap event
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AWRTrackCheckpoint::OnTriggerBeginOverlap);
}

void AWRTrackCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	// Setup visual appearance based on checkpoint type
	if (bIsFinishLine)
	{
		// Finish line specific setup - checkered pattern, different effects
		if (CheckpointEffect)
		{
			// Set finish line particle effect
		}
	}
	else
	{
		// Regular checkpoint setup
		if (CheckpointEffect)
		{
			// Set regular checkpoint particle effect
		}
	}
}

void AWRTrackCheckpoint::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWRKart* Kart = Cast<AWRKart>(OtherActor);
	if (!Kart)
		return;

	// Check if this kart has already passed this checkpoint this lap
	if (KartsPassedThisLap.Contains(Kart))
		return;

	// Add kart to passed list
	KartsPassedThisLap.Add(Kart);

	// Play checkpoint effects
	PlayCheckpointEffects(Kart);

	// Broadcast checkpoint passed event
	OnCheckpointPassed.Broadcast(Kart, CheckpointIndex);

	// Notify race manager
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AWRRaceManager> ActorItr(World); ActorItr; ++ActorItr)
		{
			AWRRaceManager* RaceManager = *ActorItr;
			if (RaceManager)
			{
				RaceManager->OnKartPassedCheckpoint(Kart, CheckpointIndex);
				
				// If this is the finish line, check for lap completion
				if (bIsFinishLine)
				{
					RaceManager->OnKartCompletedLap(Kart);
					// Clear the passed karts for next lap
					KartsPassedThisLap.Remove(Kart);
				}
				break;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Kart %s passed checkpoint %d"), *Kart->GetName(), CheckpointIndex);
}

void AWRTrackCheckpoint::PlayCheckpointEffects(AWRKart* Kart)
{
	// Play visual effect
	if (PassEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), PassEffect, GetActorLocation(), GetActorRotation());
	}

	// Play sound effect
	USoundBase* SoundToPlay = bIsFinishLine ? FinishLineSound : PassSound;
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, GetActorLocation());
	}

	// Additional effects for finish line
	if (bIsFinishLine)
	{
		// Play special finish line effects
		UE_LOG(LogTemp, Warning, TEXT("Kart %s crossed finish line!"), *Kart->GetName());
	}
}

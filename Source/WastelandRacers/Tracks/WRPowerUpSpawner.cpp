#include "WRPowerUpSpawner.h"
#include "WastelandRacers.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Gameplay/WRPowerUpComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

AWRPowerUpSpawner::AWRPowerUpSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create collection trigger
	CollectionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionTrigger"));
	CollectionTrigger->SetSphereRadius(100.0f);
	CollectionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollectionTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	CollectionTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollectionTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	RootComponent = CollectionTrigger;

	// Create power-up mesh
	PowerUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerUpMesh"));
	PowerUpMesh->SetupAttachment(RootComponent);
	PowerUpMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create power-up effect
	PowerUpEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PowerUpEffect"));
	PowerUpEffect->SetupAttachment(RootComponent);

	// Create power-up audio
	PowerUpAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("PowerUpAudio"));
	PowerUpAudio->SetupAttachment(RootComponent);
	PowerUpAudio->bAutoActivate = false;

	// Bind overlap event
	CollectionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWRPowerUpSpawner::OnTriggerBeginOverlap);

	// Initialize available power-ups
	FPowerUpData SpeedBoost;
	SpeedBoost.PowerUpType = EPowerUpType::SpeedBoost;
	SpeedBoost.Duration = 3.0f;
	SpeedBoost.Magnitude = 1.5f;
	AvailablePowerUps.Add(SpeedBoost);

	FPowerUpData Shield;
	Shield.PowerUpType = EPowerUpType::Shield;
	Shield.Duration = 5.0f;
	AvailablePowerUps.Add(Shield);

	FPowerUpData HomingRocket;
	HomingRocket.PowerUpType = EPowerUpType::Weapon;
	HomingRocket.WeaponType = EWeaponType::HomingRocket;
	AvailablePowerUps.Add(HomingRocket);

	FPowerUpData Shotgun;
	Shotgun.PowerUpType = EPowerUpType::Weapon;
	Shotgun.WeaponType = EWeaponType::ShotgunBlast;
	AvailablePowerUps.Add(Shotgun);

	FPowerUpData Grenade;
	Grenade.PowerUpType = EPowerUpType::Weapon;
	Grenade.WeaponType = EWeaponType::Grenade;
	AvailablePowerUps.Add(Grenade);

	FPowerUpData OilSlick;
	OilSlick.PowerUpType = EPowerUpType::Weapon;
	OilSlick.WeaponType = EWeaponType::OilSlick;
	AvailablePowerUps.Add(OilSlick);
}

void AWRPowerUpSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (bHasPowerUp)
	{
		SpawnPowerUp();
	}
}

void AWRPowerUpSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate power-up for visual appeal
	if (bHasPowerUp && PowerUpMesh)
	{
		FRotator CurrentRotation = PowerUpMesh->GetRelativeRotation();
		CurrentRotation.Yaw += 90.0f * DeltaTime;
		PowerUpMesh->SetRelativeRotation(CurrentRotation);
	}
}

void AWRPowerUpSpawner::SpawnPowerUp()
{
	if (bHasPowerUp)
		return;

	bHasPowerUp = true;

	// Select power-up
	if (bRandomizePowerUp)
	{
		CurrentPowerUp = SelectRandomPowerUp();
	}
	else
	{
		CurrentPowerUp = FixedPowerUp;
	}

	// Setup appearance
	SetupPowerUpAppearance();

	// Show visual elements
	if (PowerUpMesh)
	{
		PowerUpMesh->SetVisibility(true);
	}

	if (PowerUpEffect)
	{
		PowerUpEffect->Activate();
	}

	UE_LOG(LogTemp, Warning, TEXT("Power-up spawned: %d"), (int32)CurrentPowerUp.PowerUpType);
}

void AWRPowerUpSpawner::CollectPowerUp(AWRKart* Kart)
{
	if (!bHasPowerUp || !Kart)
		return;

	// Per-lap pickup restriction: each kart can only collect this spawner once per lap.
	int32 KartLap = Kart->GetCurrentLap();
	int32* LastLap = LastLapCollected.Find(Kart);
	if (LastLap && *LastLap == KartLap)
	{
		// Already collected this spawner this lap – do nothing.
		UE_LOG(LogWastelandRacers, Log, TEXT("%s already collected this spawner on lap %d"), *Kart->GetName(), KartLap);
		return;
	}
	// Else proceed to give power-up and record lap.
	LastLapCollected.Add(Kart, KartLap);

	// Give power-up to kart
	UWRPowerUpComponent* PowerUpComponent = Kart->FindComponentByClass<UWRPowerUpComponent>();
	if (PowerUpComponent)
	{
		PowerUpComponent->CollectPowerUp(CurrentPowerUp);
	}

	// Hide power-up
	bHasPowerUp = false;

	if (PowerUpMesh)
	{
		PowerUpMesh->SetVisibility(false);
	}

	if (PowerUpEffect)
	{
		PowerUpEffect->Deactivate();
	}

	// Play collection effects
	PlayCollectionEffects();

	// Start respawn timer
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &AWRPowerUpSpawner::SpawnPowerUp, RespawnTime, false);

	UE_LOG(LogTemp, Warning, TEXT("Power-up collected by %s"), *Kart->GetName());
}

void AWRPowerUpSpawner::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bHasPowerUp)
		return;

	AWRKart* Kart = Cast<AWRKart>(OtherActor);
	if (Kart)
	{
		CollectPowerUp(Kart);
	}
}

void AWRPowerUpSpawner::SetupPowerUpAppearance()
{
	// Set mesh and effects based on power-up type
	switch (CurrentPowerUp.PowerUpType)
	{
		case EPowerUpType::SpeedBoost:
			// Set speed boost mesh and blue effects
			break;

		case EPowerUpType::Shield:
			// Set shield mesh and protective effects
			break;

		case EPowerUpType::Weapon:
			// Set weapon-specific mesh based on weapon type
			switch (CurrentPowerUp.WeaponType)
			{
				case EWeaponType::HomingRocket:
					// Rocket mesh and red effects
					break;
				case EWeaponType::ShotgunBlast:
					// Shotgun mesh and orange effects
					break;
				case EWeaponType::Grenade:
					// Grenade mesh and explosive effects
					break;
				case EWeaponType::OilSlick:
					// Oil barrel mesh and black effects
					break;
				default:
					break;
			}
			break;

		case EPowerUpType::Repair:
			// Set repair mesh and green effects
			break;

		default:
			break;
	}
}

FPowerUpData AWRPowerUpSpawner::SelectRandomPowerUp() const
{
	if (AvailablePowerUps.Num() == 0)
	{
		return FPowerUpData();
	}

	int32 RandomIndex = FMath::RandRange(0, AvailablePowerUps.Num() - 1);
	return AvailablePowerUps[RandomIndex];
}

void AWRPowerUpSpawner::PlayCollectionEffects()
{
	// Play collection effect
	if (CollectionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), CollectionEffect, GetActorLocation(), GetActorRotation());
	}

	// Play collection sound
	if (CollectionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CollectionSound, GetActorLocation());
	}
}

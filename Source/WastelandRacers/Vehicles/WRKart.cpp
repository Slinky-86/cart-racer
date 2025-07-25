#include "WRKart.h"
#include "WastelandRacers/Weapons/WRWeaponComponent.h"
#include "WastelandRacers/Gameplay/WRPowerUpComponent.h"
#include "WastelandRacers/Gameplay/WRGameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ChaosVehicleMovementComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

AWRKart::AWRKart()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create weapon component
	WeaponComponent = CreateDefaultSubobject<UWRWeaponComponent>(TEXT("WeaponComponent"));

	// Create power-up component
	PowerUpComponent = CreateDefaultSubobject<UWRPowerUpComponent>(TEXT("PowerUpComponent"));

	// Create Niagara effects
	BoostEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoostEffect"));
	BoostEffect->SetupAttachment(RootComponent);
	BoostEffect->SetAutoActivate(false);

	DriftEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DriftEffect"));
	DriftEffect->SetupAttachment(RootComponent);
	DriftEffect->SetAutoActivate(false);

	// Set collision
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// Bind hit event
	GetMesh()->OnComponentHit.AddDynamic(this, &AWRKart::OnHit);
}

void AWRKart::BeginPlay()
{
	Super::BeginPlay();

	// Sync EngineClass from GameInstance if not set
	if (EngineClass == EEngineClass::EC_100cc)
	{
		if (const UWorld* World = GetWorld())
		{
			if (const UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (const UWRGameInstance* WRGI = Cast<UWRGameInstance>(GameInstance))
				{
					EngineClass = WRGI->CurrentEngineClass;
				}
			}
		}
	}
	ApplyKartStats();
}

void AWRKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDrift(DeltaTime);
	UpdateBoost(DeltaTime);
}

void AWRKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AWRKart::SetThrottleInput(float Value)
{
	CurrentThrottle = Value;
	
	if (UChaosVehicleMovementComponent* VehicleMovement = Cast<UChaosVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		VehicleMovement->SetThrottleInput(Value);
	}
}

void AWRKart::SetSteeringInput(float Value)
{
	CurrentSteering = Value;
	
	if (UChaosVehicleMovementComponent* VehicleMovement = Cast<UChaosVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		VehicleMovement->SetSteeringInput(Value);
	}
}

void AWRKart::SetBrakeInput(float Value)
{
	CurrentBrake = Value;
	
	if (UChaosVehicleMovementComponent* VehicleMovement = Cast<UChaosVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		VehicleMovement->SetBrakeInput(Value);
	}
}

void AWRKart::UseWeapon()
{
	if (WeaponComponent)
	{
		WeaponComponent->FireWeapon();
	}
}

void AWRKart::StartDrift()
{
	if (!bIsDrifting && FMath::Abs(CurrentSteering) > DriftThreshold)
	{
		bIsDrifting = true;
		DriftTime = 0.0f;
		
		if (DriftEffect)
		{
			DriftEffect->Activate();
		}
	}
}

void AWRKart::StopDrift()
{
	if (bIsDrifting)
	{
		bIsDrifting = false;
		ApplyDriftBoost();
		
		if (DriftEffect)
		{
			DriftEffect->Deactivate();
		}
	}
}

void AWRKart::ActivateBoost(float BoostAmount)
{
	bIsBoosting = true;
	BoostTimeRemaining = BoostAmount;
	
	if (BoostEffect)
	{
		BoostEffect->Activate();
	}
}

float AWRKart::GetCurrentSpeed() const
{
	if (UChaosVehicleMovementComponent* VehicleMovement = Cast<UChaosVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		return VehicleMovement->GetForwardSpeed();
	}
	return 0.0f;
}

void AWRKart::UpdateDrift(float DeltaTime)
{
	if (bIsDrifting)
	{
		DriftTime += DeltaTime;
		
		// Visual feedback for drift levels
		if (DriftTime >= UltraMiniTurboTime)
		{
			// Ultra boost ready - purple sparks
		}
		else if (DriftTime >= SuperMiniTurboTime)
		{
			// Super boost ready - orange sparks
		}
		else if (DriftTime >= MiniTurboTime)
		{
			// Mini boost ready - blue sparks
		}
	}
}

void AWRKart::ApplyDriftBoost()
{
	float BoostPower = 0.0f;
	
	if (DriftTime >= UltraMiniTurboTime)
	{
		BoostPower = 2.0f; // Ultra boost
	}
	else if (DriftTime >= SuperMiniTurboTime)
	{
		BoostPower = 1.5f; // Super boost
	}
	else if (DriftTime >= MiniTurboTime)
	{
		BoostPower = 1.0f; // Mini boost
	}
	
	if (BoostPower > 0.0f)
	{
		ActivateBoost(BoostPower);
	}
	
	DriftTime = 0.0f;
}

void AWRKart::UpdateBoost(float DeltaTime)
{
	if (bIsBoosting)
	{
		BoostTimeRemaining -= DeltaTime;
		// (Removed AddForce; boost logic can be implemented via increasing throttle or modifying acceleration.)
		if (BoostTimeRemaining <= 0.0f)
		{
			bIsBoosting = false;
			if (BoostEffect)
			{
				BoostEffect->Deactivate();
			}
		}
	}
}

void AWRKart::ApplyKartStats()
{
	if (UChaosVehicleMovementComponent* VehicleMovement = Cast<UChaosVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		const float Mult = GetEngineClassMultiplier(EngineClass);

		// ChaosVehicleMovementComponent does not expose EngineSetup in UE 5.6
		// Tuning for engine class multiplier goes here if API is exposed in future

		// Adjust mass
		GetMesh()->SetMassOverrideInKg(NAME_None, KartStats.Weight, true);

		// Optionally scale acceleration and boost power
		// (This scales only boost power, as requested)
		KartStats.BoostPower = KartStats.BoostPower * Mult;
	}
}

void AWRKart::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Handle collisions with other karts, walls, etc.
	if (AWRKart* OtherKart = Cast<AWRKart>(OtherActor))
	{
		// Kart-to-kart collision logic
		float ImpactForce = NormalImpulse.Size();
		if (ImpactForce > 1000.0f)
		{
			// Apply spin-out or slowdown effect
		}
	}
}

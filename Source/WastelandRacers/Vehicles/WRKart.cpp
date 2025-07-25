#include "WRKart.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "WastelandRacers/Weapons/WRWeaponComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"

AWRKart::AWRKart()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create kart mesh component
	KartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KartMesh"));
	RootComponent = KartMesh;

	// Create engine audio component
	EngineAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
	EngineAudioComponent->SetupAttachment(RootComponent);

	// Create weapon component
	WeaponComponent = CreateDefaultSubobject<UWRWeaponComponent>(TEXT("WeaponComponent"));

	// Initialize values
	CurrentBoostEnergy = MaxBoostEnergy;
	CurrentHealth = MaxHealth;
}

void AWRKart::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogWastelandRacers, Log, TEXT("WRKart BeginPlay - Health: %.1f, Boost: %.1f"), CurrentHealth, CurrentBoostEnergy);
}

void AWRKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle boost
	if (bIsBoosting && CurrentBoostEnergy > 0.0f)
	{
		UseBoost(DeltaTime);
	}
	else
	{
		RechargeBoost(DeltaTime);
	}

	// Update engine audio based on throttle
	if (EngineAudioComponent && EngineAudioComponent->GetSound())
	{
		float PitchMultiplier = 1.0f + (FMath::Abs(ThrottleInput) * 0.5f);
		EngineAudioComponent->SetPitchMultiplier(PitchMultiplier);
	}
}

void AWRKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement
	PlayerInputComponent->BindAxis("MoveForward", this, &AWRKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWRKart::MoveRight);

	// Bind actions
	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AWRKart::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AWRKart::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &AWRKart::OnBoostPressed);
	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AWRKart::OnFireWeapon);
}

void AWRKart::MoveForward(float Value)
{
	ThrottleInput = Value;
	
	if (IsDestroyed())
	{
		return;
	}

	// Apply boost multiplier if boosting
	float FinalValue = Value;
	if (bIsBoosting && CurrentBoostEnergy > 0.0f)
	{
		FinalValue *= BoostMultiplier;
	}

	// Apply throttle to vehicle
	GetVehicleMovementComponent()->SetThrottleInput(FinalValue);
}

void AWRKart::MoveRight(float Value)
{
	SteeringInput = Value;
	
	if (IsDestroyed())
	{
		return;
	}

	// Apply steering to vehicle
	GetVehicleMovementComponent()->SetSteeringInput(Value);
}

void AWRKart::OnHandbrakePressed()
{
	bIsHandbrakePressed = true;
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AWRKart::OnHandbrakeReleased()
{
	bIsHandbrakePressed = false;
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AWRKart::OnBoostPressed()
{
	if (CurrentBoostEnergy > 0.0f && !IsDestroyed())
	{
		bIsBoosting = !bIsBoosting;
		UE_LOG(LogWastelandRacers, Log, TEXT("Boost toggled: %s"), bIsBoosting ? TEXT("ON") : TEXT("OFF"));
	}
}

void AWRKart::OnFireWeapon()
{
	if (WeaponComponent && !IsDestroyed())
	{
		WeaponComponent->FireWeapon();
	}
}

void AWRKart::UseBoost(float DeltaTime)
{
	if (CurrentBoostEnergy > 0.0f)
	{
		CurrentBoostEnergy = FMath::Max(0.0f, CurrentBoostEnergy - (BoostConsumptionRate * DeltaTime));
		
		if (CurrentBoostEnergy <= 0.0f)
		{
			bIsBoosting = false;
			UE_LOG(LogWastelandRacers, Log, TEXT("Boost depleted"));
		}
	}
}

void AWRKart::RechargeBoost(float DeltaTime)
{
	if (CurrentBoostEnergy < MaxBoostEnergy && !bIsBoosting)
	{
		CurrentBoostEnergy = FMath::Min(MaxBoostEnergy, CurrentBoostEnergy + (BoostRechargeRate * DeltaTime));
	}
}

void AWRKart::TakeDamage(float DamageAmount)
{
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
	
	UE_LOG(LogWastelandRacers, Log, TEXT("Kart took %.1f damage, health now: %.1f"), DamageAmount, CurrentHealth);
	
	if (IsDestroyed())
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Kart destroyed!"));
		// Handle kart destruction
		GetVehicleMovementComponent()->SetThrottleInput(0.0f);
		GetVehicleMovementComponent()->SetSteeringInput(0.0f);
	}
}

void AWRKart::RepairKart(float RepairAmount)
{
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + RepairAmount);
	UE_LOG(LogWastelandRacers, Log, TEXT("Kart repaired by %.1f, health now: %.1f"), RepairAmount, CurrentHealth);
}

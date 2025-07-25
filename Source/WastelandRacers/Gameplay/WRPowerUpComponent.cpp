#include "WRPowerUpComponent.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Weapons/WRWeaponComponent.h"
#include "Engine/Engine.h"

UWRPowerUpComponent::UWRPowerUpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize available power-ups
	FPowerUpData SpeedBoostPowerUp;
	SpeedBoostPowerUp.PowerUpType = EPowerUpType::SpeedBoost;
	SpeedBoostPowerUp.Duration = 3.0f;
	SpeedBoostPowerUp.Magnitude = 1.5f;
	AvailablePowerUps.Add(SpeedBoostPowerUp);

	FPowerUpData ShieldPowerUp;
	ShieldPowerUp.PowerUpType = EPowerUpType::Shield;
	ShieldPowerUp.Duration = 5.0f;
	AvailablePowerUps.Add(ShieldPowerUp);

	FPowerUpData RepairPowerUp;
	RepairPowerUp.PowerUpType = EPowerUpType::Repair;
	RepairPowerUp.Magnitude = 50.0f;
	AvailablePowerUps.Add(RepairPowerUp);

	// Weapon power-ups
	FPowerUpData RocketPowerUp;
	RocketPowerUp.PowerUpType = EPowerUpType::Weapon;
	RocketPowerUp.WeaponType = EWeaponType::HomingRocket;
	AvailablePowerUps.Add(RocketPowerUp);

	FPowerUpData ShotgunPowerUp;
	ShotgunPowerUp.PowerUpType = EPowerUpType::Weapon;
	ShotgunPowerUp.WeaponType = EWeaponType::ShotgunBlast;
	AvailablePowerUps.Add(ShotgunPowerUp);

	FPowerUpData GrenadePowerUp;
	GrenadePowerUp.PowerUpType = EPowerUpType::Weapon;
	GrenadePowerUp.WeaponType = EWeaponType::Grenade;
	AvailablePowerUps.Add(GrenadePowerUp);
}

void UWRPowerUpComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWRPowerUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWRPowerUpComponent::CollectPowerUp(const FPowerUpData& PowerUpData)
{
	if (CurrentPowerUp.PowerUpType == EPowerUpType::None)
	{
		CurrentPowerUp = PowerUpData;
		UE_LOG(LogTemp, Warning, TEXT("Collected power-up: %d"), (int32)PowerUpData.PowerUpType);
	}
}

void UWRPowerUpComponent::ActivatePowerUp()
{
	if (CurrentPowerUp.PowerUpType == EPowerUpType::None)
		return;

	switch (CurrentPowerUp.PowerUpType)
	{
		case EPowerUpType::SpeedBoost:
			ApplySpeedBoost(CurrentPowerUp.Magnitude, CurrentPowerUp.Duration);
			break;
		case EPowerUpType::Shield:
			ApplyShield(CurrentPowerUp.Duration);
			break;
		case EPowerUpType::Repair:
			ApplyRepair(CurrentPowerUp.Magnitude);
			break;
		case EPowerUpType::Weapon:
			GiveWeapon(CurrentPowerUp.WeaponType);
			break;
	}

	// Clear current power-up
	CurrentPowerUp = FPowerUpData();
}

void UWRPowerUpComponent::ApplySpeedBoost(float Magnitude, float Duration)
{
	AWRKart* OwnerKart = GetOwnerKart();
	if (OwnerKart)
	{
		OwnerKart->ActivateBoost(Duration);
		UE_LOG(LogTemp, Warning, TEXT("Applied speed boost: %f for %f seconds"), Magnitude, Duration);
	}
}

void UWRPowerUpComponent::ApplyShield(float Duration)
{
	AWRKart* OwnerKart = GetOwnerKart();
	if (OwnerKart)
	{
		// Apply shield effect (to be implemented)
		UE_LOG(LogTemp, Warning, TEXT("Applied shield for %f seconds"), Duration);
	}
}

void UWRPowerUpComponent::ApplyRepair(float Amount)
{
	AWRKart* OwnerKart = GetOwnerKart();
	if (OwnerKart)
	{
		// Apply repair (to be implemented)
		UE_LOG(LogTemp, Warning, TEXT("Applied repair: %f"), Amount);
	}
}

void UWRPowerUpComponent::GiveWeapon(EWeaponType WeaponType)
{
	AWRKart* OwnerKart = GetOwnerKart();
	if (OwnerKart)
	{
		UWRWeaponComponent* WeaponComponent = OwnerKart->FindComponentByClass<UWRWeaponComponent>();
		if (WeaponComponent)
		{
			WeaponComponent->AddWeapon(WeaponType);
			UE_LOG(LogTemp, Warning, TEXT("Gave weapon: %d"), (int32)WeaponType);
		}
	}
}

AWRKart* UWRPowerUpComponent::GetOwnerKart() const
{
	return Cast<AWRKart>(GetOwner());
}

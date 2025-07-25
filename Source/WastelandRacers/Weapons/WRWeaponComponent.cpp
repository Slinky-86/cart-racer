#include "WRWeaponComponent.h"
#include "WastelandRacers/WastelandRacers.h"
#include "WastelandRacers/Weapons/WRProjectile.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

UWRWeaponComponent::UWRWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentAmmo = MaxAmmo;
}

void UWRWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;
}

void UWRWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Handle reloading
	if (bIsReloading)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - ReloadStartTime >= ReloadTime)
		{
			CurrentAmmo = MaxAmmo;
			bIsReloading = false;
			UE_LOG(LogWastelandRacers, Log, TEXT("Weapon reloaded"));
		}
	}
}

void UWRWeaponComponent::FireWeapon()
{
	if (!CanFire())
	{
		return;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();

	switch (CurrentWeaponType)
	{
		case EWeaponType::MachineGun:
			FireMachineGun();
			break;
		case EWeaponType::RocketLauncher:
			FireRocketLauncher();
			break;
		case EWeaponType::Shotgun:
			FireShotgun();
			break;
		case EWeaponType::Flamethrower:
			FireFlamethrower();
			break;
	}

	CurrentAmmo--;
	
	if (CurrentAmmo <= 0)
	{
		ReloadWeapon();
	}
}

void UWRWeaponComponent::SetWeaponType(EWeaponType NewWeaponType)
{
	CurrentWeaponType = NewWeaponType;
	
	// Adjust weapon stats based on type
	switch (NewWeaponType)
	{
		case EWeaponType::MachineGun:
			MaxAmmo = 30;
			FireRate = 0.1f;
			WeaponDamage = 25.0f;
			ReloadTime = 2.0f;
			break;
		case EWeaponType::RocketLauncher:
			MaxAmmo = 5;
			FireRate = 1.0f;
			WeaponDamage = 100.0f;
			ReloadTime = 3.0f;
			break;
		case EWeaponType::Shotgun:
			MaxAmmo = 8;
			FireRate = 0.8f;
			WeaponDamage = 75.0f;
			ReloadTime = 2.5f;
			break;
		case EWeaponType::Flamethrower:
			MaxAmmo = 50;
			FireRate = 0.05f;
			WeaponDamage = 15.0f;
			ReloadTime = 4.0f;
			break;
	}
	
	CurrentAmmo = MaxAmmo;
	UE_LOG(LogWastelandRacers, Log, TEXT("Weapon type changed to: %d"), (int32)NewWeaponType);
}

void UWRWeaponComponent::ReloadWeapon()
{
	if (!bIsReloading && CurrentAmmo < MaxAmmo)
	{
		bIsReloading = true;
		ReloadStartTime = GetWorld()->GetTimeSeconds();
		UE_LOG(LogWastelandRacers, Log, TEXT("Reloading weapon..."));
	}
}

bool UWRWeaponComponent::CanFire() const
{
	if (bIsReloading || CurrentAmmo <= 0)
	{
		return false;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	return (CurrentTime - LastFireTime) >= FireRate;
}

void UWRWeaponComponent::FireMachineGun()
{
	FVector StartLocation = GetMuzzleLocation();
	FVector Direction = GetFireDirection();
	
	SpawnProjectile(StartLocation, Direction);
	UE_LOG(LogWastelandRacers, Log, TEXT("Fired machine gun"));
}

void UWRWeaponComponent::FireRocketLauncher()
{
	FVector StartLocation = GetMuzzleLocation();
	FVector Direction = GetFireDirection();
	
	SpawnProjectile(StartLocation, Direction);
	UE_LOG(LogWastelandRacers, Log, TEXT("Fired rocket"));
}

void UWRWeaponComponent::FireShotgun()
{
	FVector StartLocation = GetMuzzleLocation();
	FVector BaseDirection = GetFireDirection();
	
	// Fire multiple pellets for shotgun spread
	for (int32 i = 0; i < 5; i++)
	{
		FVector SpreadDirection = BaseDirection;
		SpreadDirection += FVector(
			FMath::RandRange(-0.2f, 0.2f),
			FMath::RandRange(-0.2f, 0.2f),
			FMath::RandRange(-0.1f, 0.1f)
		);
		SpreadDirection.Normalize();
		
		SpawnProjectile(StartLocation, SpreadDirection);
	}
	
	UE_LOG(LogWastelandRacers, Log, TEXT("Fired shotgun"));
}

void UWRWeaponComponent::FireFlamethrower()
{
	FVector StartLocation = GetMuzzleLocation();
	FVector Direction = GetFireDirection();
	
	// Flamethrower creates a short-range flame effect
	SpawnProjectile(StartLocation, Direction);
	UE_LOG(LogWastelandRacers, Log, TEXT("Fired flamethrower"));
}

void UWRWeaponComponent::SpawnProjectile(const FVector& StartLocation, const FVector& Direction)
{
	if (!ProjectileClass)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("No projectile class set for weapon"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	FRotator SpawnRotation = Direction.Rotation();
	
	AWRProjectile* Projectile = World->SpawnActor<AWRProjectile>(
		ProjectileClass,
		StartLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile)
	{
		Projectile->SetDamage(WeaponDamage);
		Projectile->SetWeaponType(CurrentWeaponType);
	}
}

FVector UWRWeaponComponent::GetFireDirection() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ForwardVector;
	}

	return Owner->GetActorForwardVector();
}

FVector UWRWeaponComponent::GetMuzzleLocation() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	// Get location slightly in front of the kart
	FVector ForwardVector = Owner->GetActorForwardVector();
	FVector OwnerLocation = Owner->GetActorLocation();
	
	return OwnerLocation + (ForwardVector * 100.0f) + FVector(0, 0, 50.0f);
}

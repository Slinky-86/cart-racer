#include "WRWeaponComponent.h"
#include "WastelandRacers/WastelandRacers.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Weapons/Projectiles/WRProjectile.h"
#include "WastelandRacers/Weapons/Projectiles/WRHomingRocket.h"
#include "WastelandRacers/Weapons/Projectiles/WRGrenade.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"

UWRWeaponComponent::UWRWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWRWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Create weapon mount point if not set
	if (!WeaponMountPoint)
	{
		WeaponMountPoint = NewObject<USceneComponent>(GetOwner());
		WeaponMountPoint->SetupAttachment(GetOwner()->GetRootComponent());
	}
}

void UWRWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWRWeaponComponent::FireWeapon()
{
	if (CurrentWeapon == EWeaponType::None)
		return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastFireTime < FireCooldown)
		return;

	LastFireTime = CurrentTime;

	switch (CurrentWeapon)
	{
		case EWeaponType::HomingRocket:
			FireHomingRocket();
			break;
		case EWeaponType::ShotgunBlast:
			FireShotgunBlast();
			break;
		case EWeaponType::OilSlick:
			DeployOilSlick();
			break;
		case EWeaponType::Grenade:
			ThrowGrenade();
			break;
		case EWeaponType::SlagDebuff:
			ApplySlagDebuff();
			break;
		case EWeaponType::Shield:
			ActivateShield();
			break;
		case EWeaponType::SpeedBoost:
			ActivateSpeedBoost();
			break;
		case EWeaponType::TeleportPad:
			UseTeleportPad();
			break;
		case EWeaponType::Decoy:
			DeployDecoy();
			break;
	}

	// Clear weapon after use
	CurrentWeapon = EWeaponType::None;
}

void UWRWeaponComponent::SetCurrentWeapon(EWeaponType NewWeaponType)
{
	CurrentWeapon = NewWeaponType;
}

void UWRWeaponComponent::AddWeapon(EWeaponType WeaponType)
{
	if (CurrentWeapon == EWeaponType::None)
	{
		CurrentWeapon = WeaponType;
	}
}

void UWRWeaponComponent::FireHomingRocket()
{
	if (ProjectileClasses.Num() > 0 && ProjectileClasses[0])
	{
		FVector SpawnLocation = WeaponMountPoint->GetComponentLocation();
		FRotator SpawnRotation = GetOwner()->GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());

		AWRHomingRocket* Rocket = GetWorld()->SpawnActor<AWRHomingRocket>(
			AWRHomingRocket::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

		if (Rocket)
		{
			Rocket->SetTarget(FindNearestTarget());
		}
	}
}

void UWRWeaponComponent::FireShotgunBlast()
{
	// Fire multiple projectiles in a spread pattern
	int32 PelletCount = 5;
	float SpreadAngle = 30.0f;

	for (int32 i = 0; i < PelletCount; i++)
	{
		FVector SpawnLocation = WeaponMountPoint->GetComponentLocation();
		FRotator SpawnRotation = GetOwner()->GetActorRotation();
		
		// Add spread
		float AngleOffset = (i - PelletCount / 2) * (SpreadAngle / PelletCount);
		SpawnRotation.Yaw += AngleOffset;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());

		GetWorld()->SpawnActor<AWRProjectile>(
			AWRProjectile::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
	}
}

void UWRWeaponComponent::DeployOilSlick()
{
	// Spawn oil slick behind the kart
	FVector SpawnLocation = GetOwner()->GetActorLocation() - (GetOwner()->GetActorForwardVector() * 200.0f);
	FRotator SpawnRotation = GetOwner()->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	// Spawn oil slick actor (to be implemented)
	// GetWorld()->SpawnActor<AWROilSlick>(AWROilSlick::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
}

void UWRWeaponComponent::ThrowGrenade()
{
	FVector SpawnLocation = WeaponMountPoint->GetComponentLocation();
	FRotator SpawnRotation = GetOwner()->GetActorRotation();
	SpawnRotation.Pitch += 15.0f; // Arc the grenade

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	GetWorld()->SpawnActor<AWRGrenade>(
		AWRGrenade::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
}

void UWRWeaponComponent::ApplySlagDebuff()
{
	// Find target and apply slag effect
	AWRKart* Target = FindNearestTarget();
	if (Target)
	{
		// Apply slag debuff to target (to be implemented)
	}
}

void UWRWeaponComponent::ActivateShield()
{
	AWRKart* OwnerKart = GetOwnerKart();
	if (OwnerKart)
	{
		// Activate shield on owner kart (to be implemented)
	}
}

void UWRWeaponComponent::ActivateSpeedBoost()
{
	AWRKart* OwnerKart = GetOwnerKart();
	if (OwnerKart)
	{
		OwnerKart->ActivateBoost(2.0f);
	}
}

void UWRWeaponComponent::UseTeleportPad()
{
	AWRKart* OwnerKart = GetOwnerKart();
	if (OwnerKart)
	{
		// Teleport kart forward (to be implemented)
		FVector TeleportLocation = OwnerKart->GetActorLocation() + (OwnerKart->GetActorForwardVector() * 1000.0f);
		OwnerKart->SetActorLocation(TeleportLocation);
	}
}

void UWRWeaponComponent::DeployDecoy()
{
	// Spawn decoy hologram (to be implemented)
}

AWRKart* UWRWeaponComponent::GetOwnerKart() const
{
	return Cast<AWRKart>(GetOwner());
}

AWRKart* UWRWeaponComponent::FindNearestTarget() const
{
	AWRKart* OwnerKart = Cast<AWRKart>(GetOwner());
	if (!OwnerKart)
		return nullptr;

	AWRKart* NearestTarget = nullptr;
	float NearestDistance = FLT_MAX;

	for (TActorIterator<AWRKart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AWRKart* OtherKart = *ActorItr;
		if (OtherKart != OwnerKart)
		{
			float Distance = FVector::Dist(OwnerKart->GetActorLocation(), OtherKart->GetActorLocation());
			if (Distance < NearestDistance)
			{
				NearestDistance = Distance;
				NearestTarget = OtherKart;
			}
		}
	}

	return NearestTarget;
}

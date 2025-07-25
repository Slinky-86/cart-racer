#include "WRProjectile.h"
#include "WastelandRacers/WastelandRacers.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AWRProjectile::AWRProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->SetSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComponent;

	// Create mesh component
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create trail effect component
	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);

	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 0.0f;
	ProjectileMovement->ProjectileGravityScale = 0.1f;

	// Set up collision delegate
	CollisionComponent->OnComponentHit.AddDynamic(this, &AWRProjectile::OnHit);

	// Set lifespan
	InitialLifeSpan = LifeSpan;
}

void AWRProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Adjust projectile properties based on weapon type
	switch (WeaponType)
	{
		case EWeaponType::MachineGun:
			ProjectileMovement->InitialSpeed = 2500.0f;
			ProjectileMovement->MaxSpeed = 2500.0f;
			ProjectileMovement->ProjectileGravityScale = 0.1f;
			break;
		case EWeaponType::RocketLauncher:
			ProjectileMovement->InitialSpeed = 1500.0f;
			ProjectileMovement->MaxSpeed = 1500.0f;
			ProjectileMovement->ProjectileGravityScale = 0.3f;
			break;
		case EWeaponType::Shotgun:
			ProjectileMovement->InitialSpeed = 1800.0f;
			ProjectileMovement->MaxSpeed = 1800.0f;
			ProjectileMovement->ProjectileGravityScale = 0.5f;
			break;
		case EWeaponType::Flamethrower:
			ProjectileMovement->InitialSpeed = 800.0f;
			ProjectileMovement->MaxSpeed = 800.0f;
			ProjectileMovement->ProjectileGravityScale = 1.0f;
			InitialLifeSpan = 1.0f; // Short range
			break;
	}

	UE_LOG(LogWastelandRacers, Log, TEXT("Projectile spawned with weapon type: %d"), (int32)WeaponType);
}

void AWRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWRProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Don't hit the owner or self
	if (OtherActor && OtherActor != GetOwner() && OtherActor != this && !bHasExploded)
	{
		UE_LOG(LogWastelandRacers, Log, TEXT("Projectile hit: %s"), *OtherActor->GetName());
		
		// Call impact handling
		OnImpact(Hit);
		
		// Apply damage if it's a kart
		ApplyDamageToTarget(OtherActor);
		
		// Create explosion effect for rockets
		if (WeaponType == EWeaponType::RocketLauncher)
		{
			CreateExplosion();
		}
		
		// Play impact sound
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
		}
		
		// Spawn impact effect
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnSystemAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
		}
		
		// Mark as exploded and destroy
		Explode();
	}
}

void AWRProjectile::OnImpact(const FHitResult& HitResult)
{
	// Override in derived classes for specific impact behavior
	UE_LOG(LogWastelandRacers, Log, TEXT("Projectile impact at: %s"), *HitResult.Location.ToString());
}

void AWRProjectile::Explode()
{
	if (bHasExploded)
	{
		return;
	}
	
	bHasExploded = true;
	
	// Disable collision
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Stop movement
	ProjectileMovement->StopMovementImmediately();
	
	// Hide mesh
	ProjectileMesh->SetVisibility(false);
	
	// Stop trail effect
	if (TrailEffect)
	{
		TrailEffect->Deactivate();
	}
	
	// Destroy after a short delay to allow effects to play
	SetLifeSpan(0.5f);
}

void AWRProjectile::ApplyDamageToTarget(AActor* Target)
{
	if (!Target)
	{
		return;
	}

	// Check if target is a kart
	AWRKart* TargetKart = Cast<AWRKart>(Target);
	if (TargetKart)
	{
		TargetKart->TakeDamage(Damage);
		UE_LOG(LogWastelandRacers, Log, TEXT("Applied %.1f damage to kart: %s"), Damage, *TargetKart->GetName());
	}
	else
	{
		// Apply generic damage using UE's damage system
		UGameplayStatics::ApplyPointDamage(
			Target,
			Damage,
			GetActorLocation(),
			FHitResult(),
			nullptr,
			this,
			UDamageType::StaticClass()
		);
		
		UE_LOG(LogWastelandRacers, Log, TEXT("Applied %.1f generic damage to: %s"), Damage, *Target->GetName());
	}
}

void AWRProjectile::CreateExplosion()
{
	if (ExplosionEffect)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			World->SpawnActor<AActor>(
				ExplosionEffect,
				GetActorLocation(),
				GetActorRotation(),
				SpawnParams
			);
		}
	}

	// Apply area damage for rocket explosions
	if (WeaponType == EWeaponType::RocketLauncher)
	{
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			Damage * 0.7f, // Reduced damage for area effect
			GetActorLocation(),
			300.0f, // Explosion radius
			UDamageType::StaticClass(),
			TArray<AActor*>(), // Ignore list
			this,
			nullptr,
			true // Full damage at center
		);
		
		UE_LOG(LogWastelandRacers, Log, TEXT("Created explosion at location: %s with radius damage"), *GetActorLocation().ToString());
	}
}

#include "WRGrenade.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Engine/EngineTypes.h"

AWRGrenade::AWRGrenade()
{
	// Grenades have arc trajectory
	Speed = 600.0f;
	Damage = 0.0f; // Damage is applied by explosion
	LifeTime = 10.0f;

	if (ProjectileMovement)
	{
		ProjectileMovement->ProjectileGravityScale = 1.0f;
		ProjectileMovement->bShouldBounce = true;
		ProjectileMovement->Bounciness = 0.3f;
	}
}

void AWRGrenade::BeginPlay()
{
	Super::BeginPlay();

	// Start fuse timer
	GetWorldTimerManager().SetTimer(FuseTimer, this, &AWRGrenade::OnFuseExpired, FuseTime, false);
}

void AWRGrenade::OnImpact(const FHitResult& HitResult)
{
	// Grenades don't explode on first impact, they bounce
	if (ProjectileMovement && ProjectileMovement->bShouldBounce)
	{
		return;
	}

	// If bouncing is disabled, explode
	Explode();
}

void AWRGrenade::OnFuseExpired()
{
	Explode();
}

void AWRGrenade::Explode()
{
	if (bHasExploded)
		return;

	bHasExploded = true;

	// Apply explosion damage
	ApplyExplosionDamage();

	// Spawn explosion effect
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator);
	}

	// Play explosion sound
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}

	Destroy();
}

void AWRGrenade::ApplyExplosionDamage()
{
	TArray<FOverlapResult> OverlapResults;
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (AWRKart* HitKart = Cast<AWRKart>(Result.GetActor()))
		{
			// Calculate damage based on distance
			float Distance = FVector::Dist(GetActorLocation(), HitKart->GetActorLocation());
			float DamageMultiplier = 1.0f - (Distance / ExplosionRadius);
			float FinalDamage = ExplosionDamage * DamageMultiplier;

			// Apply knockback force
			FVector KnockbackDirection = (HitKart->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			FVector KnockbackForce = KnockbackDirection * 2000.0f * DamageMultiplier;

			// Apply force to kart (implementation depends on physics setup)
			UE_LOG(LogTemp, Warning, TEXT("Grenade explosion hit %s for %f damage"), *HitKart->GetName(), FinalDamage);
		}
	}
}

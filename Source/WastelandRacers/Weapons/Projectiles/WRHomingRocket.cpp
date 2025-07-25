#include "WRHomingRocket.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"

AWRHomingRocket::AWRHomingRocket()
{
	// Homing rockets are slower but more accurate
	Speed = 800.0f;
	Damage = 50.0f;
	LifeTime = 8.0f;
}

void AWRHomingRocket::BeginPlay()
{
	Super::BeginPlay();
	HomingTimer = 0.0f;
}

void AWRHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HomingTimer += DeltaTime;
	if (HomingTimer >= HomingDelay)
	{
		bCanHome = true;
	}

	if (bCanHome && TargetKart)
	{
		UpdateHoming(DeltaTime);
	}
}

void AWRHomingRocket::SetTarget(AWRKart* NewTarget)
{
	TargetKart = NewTarget;
}

void AWRHomingRocket::UpdateHoming(float DeltaTime)
{
	if (!TargetKart || !IsValid(TargetKart))
	{
		TargetKart = nullptr;
		return;
	}

	float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetKart->GetActorLocation());
	if (DistanceToTarget > MaxHomingDistance)
	{
		return;
	}

	FVector HomingDirection = CalculateHomingDirection();
	if (!HomingDirection.IsZero())
	{
		FVector CurrentVelocity = ProjectileMovement->Velocity;
		FVector NewVelocity = FMath::VInterpTo(CurrentVelocity, HomingDirection * Speed, DeltaTime, HomingStrength);
		ProjectileMovement->Velocity = NewVelocity;

		// Update rotation to match velocity
		SetActorRotation(NewVelocity.Rotation());
	}
}

FVector AWRHomingRocket::CalculateHomingDirection() const
{
	if (!TargetKart)
		return FVector::ZeroVector;

	FVector TargetLocation = TargetKart->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	
	// Predict target position based on target velocity
	FVector TargetVelocity = TargetKart->GetVelocity();
	float TimeToTarget = FVector::Dist(CurrentLocation, TargetLocation) / Speed;
	FVector PredictedLocation = TargetLocation + (TargetVelocity * TimeToTarget);

	return (PredictedLocation - CurrentLocation).GetSafeNormal();
}

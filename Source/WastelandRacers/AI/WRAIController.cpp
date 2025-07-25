#include "WRAIController.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Weapons/WRWeaponComponent.h"
#include "Engine/Engine.h"
#include "NavigationSystem.h"

AWRAIController::AWRAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWRAIController::BeginPlay()
{
	Super::BeginPlay();
	ControlledKart = Cast<AWRKart>(GetPawn());
}

void AWRAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ControlledKart)
		return;

	UpdateMovement(DeltaTime);
	UpdateWeaponUsage(DeltaTime);
	UpdateDrifting(DeltaTime);

	if (bEnableRubberBanding)
	{
		ApplyRubberBanding();
	}
}

void AWRAIController::UpdateMovement(float DeltaTime)
{
	// Find next waypoint/target
	CurrentTarget = FindNextWaypoint();

	// Calculate steering input
	float SteeringInput = CalculateSteeringInput(CurrentTarget);
	ControlledKart->SetSteeringInput(SteeringInput);

	// Calculate throttle input
	float ThrottleInput = CalculateThrottleInput();
	ControlledKart->SetThrottleInput(ThrottleInput);

	// Simple brake logic
	float DistanceToTarget = FVector::Dist(ControlledKart->GetActorLocation(), CurrentTarget);
	if (DistanceToTarget < 300.0f && FMath::Abs(SteeringInput) > 0.7f)
	{
		ControlledKart->SetBrakeInput(0.3f);
	}
	else
	{
		ControlledKart->SetBrakeInput(0.0f);
	}
}

void AWRAIController::UpdateWeaponUsage(float DeltaTime)
{
	WeaponCooldown -= DeltaTime;

	if (WeaponCooldown <= 0.0f && ShouldUseWeapon())
	{
		UWRWeaponComponent* WeaponComponent = ControlledKart->FindComponentByClass<UWRWeaponComponent>();
		if (WeaponComponent && WeaponComponent->HasWeapon())
		{
			WeaponComponent->FireWeapon();
			WeaponCooldown = FMath::RandRange(2.0f, 5.0f) / (Difficulty + 0.1f);
		}
	}
}

void AWRAIController::UpdateDrifting(float DeltaTime)
{
	if (bIsDrifting)
	{
		DriftTimer += DeltaTime;
		if (DriftTimer > FMath::RandRange(1.0f, 3.0f))
		{
			ControlledKart->StopDrift();
			bIsDrifting = false;
			DriftTimer = 0.0f;
		}
	}
	else if (ShouldStartDrift())
	{
		ControlledKart->StartDrift();
		bIsDrifting = true;
		DriftTimer = 0.0f;
	}
}

void AWRAIController::ApplyRubberBanding()
{
	// Get player kart position (simplified - would need proper race manager integration)
	// Apply speed boost if far behind, or slight slowdown if far ahead
	int32 AIPosition = ControlledKart->GetPosition();
	
	if (AIPosition > 4) // Behind
	{
		// Apply slight speed boost
		float BoostAmount = RubberBandStrength * (AIPosition - 4) * 0.1f;
		ControlledKart->ActivateBoost(BoostAmount);
	}
}

FVector AWRAIController::FindNextWaypoint() const
{
	// Simplified waypoint finding - in a real implementation,
	// this would use a proper track waypoint system
	FVector ForwardDirection = ControlledKart->GetActorForwardVector();
	FVector CurrentLocation = ControlledKart->GetActorLocation();
	
	// Look ahead for the next target point
	return CurrentLocation + (ForwardDirection * LookAheadDistance);
}

bool AWRAIController::ShouldUseWeapon() const
{
	// Use weapon based on difficulty and random chance
	float RandomValue = FMath::RandRange(0.0f, 1.0f);
	float AdjustedChance = WeaponUseChance * (0.5f + Difficulty * 0.5f);
	
	return RandomValue < AdjustedChance;
}

bool AWRAIController::ShouldStartDrift() const
{
	// Start drift on corners based on difficulty
	float SteeringAmount = FMath::Abs(CalculateSteeringInput(CurrentTarget));
	float RandomValue = FMath::RandRange(0.0f, 1.0f);
	float AdjustedChance = DriftChance * Difficulty;
	
	return SteeringAmount > 0.6f && RandomValue < AdjustedChance;
}

float AWRAIController::CalculateSteeringInput(const FVector& TargetLocation) const
{
	FVector ToTarget = (TargetLocation - ControlledKart->GetActorLocation()).GetSafeNormal();
	FVector Forward = ControlledKart->GetActorForwardVector();
	FVector Right = ControlledKart->GetActorRightVector();

	float DotProduct = FVector::DotProduct(ToTarget, Right);
	
	// Add some AI imperfection based on difficulty
	float Imperfection = (1.0f - Difficulty) * FMath::RandRange(-0.2f, 0.2f);
	
	return FMath::Clamp(DotProduct + Imperfection, -1.0f, 1.0f);
}

float AWRAIController::CalculateThrottleInput() const
{
	// Full throttle with slight variation based on difficulty
	float BaseThrottle = 0.8f + (Difficulty * 0.2f);
	float Variation = FMath::RandRange(-0.1f, 0.1f);
	
	return FMath::Clamp(BaseThrottle + Variation, 0.0f, 1.0f);
}

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WRAIController.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRAIController : public AAIController
{
	GENERATED_BODY()

public:
	AWRAIController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetDifficulty(float NewDifficulty) { Difficulty = FMath::Clamp(NewDifficulty, 0.0f, 1.0f); }

	UFUNCTION(BlueprintPure, Category = "AI")
	float GetDifficulty() const { return Difficulty; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Difficulty = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LookAheadDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WeaponUseChance = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DriftChance = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bEnableRubberBanding = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RubberBandStrength = 0.2f;

private:
	class AWRKart* ControlledKart;
	FVector CurrentTarget;
	float WeaponCooldown = 0.0f;
	float DriftTimer = 0.0f;
	bool bIsDrifting = false;

	void UpdateMovement(float DeltaTime);
	void UpdateWeaponUsage(float DeltaTime);
	void UpdateDrifting(float DeltaTime);
	void ApplyRubberBanding();

	FVector FindNextWaypoint() const;
	bool ShouldUseWeapon() const;
	bool ShouldStartDrift() const;
	float CalculateSteeringInput(const FVector& TargetLocation) const;
	float CalculateThrottleInput() const;
};

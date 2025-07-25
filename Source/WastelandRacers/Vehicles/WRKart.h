#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "WheeledVehiclePawn.h"
#include "WastelandRacers/Weapons/WRWeaponComponent.h"
#include "WastelandRacers/Gameplay/WRPowerUpComponent.h"
#include "WastelandRacers/Core/WREngineClass.h"
#include "WRKart.generated.h"

UENUM(BlueprintType)
enum class EKartType : uint8
{
	Balanced,
	Speed,
	Acceleration,
	Handling,
	Heavy
};

USTRUCT(BlueprintType)
struct FKartStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Handling = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoostPower = 2000.0f;
};

UCLASS()
class WASTELANDRACERS_API AWRKart : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AWRKart();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input functions
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetThrottleInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetSteeringInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetBrakeInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void UseWeapon();

	UFUNCTION(BlueprintCallable, Category = "Drift")
	void StartDrift();

	UFUNCTION(BlueprintCallable, Category = "Drift")
	void StopDrift();

	UFUNCTION(BlueprintCallable, Category = "Boost")
	void ActivateBoost(float BoostAmount);

	// Getters
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	bool IsDrifting() const { return bIsDrifting; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetDriftTime() const { return DriftTime; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	int32 GetCurrentLap() const { return CurrentLap; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	int32 GetPosition() const { return RacePosition; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWRWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWRPowerUpComponent* PowerUpComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* BoostEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* DriftEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart")
	EKartType KartType = EKartType::Balanced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart")
	FKartStats KartStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	EEngineClass EngineClass = EEngineClass::EC_100cc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drift")
	float DriftThreshold = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drift")
	float MiniTurboTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drift")
	float SuperMiniTurboTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drift")
	float UltraMiniTurboTime = 4.0f;

public:
	bool IsBoosting() const { return bIsBoosting; }

private:
	bool bIsDrifting = false;
	float DriftTime = 0.0f;
	float CurrentThrottle = 0.0f;
	float CurrentSteering = 0.0f;
	float CurrentBrake = 0.0f;

	// Race tracking
	int32 CurrentLap = 1;
	int32 RacePosition = 1;
	int32 CheckpointsPassed = 0;

	// Boost system
	bool bIsBoosting = false;
	float BoostTimeRemaining = 0.0f;

	void UpdateDrift(float DeltaTime);
	void ApplyDriftBoost();
	void UpdateBoost(float DeltaTime);
	void ApplyKartStats();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

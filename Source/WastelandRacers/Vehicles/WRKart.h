#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "ChaosWheeledVehiclePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "WastelandRacers/WastelandRacers.h"
#include "WRKart.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRKart : public AChaosWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AWRKart();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();
	void OnBoostPressed();
	void OnFireWeapon();

	// Kart properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* KartMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* EngineAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWRWeaponComponent* WeaponComponent;

	// Boost system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost")
	float MaxBoostEnergy = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Boost")
	float CurrentBoostEnergy = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost")
	float BoostConsumptionRate = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost")
	float BoostRechargeRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost")
	float BoostMultiplier = 1.5f;

	// Health system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Boost")
	void UseBoost(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Boost")
	void RechargeBoost(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void RepairKart(float RepairAmount);

	UFUNCTION(BlueprintPure, Category = "Status")
	bool IsDestroyed() const { return CurrentHealth <= 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Status")
	float GetHealthPercentage() const { return CurrentHealth / MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Status")
	float GetBoostPercentage() const { return CurrentBoostEnergy / MaxBoostEnergy; }

private:
	bool bIsBoosting = false;
	bool bIsHandbrakePressed = false;
	float ThrottleInput = 0.0f;
	float SteeringInput = 0.0f;
};

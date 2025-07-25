#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WastelandRacers/Weapons/WRWeaponTypes.h"
#include "WRPowerUpComponent.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
	None,
	Weapon,
	SpeedBoost,
	Shield,
	Repair
};

USTRUCT(BlueprintType)
struct FPowerUpData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPowerUpType PowerUpType = EPowerUpType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude = 1.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WASTELANDRACERS_API UWRPowerUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWRPowerUpComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "PowerUp")
	void CollectPowerUp(const FPowerUpData& PowerUpData);

	UFUNCTION(BlueprintCallable, Category = "PowerUp")
	void ActivatePowerUp();

	UFUNCTION(BlueprintPure, Category = "PowerUp")
	bool HasPowerUp() const { return CurrentPowerUp.PowerUpType != EPowerUpType::None; }

	UFUNCTION(BlueprintPure, Category = "PowerUp")
	FPowerUpData GetCurrentPowerUp() const { return CurrentPowerUp; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerUp")
	FPowerUpData CurrentPowerUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	TArray<FPowerUpData> AvailablePowerUps;

private:
	void ApplySpeedBoost(float Magnitude, float Duration);
	void ApplyShield(float Duration);
	void ApplyRepair(float Amount);
	void GiveWeapon(EWeaponType WeaponType);

	class AWRKart* GetOwnerKart() const;
};

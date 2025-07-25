#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "WRWeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	MachineGun UMETA(DisplayName = "Machine Gun"),
	RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Flamethrower UMETA(DisplayName = "Flamethrower")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WASTELANDRACERS_API UWRWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWRWeaponComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponType(EWeaponType NewWeaponType);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool CanFire() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponType GetWeaponType() const { return CurrentWeaponType; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType CurrentWeaponType = EWeaponType::MachineGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo = 30;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate = 0.1f; // Time between shots

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ReloadTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<class AWRProjectile> ProjectileClass;

private:
	float LastFireTime = 0.0f;
	bool bIsReloading = false;
	float ReloadStartTime = 0.0f;

	void FireMachineGun();
	void FireRocketLauncher();
	void FireShotgun();
	void FireFlamethrower();
	
	void SpawnProjectile(const FVector& StartLocation, const FVector& Direction);
	FVector GetFireDirection() const;
	FVector GetMuzzleLocation() const;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WRWeaponTypes.h"
#include "WRWeaponComponent.generated.h"

// Forward declarations
class AWRKart;
class AWRProjectile;
class USceneComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WASTELANDRACERS_API UWRWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWRWeaponComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void FireWeapon();

	UFUNCTION(BlueprintCallable)
	bool HasWeapon() const { return CurrentWeapon != EWeaponType::None; }

	UFUNCTION(BlueprintPure)
	EWeaponType GetCurrentWeapon() const { return CurrentWeapon; }

	AWRKart* FindNearestTarget() const;
	AWRKart* GetOwnerKart() const;

	void SetCurrentWeapon(EWeaponType NewWeaponType);
	void AddWeapon(EWeaponType WeaponType);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType CurrentWeapon = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<TSubclassOf<AWRProjectile>> ProjectileClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USceneComponent* WeaponMountPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireCooldown = 1.0f;

private:
	float LastFireTime = 0.0f;

	void FireHomingRocket();
	void FireShotgunBlast();
	void DeployOilSlick();
	void ThrowGrenade();
	void ApplySlagDebuff();
	void ActivateShield();
	void ActivateSpeedBoost();
	void UseTeleportPad();
	void DeployDecoy();
};

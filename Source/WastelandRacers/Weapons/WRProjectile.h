#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "WastelandRacers/Weapons/WRWeaponComponent.h"
#include "WRProjectile.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRProjectile : public AActor
{
	GENERATED_BODY()

public:
	AWRProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetDamage(float NewDamage) { Damage = NewDamage; }

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetWeaponType(EWeaponType NewWeaponType) { WeaponType = NewWeaponType; }

	UFUNCTION(BlueprintPure, Category = "Projectile")
	float GetDamage() const { return Damage; }

	UFUNCTION(BlueprintPure, Category = "Projectile")
	bool HasExploded() const { return bHasExploded; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* TrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float LifeSpan = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Speed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	EWeaponType WeaponType = EWeaponType::MachineGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<class AActor> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ImpactSound;

private:
	bool bHasExploded = false;
	
	void ApplyDamageToTarget(AActor* Target);
	void CreateExplosion();
	virtual void OnImpact(const FHitResult& HitResult);
	virtual void Explode();
};

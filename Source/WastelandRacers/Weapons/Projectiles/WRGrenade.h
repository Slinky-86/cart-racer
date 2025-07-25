#pragma once

#include "CoreMinimal.h"
#include "WRProjectile.h"
#include "WRGrenade.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRGrenade : public AWRProjectile
{
	GENERATED_BODY()

public:
	AWRGrenade();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionDamage = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float FuseTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* ExplosionEffect;

	virtual void OnImpact(const FHitResult& HitResult) override;
	virtual void Explode() override;

private:
	FTimerHandle FuseTimer;

	UFUNCTION()
	void OnFuseExpired();

	void ApplyExplosionDamage();
};

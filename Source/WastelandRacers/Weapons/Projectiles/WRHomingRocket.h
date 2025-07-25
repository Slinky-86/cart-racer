#pragma once

#include "CoreMinimal.h"
#include "WRProjectile.h"
#include "WRHomingRocket.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRHomingRocket : public AWRProjectile
{
	GENERATED_BODY()

public:
	AWRHomingRocket();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Homing")
	void SetTarget(class AWRKart* NewTarget);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float HomingStrength = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float MaxHomingDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float HomingDelay = 0.5f;

private:
	UPROPERTY()
	class AWRKart* TargetKart;

	float HomingTimer = 0.0f;
	bool bCanHome = false;

	void UpdateHoming(float DeltaTime);
	FVector CalculateHomingDirection() const;
};

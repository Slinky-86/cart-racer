#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WastelandRacers/Gameplay/WRPowerUpComponent.h"
#include "Components/SphereComponent.h"
#include "WRPowerUpSpawner.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRPowerUpSpawner : public AActor
{
	GENERATED_BODY()

public:
	AWRPowerUpSpawner();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "PowerUp")
	void SpawnPowerUp();

	UFUNCTION(BlueprintCallable, Category = "PowerUp")
	void CollectPowerUp(class AWRKart* Kart);

	UFUNCTION(BlueprintPure, Category = "PowerUp")
	bool HasPowerUp() const { return bHasPowerUp; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollectionTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* PowerUpMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* PowerUpEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* PowerUpAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	TArray<FPowerUpData> AvailablePowerUps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	float RespawnTime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	bool bRandomizePowerUp = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	FPowerUpData FixedPowerUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* CollectionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* CollectionSound;

private:
	// Restricts each kart to collect this power-up only once per lap.
	TMap<TWeakObjectPtr<class AWRKart>, int32> LastLapCollected;

	bool bHasPowerUp = true;
	FPowerUpData CurrentPowerUp;
	FTimerHandle RespawnTimer;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetupPowerUpAppearance();
	FPowerUpData SelectRandomPowerUp() const;
	void PlayCollectionEffects();
};

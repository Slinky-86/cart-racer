#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "WRTrackHazard.generated.h"

UENUM(BlueprintType)
enum class EHazardType : uint8
{
	AcidPool,
	ExplosiveBarrel,
	ElectricFence,
	OilSlick,
	SteamVent,
	LaserGrid,
	SpikeTrap,
	ToxicGas
};

UCLASS()
class WASTELANDRACERS_API AWRTrackHazard : public AActor
{
	GENERATED_BODY()

public:
	AWRTrackHazard();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Hazard")
	void SetHazardType(EHazardType Type);

	UFUNCTION(BlueprintPure, Category = "Hazard")
	EHazardType GetHazardType() const { return HazardType; }

	UFUNCTION(BlueprintCallable, Category = "Hazard")
	void ActivateHazard();

	UFUNCTION(BlueprintCallable, Category = "Hazard")
	void DeactivateHazard();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* HazardTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* HazardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* HazardEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* HazardAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard")
	EHazardType HazardType = EHazardType::AcidPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard")
	float SlowdownFactor = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard")
	float EffectDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard")
	bool bIsActive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard")
	bool bIsContinuous = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard")
	float ActivationCooldown = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* ActivationEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ActivationSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* DamageSound;

private:
	float LastActivationTime = 0.0f;
	TSet<class AWRKart*> AffectedKarts;

	UFUNCTION()
	void OnHazardBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHazardEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ApplyHazardEffect(class AWRKart* Kart);
	void RemoveHazardEffect(class AWRKart* Kart);
	void SetupHazardAppearance();
};

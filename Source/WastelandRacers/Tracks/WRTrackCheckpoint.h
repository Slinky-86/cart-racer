#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "WRTrackCheckpoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCheckpointPassed, class AWRKart*, Kart, int32, CheckpointIndex);

UCLASS()
class WASTELANDRACERS_API AWRTrackCheckpoint : public AActor
{
	GENERATED_BODY()

public:
	AWRTrackCheckpoint();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetCheckpointIndex(int32 Index) { CheckpointIndex = Index; }

	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	int32 GetCheckpointIndex() const { return CheckpointIndex; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetIsFinishLine(bool bFinish) { bIsFinishLine = bFinish; }

	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	bool IsFinishLine() const { return bIsFinishLine; }

	UPROPERTY(BlueprintAssignable)
	FOnCheckpointPassed OnCheckpointPassed;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* CheckpointMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* CheckpointEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	int32 CheckpointIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	bool bIsFinishLine = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* PassEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* PassSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* FinishLineSound;

private:
	TSet<class AWRKart*> KartsPassedThisLap;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PlayCheckpointEffects(class AWRKart* Kart);
};

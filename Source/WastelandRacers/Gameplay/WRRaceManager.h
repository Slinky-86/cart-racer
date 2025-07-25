#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WastelandRacers/Story/WRStoryManager.h"
#include "WRRaceManager.generated.h"

UENUM(BlueprintType)
enum class ERaceState : uint8
{
	Waiting,
	Countdown,
	Racing,
	Finished
};

USTRUCT(BlueprintType)
struct FRaceResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	class AWRKart* Kart;

	UPROPERTY(BlueprintReadWrite)
	int32 Position;

	UPROPERTY(BlueprintReadWrite)
	float FinishTime;

	UPROPERTY(BlueprintReadWrite)
	int32 LapsCompleted;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaceStateChanged, ERaceState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKartFinished, const FRaceResult&, Result);

UCLASS()
class WASTELANDRACERS_API AWRRaceManager : public AActor
{
	GENERATED_BODY()

public:
	AWRRaceManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Race")
	void Initialize(int32 TotalLaps, int32 MaxPlayers);

	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRace();

	UFUNCTION(BlueprintCallable, Category = "Race")
	void EndRace();

	UFUNCTION(BlueprintCallable, Category = "Race")
	void RegisterKart(class AWRKart* Kart);

	UFUNCTION(BlueprintCallable, Category = "Race")
	void OnKartPassedCheckpoint(class AWRKart* Kart, int32 CheckpointIndex);

	UFUNCTION(BlueprintCallable, Category = "Race")
	void OnKartCompletedLap(class AWRKart* Kart);

	UFUNCTION(BlueprintPure, Category = "Race")
	ERaceState GetRaceState() const { return CurrentRaceState; }

	UFUNCTION(BlueprintPure, Category = "Race")
	float GetRaceTime() const { return RaceTime; }

	UFUNCTION(BlueprintPure, Category = "Race")
	int32 GetKartPosition(class AWRKart* Kart) const;

	UFUNCTION(BlueprintCallable, Category = "Race")
	void OnRaceCompleted();

	UFUNCTION(BlueprintCallable, Category = "Race")
	void AwardRacePoints(class AWRKart* Kart, int32 Position);

	UPROPERTY(BlueprintAssignable)
	FOnRaceStateChanged OnRaceStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnKartFinished OnKartFinished;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	int32 TotalLaps = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	int32 MaxPlayers = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	float CountdownTime = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race")
	ERaceState CurrentRaceState = ERaceState::Waiting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race")
	TArray<class AWRKart*> RegisteredKarts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race")
	TArray<FRaceResult> RaceResults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	TArray<int32> PositionRewards = {1000, 750, 500, 300, 200};

private:
	float RaceTime = 0.0f;
	float CountdownTimer = 0.0f;
	int32 FinishedKarts = 0;

	void UpdateRaceState(ERaceState NewState);
	void UpdateKartPositions();
	void CalculateKartProgress(class AWRKart* Kart, float& Progress) const;
};

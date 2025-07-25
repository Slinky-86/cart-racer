#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WastelandRacers/Gameplay/WRRaceManager.h"
#include "WastelandRacers/Story/WRStoryManager.h"
#include "WRGameMode.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWRGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRace();

	UFUNCTION(BlueprintCallable, Category = "Race")
	void EndRace();

	UFUNCTION(BlueprintCallable, Category = "Race")
	bool IsRaceActive() const { return bRaceActive; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race")
	class AWRRaceManager* RaceManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Story")
	class AWRStoryManager* StoryManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	int32 MaxPlayers = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	float RaceCountdownTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	int32 TotalLaps = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	bool bIsStoryMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	bool bIsFreeRoam = false;

private:
	bool bRaceActive = false;
	int32 ConnectedPlayers = 0;

	UFUNCTION()
	void OnCountdownFinished();
};

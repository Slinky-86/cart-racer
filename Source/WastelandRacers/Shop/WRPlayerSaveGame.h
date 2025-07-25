#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WastelandRacers/Core/WREngineClass.h"
#include "WRPlayerSaveGame.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	int32 RacePoints = 0;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	TSet<EEngineClass> UnlockedEngineClasses;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	TSet<FName> OwnedUpgradeIds;
};

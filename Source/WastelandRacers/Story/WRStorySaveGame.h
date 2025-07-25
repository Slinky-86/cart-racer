#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WRStoryTypes.h"
#include "WRStorySaveGame.generated.h"

UCLASS(BlueprintType)
class WASTELANDRACERS_API UWRStorySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<EStoryWorld, bool> UnlockedWorlds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<ECupSeries, bool> CompletedCups;
};

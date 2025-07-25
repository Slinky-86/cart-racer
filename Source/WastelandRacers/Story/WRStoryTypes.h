#pragma once
#include "CoreMinimal.h"
#include "WRStoryTypes.generated.h"

UENUM(BlueprintType)
enum class EStoryWorld : uint8
{
    Pandora,
    Opportunity,
    EridiumMines,
    WildlifePreserve,
    HyperionMoonBase
};

UENUM(BlueprintType)
enum class ECupSeries : uint8
{
    PandoraCup,
    OpportunityCup,
    EridiumCup,
    WildlifeCup,
    HyperionCup,
    ChampionshipCup
};

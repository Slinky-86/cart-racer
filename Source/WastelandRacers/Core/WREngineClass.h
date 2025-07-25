#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEngineClass : uint8
{
	EC_100cc UMETA(DisplayName = "100cc"),
	EC_150cc UMETA(DisplayName = "150cc"),
	EC_200cc UMETA(DisplayName = "200cc"),
	EC_Mirror UMETA(DisplayName = "Mirror")
};

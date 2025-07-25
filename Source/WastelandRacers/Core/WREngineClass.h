#pragma once

#include "CoreMinimal.h"
#include "WREngineClass.generated.h"

UENUM(BlueprintType)
enum class EEngineClass : uint8
{
	EC_100cc UMETA(DisplayName = "100cc"),
	EC_200cc UMETA(DisplayName = "200cc"),
	EC_300cc UMETA(DisplayName = "300cc")
};

inline float GetEngineClassMultiplier(EEngineClass C)
{
	return C == EEngineClass::EC_100cc ? 1.f : (C == EEngineClass::EC_200cc ? 1.25f : 1.5f);
}

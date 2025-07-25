#pragma once

#include "CoreMinimal.h"
#include "WRWeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	HomingRocket,
	ShotgunBlast,
	OilSlick,
	Grenade,
	SlagDebuff,
	Shield,
	SpeedBoost,
	TeleportPad,
	Decoy
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDefensive = false;
};

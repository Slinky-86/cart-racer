#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WRKartData.generated.h"

UENUM(BlueprintType)
enum class EKartClass : uint8
{
	BanditTechnical,
	PsychoBuggy,
	HyperionScout,
	DahlCivilian,
	VladofRunner,
	MaliwanSpeeder,
	TorgueDestroyer,
	JakobsCruiser
};

USTRUCT(BlueprintType)
struct FKartCustomization
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UMaterialInterface*> PaintJobs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UStaticMesh*> SpoilerOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UStaticMesh*> WheelOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UStaticMesh*> ExhaustOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UStaticMesh*> ArmorOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UNiagaraSystem*> ExhaustEffects;
};

UCLASS(BlueprintType)
class WASTELANDRACERS_API UWRKartData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart")
	EKartClass KartClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart")
	FString KartName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart")
	FString KartDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart")
	class UTexture2D* KartPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart")
	class USkeletalMesh* KartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FKartStats KartStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* EngineSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* HornSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* BoostSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
	FKartCustomization CustomizationOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
	bool bIsUnlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
	int32 UnlockRequirement = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
	FString UnlockDescription;

	UFUNCTION(BlueprintPure, Category = "Kart")
	float GetOverallRating() const;

	UFUNCTION(BlueprintCallable, Category = "Kart")
	void ApplyToKart(class AWRKart* Kart) const;
};

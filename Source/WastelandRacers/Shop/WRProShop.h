#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WastelandRacers/Core/WREngineClass.h"
#include "WRProShop.generated.h"

UCLASS(BlueprintType)
class WASTELANDRACERS_API UWRProShop : public UObject
{
	GENERATED_BODY()
public:
	UWRProShop();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProShop")
	TSoftObjectPtr<UWorld> ShowroomLevel;

	UPROPERTY(BlueprintReadWrite, Category = "ProShop")
	int32 RacePoints = 0;

	UPROPERTY(BlueprintReadWrite, Category = "ProShop")
	TSet<EEngineClass> UnlockedEngineClasses;

	UPROPERTY(BlueprintReadWrite, Category = "ProShop")
	TSet<FName> OwnedUpgradeIds;

	// Persistence
	void LoadPlayerProgress();
	void SavePlayerProgress() const;

	// Upgrade logic
	bool PurchaseUpgrade(FName UpgradeId, int32 Cost);
	bool IsUpgradeOwned(FName UpgradeId) const;

	// Cosmetic application
	void ApplyPaintUpgrade(class AWRKart* Kart, UMaterialInterface* PaintMat);

	UFUNCTION(BlueprintCallable, Category = "ProShop")
	void AddRacePoints(int32 Points);

	UFUNCTION(BlueprintCallable, Category = "ProShop")
	bool PurchaseEngineClass(EEngineClass ClassToBuy, int32 Cost);

	UFUNCTION(BlueprintPure, Category = "ProShop")
	bool IsEngineClassUnlocked(EEngineClass Class) const;

public:
	bool EnterDealership(const UObject* WorldContext);
};

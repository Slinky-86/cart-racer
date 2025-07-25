#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WRProShop.generated.h"

USTRUCT(BlueprintType)
struct FVehicleData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Vehicle")
	FString VehicleName;

	UPROPERTY(BlueprintReadWrite, Category = "Vehicle")
	int32 Price;

	UPROPERTY(BlueprintReadWrite, Category = "Vehicle")
	bool bIsUnlocked;

	UPROPERTY(BlueprintReadWrite, Category = "Vehicle")
	float Speed;

	UPROPERTY(BlueprintReadWrite, Category = "Vehicle")
	float Acceleration;

	UPROPERTY(BlueprintReadWrite, Category = "Vehicle")
	float Handling;

	FVehicleData()
	{
		VehicleName = TEXT("Default Kart");
		Price = 0;
		bIsUnlocked = true;
		Speed = 100.0f;
		Acceleration = 100.0f;
		Handling = 100.0f;
	}
};

UCLASS(BlueprintType)
class WASTELANDRACERS_API UWRProShop : public UDataAsset
{
	GENERATED_BODY()

public:
	UWRProShop();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool EnterDealership(class UWRGameInstance* GameInstance);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void LoadPlayerProgress();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool PurchaseVehicle(int32 VehicleIndex);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	TArray<FVehicleData> GetAvailableVehicles() const { return AvailableVehicles; }

	UFUNCTION(BlueprintCallable, Category = "Shop")
	int32 GetPlayerCurrency() const { return PlayerCurrency; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	TArray<FVehicleData> AvailableVehicles;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	int32 PlayerCurrency;
};

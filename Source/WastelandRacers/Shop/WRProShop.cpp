#include "WRProShop.h"
#include "WastelandRacers/WastelandRacers.h"
#include "WastelandRacers/Core/WRGameInstance.h"
#include "Kismet/GameplayStatics.h"

UWRProShop::UWRProShop()
{
	PlayerCurrency = 1000;

	// Initialize default vehicles
	FVehicleData DefaultKart;
	DefaultKart.VehicleName = TEXT("Starter Kart");
	DefaultKart.Price = 0;
	DefaultKart.bIsUnlocked = true;
	DefaultKart.Speed = 80.0f;
	DefaultKart.Acceleration = 90.0f;
	DefaultKart.Handling = 85.0f;
	AvailableVehicles.Add(DefaultKart);

	FVehicleData SpeedKart;
	SpeedKart.VehicleName = TEXT("Speed Demon");
	SpeedKart.Price = 500;
	SpeedKart.bIsUnlocked = false;
	SpeedKart.Speed = 120.0f;
	SpeedKart.Acceleration = 70.0f;
	SpeedKart.Handling = 60.0f;
	AvailableVehicles.Add(SpeedKart);

	FVehicleData BalancedKart;
	BalancedKart.VehicleName = TEXT("All-Rounder");
	BalancedKart.Price = 750;
	BalancedKart.bIsUnlocked = false;
	BalancedKart.Speed = 100.0f;
	BalancedKart.Acceleration = 100.0f;
	BalancedKart.Handling = 100.0f;
	AvailableVehicles.Add(BalancedKart);
}

bool UWRProShop::EnterDealership(UWRGameInstance* GameInstance)
{
	if (!GameInstance)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Invalid GameInstance passed to EnterDealership"));
		return false;
	}

	// Try to load the dealership level
	UGameplayStatics::OpenLevel(GameInstance->GetWorld(), TEXT("ProShop_Showroom"));
	return true;
}

void UWRProShop::LoadPlayerProgress()
{
	// TODO: Load from save game
	UE_LOG(LogWastelandRacers, Log, TEXT("Loading player progress for Pro Shop"));
}

bool UWRProShop::PurchaseVehicle(int32 VehicleIndex)
{
	if (!AvailableVehicles.IsValidIndex(VehicleIndex))
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Invalid vehicle index: %d"), VehicleIndex);
		return false;
	}

	FVehicleData& Vehicle = AvailableVehicles[VehicleIndex];
	
	if (Vehicle.bIsUnlocked)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Vehicle already unlocked: %s"), *Vehicle.VehicleName);
		return false;
	}

	if (PlayerCurrency < Vehicle.Price)
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("Insufficient currency for vehicle: %s"), *Vehicle.VehicleName);
		return false;
	}

	PlayerCurrency -= Vehicle.Price;
	Vehicle.bIsUnlocked = true;
	
	UE_LOG(LogWastelandRacers, Log, TEXT("Purchased vehicle: %s"), *Vehicle.VehicleName);
	return true;
}

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WRHUDWidget.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetOwningPlayer(class AWRPlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateSpeedometer(float Speed);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdatePosition(int32 Position, int32 TotalPlayers);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateLapCounter(int32 CurrentLap, int32 TotalLaps);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateRaceTime(float RaceTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateWeaponDisplay(class UTexture2D* WeaponIcon);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ShowCountdown(int32 CountdownNumber);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void HideCountdown();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ShowRaceFinished(int32 FinalPosition);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	class AWRPlayerController* OwningPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	class AWRKart* PlayerKart;

	// Mobile touch controls
	UPROPERTY(meta = (BindWidget))
	class UButton* AccelerateButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BrakeButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* WeaponButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* DriftButton;

	UPROPERTY(meta = (BindWidget))
	class UWidget* SteeringWheel;

private:
	UFUNCTION()
	void OnAcceleratePressed();

	UFUNCTION()
	void OnAccelerateReleased();

	UFUNCTION()
	void OnBrakePressed();

	UFUNCTION()
	void OnBrakeReleased();

	UFUNCTION()
	void OnWeaponPressed();

	UFUNCTION()
	void OnDriftPressed();

	UFUNCTION()
	void OnDriftReleased();

	void UpdateHUDElements();
};

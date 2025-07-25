#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WRMinimalHUD.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRMinimalHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetOwningPlayer(class AWRPlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdatePosition(int32 Position, int32 TotalPlayers);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateLapCounter(int32 CurrentLap, int32 TotalLaps);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ShowCountdown(int32 CountdownNumber);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void HideCountdown();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ShowRaceFinished(int32 FinalPosition);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetHUDVisibility(bool bVisible);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	class AWRPlayerController* OwningPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	class AWRKart* PlayerKart;

	// UI Elements
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PositionText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LapText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountdownText;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* CountdownOverlay;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RaceFinishedText;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* RaceFinishedOverlay;

private:
	void UpdateHUDElements();
	FString GetOrdinalSuffix(int32 Number) const;
};

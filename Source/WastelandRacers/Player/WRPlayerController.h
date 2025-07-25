#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WastelandRacers/UI/WRHUDWidget.h"
#include "WastelandRacers/UI/WRProShopWidget.h"
#include "WastelandRacers/Story/WRStoryManager.h"
#include "WRPlayerController.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWRPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideHUD();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetMobileInputEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenProShop();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseProShop();

	UFUNCTION(BlueprintCallable, Category = "Story")
	void OpenWorldMap();

	UFUNCTION(BlueprintCallable, Category = "Story")
	void TravelToRace(ETrackType TrackType);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UWRHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWRHUDWidget* HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UWRProShopWidget> ProShopWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWRProShopWidget* ProShopWidget;

	// Input Actions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Brake(float Value);
	void UseWeapon();
	void Drift();
	void StopDrift();

public:
	// Mobile touch input
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchAccelerate();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchBrake();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchSteer(float SteerValue);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchWeapon();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchDrift(bool bStartDrift);

private:
	bool bMobileInputEnabled = false;
};

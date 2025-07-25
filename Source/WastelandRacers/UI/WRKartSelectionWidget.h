#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WastelandRacers/Vehicles/WRKartData.h"
#include "WRKartSelectionWidget.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRKartSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Kart Selection")
	void PopulateKartList();

	UFUNCTION(BlueprintCallable, Category = "Kart Selection")
	void OnKartSelected(EKartClass KartClass);

	UFUNCTION(BlueprintCallable, Category = "Kart Selection")
	void OnConfirmClicked();

	UFUNCTION(BlueprintCallable, Category = "Kart Selection")
	void OnBackClicked();

	UFUNCTION(BlueprintCallable, Category = "Kart Selection")
	void OnCustomizeClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* KartListBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* KartPreviewImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KartNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KartDescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SpeedStatBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AccelerationStatBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HandlingStatBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* WeightStatBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OverallRatingText;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CustomizeButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> KartEntryWidgetClass;

private:
	EKartClass SelectedKart = EKartClass::BanditTechnical;

	void UpdateKartPreview();
	void UpdateStatBars(const FKartStats& Stats);
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WastelandRacers/Tracks/WRTrackManager.h"
#include "WRTrackSelectionWidget.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRTrackSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Track Selection")
	void PopulateTrackList();

	UFUNCTION(BlueprintCallable, Category = "Track Selection")
	void OnTrackSelected(ETrackType TrackType);

	UFUNCTION(BlueprintCallable, Category = "Track Selection")
	void OnStartRaceClicked();

	UFUNCTION(BlueprintCallable, Category = "Track Selection")
	void OnBackClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* TrackListScrollBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* TrackPreviewImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TrackNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TrackDescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TrackDifficultyText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TrackLengthText;

	UPROPERTY(meta = (BindWidget))
	class UButton* StartRaceButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> TrackEntryWidgetClass;

private:
	ETrackType SelectedTrack = ETrackType::PandoraDesert;

	void UpdateTrackPreview();
};

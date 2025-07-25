#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WastelandRacers/Audio/WRAudioManager.h"
#include "WRSettingsWidget.generated.h"

USTRUCT(BlueprintType)
struct FGameSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GraphicsQuality = 2; // 0=Low, 1=Medium, 2=High, 3=Ultra

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bVSync = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetFrameRate = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FieldOfView = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInvertYAxis = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowFPS = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableHapticFeedback = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWRGameAudioSettings AudioSettings;
};

UCLASS()
class WASTELANDRACERS_API UWRSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetToDefaults();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void OnBackClicked();

protected:
	// Graphics Settings
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* GraphicsQualityCombo;

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* VSyncCheckBox;

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* FrameRateCombo;

	UPROPERTY(meta = (BindWidget))
	class USlider* FOVSlider;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FOVValueText;

	// Audio Settings
	UPROPERTY(meta = (BindWidget))
	class USlider* MasterVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	class USlider* MusicVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	class USlider* SFXVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	class USlider* VoiceVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	class USlider* EngineVolumeSlider;

	// Control Settings
	UPROPERTY(meta = (BindWidget))
	class UCheckBox* InvertYCheckBox;

	UPROPERTY(meta = (BindWidget))
	class USlider* MouseSensitivitySlider;

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* HapticFeedbackCheckBox;

	// UI Settings
	UPROPERTY(meta = (BindWidget))
	class UCheckBox* ShowFPSCheckBox;

	// Buttons
	UPROPERTY(meta = (BindWidget))
	class UButton* ApplyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ResetButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	FGameSettings CurrentSettings;

private:
	void BindEvents();
	void UpdateUI();
	void ApplyGraphicsSettings();
	void ApplyAudioSettings();
	void ApplyControlSettings();

	UFUNCTION()
	void OnFOVChanged(float Value);

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnMusicVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UFUNCTION()
	void OnVoiceVolumeChanged(float Value);

	UFUNCTION()
	void OnEngineVolumeChanged(float Value);
};

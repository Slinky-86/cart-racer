#include "WRSettingsWidget.h"
#include "WastelandRacers/Audio/WRAudioManager.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

void UWRSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindEvents();
	LoadSettings();
	UpdateUI();
}

void UWRSettingsWidget::BindEvents()
{
	if (FOVSlider)
	{
		FOVSlider->OnValueChanged.AddDynamic(this, &UWRSettingsWidget::OnFOVChanged);
	}

	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UWRSettingsWidget::OnMasterVolumeChanged);
	}

	if (MusicVolumeSlider)
	{
		MusicVolumeSlider->OnValueChanged.AddDynamic(this, &UWRSettingsWidget::OnMusicVolumeChanged);
	}

	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(this, &UWRSettingsWidget::OnSFXVolumeChanged);
	}

	if (VoiceVolumeSlider)
	{
		VoiceVolumeSlider->OnValueChanged.AddDynamic(this, &UWRSettingsWidget::OnVoiceVolumeChanged);
	}

	if (EngineVolumeSlider)
	{
		EngineVolumeSlider->OnValueChanged.AddDynamic(this, &UWRSettingsWidget::OnEngineVolumeChanged);
	}

	if (ApplyButton)
	{
		ApplyButton->OnClicked.AddDynamic(this, &UWRSettingsWidget::ApplySettings);
	}

	if (ResetButton)
	{
		ResetButton->OnClicked.AddDynamic(this, &UWRSettingsWidget::ResetToDefaults);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UWRSettingsWidget::OnBackClicked);
	}
}

void UWRSettingsWidget::LoadSettings()
{
	// Load from game user settings
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		CurrentSettings.GraphicsQuality = UserSettings->GetOverallScalabilityLevel();
		CurrentSettings.bVSync = UserSettings->IsVSyncEnabled();
		CurrentSettings.TargetFrameRate = UserSettings->GetFrameRateLimit();
	}

	// Load audio settings
	if (UWRAudioManager* AudioManager = UWRAudioManager::GetInstance(GetWorld()))
	{
		CurrentSettings.AudioSettings = AudioManager->GetAudioSettings();
	}

	// Load custom settings from config or save file
	// Implementation depends on your save system
}

void UWRSettingsWidget::SaveSettings()
{
	// Save to game user settings
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->SetOverallScalabilityLevel(CurrentSettings.GraphicsQuality);
		UserSettings->SetVSyncEnabled(CurrentSettings.bVSync);
		UserSettings->SetFrameRateLimit(CurrentSettings.TargetFrameRate);
		UserSettings->SaveSettings();
	}

	// Save audio settings
	if (UWRAudioManager* AudioManager = UWRAudioManager::GetInstance(GetWorld()))
	{
		AudioManager->SetAudioSettings(CurrentSettings.AudioSettings);
		AudioManager->SaveAudioSettings();
	}

	UE_LOG(LogTemp, Warning, TEXT("Settings saved"));
}

void UWRSettingsWidget::ApplySettings()
{
	ApplyGraphicsSettings();
	ApplyAudioSettings();
	ApplyControlSettings();
	SaveSettings();
}

void UWRSettingsWidget::ResetToDefaults()
{
	CurrentSettings = FGameSettings();
	UpdateUI();
	ApplySettings();
}

void UWRSettingsWidget::OnBackClicked()
{
	// Return to main menu
	SetVisibility(ESlateVisibility::Hidden);
	RemoveFromParent();
}

void UWRSettingsWidget::UpdateUI()
{
	// Update graphics UI
	if (GraphicsQualityCombo)
	{
		GraphicsQualityCombo->SetSelectedIndex(CurrentSettings.GraphicsQuality);
	}

	if (VSyncCheckBox)
	{
		VSyncCheckBox->SetIsChecked(CurrentSettings.bVSync);
	}

	if (FOVSlider)
	{
		FOVSlider->SetValue(CurrentSettings.FieldOfView);
	}

	// Update audio UI
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->SetValue(CurrentSettings.AudioSettings.MasterVolume);
	}

	if (MusicVolumeSlider)
	{
		MusicVolumeSlider->SetValue(CurrentSettings.AudioSettings.MusicVolume);
	}

	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->SetValue(CurrentSettings.AudioSettings.SFXVolume);
	}

	// Update control UI
	if (InvertYCheckBox)
	{
		InvertYCheckBox->SetIsChecked(CurrentSettings.bInvertYAxis);
	}

	if (MouseSensitivitySlider)
	{
		MouseSensitivitySlider->SetValue(CurrentSettings.MouseSensitivity);
	}
}

void UWRSettingsWidget::ApplyGraphicsSettings()
{
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->SetOverallScalabilityLevel(CurrentSettings.GraphicsQuality);
		UserSettings->SetVSyncEnabled(CurrentSettings.bVSync);
		UserSettings->SetFrameRateLimit(CurrentSettings.TargetFrameRate);
		UserSettings->ApplySettings(false);
	}
}

void UWRSettingsWidget::ApplyAudioSettings()
{
	if (UWRAudioManager* AudioManager = UWRAudioManager::GetInstance(GetWorld()))
	{
		AudioManager->SetAudioSettings(CurrentSettings.AudioSettings);
	}
}

void UWRSettingsWidget::ApplyControlSettings()
{
	// Apply control settings to player controller
	// Implementation depends on your input system
}

void UWRSettingsWidget::OnFOVChanged(float Value)
{
	CurrentSettings.FieldOfView = Value;
	if (FOVValueText)
	{
		FOVValueText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Value)));
	}
}

void UWRSettingsWidget::OnMasterVolumeChanged(float Value)
{
	CurrentSettings.AudioSettings.MasterVolume = Value;
}

void UWRSettingsWidget::OnMusicVolumeChanged(float Value)
{
	CurrentSettings.AudioSettings.MusicVolume = Value;
}

void UWRSettingsWidget::OnSFXVolumeChanged(float Value)
{
	CurrentSettings.AudioSettings.SFXVolume = Value;
}

void UWRSettingsWidget::OnVoiceVolumeChanged(float Value)
{
	CurrentSettings.AudioSettings.VoiceVolume = Value;
}

void UWRSettingsWidget::OnEngineVolumeChanged(float Value)
{
	CurrentSettings.AudioSettings.EngineVolume = Value;
}

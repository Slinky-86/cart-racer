#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "WRAudioManager.generated.h"

UENUM(BlueprintType)
enum class EAudioCategory : uint8
{
	Master,
	Music,
	SFX,
	Voice,
	Engine,
	Weapon,
	Ambient,
	UI
};

USTRUCT(BlueprintType)
struct FWRGameAudioSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MasterVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MusicVolume = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VoiceVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EngineVolume = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AmbientVolume = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UIVolume = 0.8f;
};

UCLASS()
class WASTELANDRACERS_API UWRAudioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static UWRAudioManager* GetInstance(const UObject* WorldContext);

	// Music System
	UFUNCTION(BlueprintCallable, Category = "Audio|Music")
	void PlayMusic(class USoundBase* MusicTrack, float FadeInTime = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio|Music")
	void StopMusic(float FadeOutTime = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio|Music")
	void CrossfadeMusic(class USoundBase* NewMusicTrack, float CrossfadeTime = 3.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio|Music")
	void SetMusicVolume(float Volume);

	// Sound Effects
	UFUNCTION(BlueprintCallable, Category = "Audio|SFX")
	class UAudioComponent* PlaySoundEffect(class USoundBase* Sound, FVector Location = FVector::ZeroVector, 
		EAudioCategory Category = EAudioCategory::SFX, float VolumeMultiplier = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio|SFX")
	class UAudioComponent* PlaySoundEffect2D(class USoundBase* Sound, 
		EAudioCategory Category = EAudioCategory::SFX, float VolumeMultiplier = 1.0f);

	// Engine Sounds
	UFUNCTION(BlueprintCallable, Category = "Audio|Engine")
	class UAudioComponent* PlayEngineSound(class USoundBase* EngineSound, class AActor* Vehicle);

	UFUNCTION(BlueprintCallable, Category = "Audio|Engine")
	void UpdateEngineSound(class UAudioComponent* EngineAudio, float RPM, float Speed);

	// Voice Lines
	UFUNCTION(BlueprintCallable, Category = "Audio|Voice")
	void PlayVoiceLine(class USoundBase* VoiceLine, class AActor* Speaker = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Audio|Voice")
	void StopVoiceLine();

	// Weapon Sounds
	UFUNCTION(BlueprintCallable, Category = "Audio|Weapon")
	void PlayWeaponSound(class USoundBase* WeaponSound, FVector Location, float VolumeMultiplier = 1.0f);

	// Ambient Sounds
	UFUNCTION(BlueprintCallable, Category = "Audio|Ambient")
	void PlayAmbientSound(class USoundBase* AmbientSound);

	UFUNCTION(BlueprintCallable, Category = "Audio|Ambient")
	void StopAmbientSound();

	// Volume Control
	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void SetCategoryVolume(EAudioCategory Category, float Volume);

	UFUNCTION(BlueprintPure, Category = "Audio|Settings")
	float GetCategoryVolume(EAudioCategory Category) const;

	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void SetAudioSettings(const FWRGameAudioSettings& Settings);

	UFUNCTION(BlueprintPure, Category = "Audio|Settings")
	FWRGameAudioSettings GetAudioSettings() const { return AudioSettings; }

	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void SaveAudioSettings();

	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void LoadAudioSettings();

	// 3D Audio
	UFUNCTION(BlueprintCallable, Category = "Audio|3D")
	void SetListenerLocation(FVector Location, FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = "Audio|3D")
	void UpdateAudioListener(class APawn* ListenerPawn);

protected:
	UPROPERTY()
	FWRGameAudioSettings AudioSettings;

	UPROPERTY()
	class UAudioComponent* CurrentMusicComponent;

	UPROPERTY()
	class UAudioComponent* CurrentAmbientComponent;

	UPROPERTY()
	class UAudioComponent* CurrentVoiceComponent;

	UPROPERTY()
	TArray<class UAudioComponent*> ActiveAudioComponents;

	// Audio pools for performance
	UPROPERTY()
	TArray<class UAudioComponent*> AudioComponentPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	int32 MaxPoolSize = 50;

private:
	FTimerHandle MusicFadeTimer;
	float MusicFadeTarget = 1.0f;
	float MusicFadeSpeed = 1.0f;

	class UAudioComponent* GetPooledAudioComponent();
	void ReturnAudioComponentToPool(class UAudioComponent* AudioComponent);
	void UpdateMusicFade();
	float CalculateFinalVolume(EAudioCategory Category, float VolumeMultiplier) const;
	void CleanupFinishedAudioComponents();
};

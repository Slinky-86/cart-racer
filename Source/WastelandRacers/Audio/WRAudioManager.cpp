#include "WRAudioManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

void UWRAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Initialize audio settings with defaults
	AudioSettings = FWRGameAudioSettings();
	
	// Load saved audio settings
	LoadAudioSettings();
	
	UE_LOG(LogTemp, Warning, TEXT("WRAudioManager initialized"));
}

void UWRAudioManager::Deinitialize()
{
	// Save audio settings
	SaveAudioSettings();
	
	// Cleanup audio components
	for (UAudioComponent* AudioComp : ActiveAudioComponents)
	{
		if (IsValid(AudioComp))
		{
			AudioComp->Stop();
			AudioComp->DestroyComponent();
		}
	}
	ActiveAudioComponents.Empty();
	
	for (UAudioComponent* AudioComp : AudioComponentPool)
	{
		if (IsValid(AudioComp))
		{
			AudioComp->DestroyComponent();
		}
	}
	AudioComponentPool.Empty();
	
	Super::Deinitialize();
}

UWRAudioManager* UWRAudioManager::GetInstance(const UObject* WorldContext)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UWRAudioManager>();
		}
	}
	return nullptr;
}

void UWRAudioManager::PlayMusic(USoundBase* MusicTrack, float FadeInTime)
{
	if (!MusicTrack)
		return;

	// Stop current music if playing
	if (CurrentMusicComponent && CurrentMusicComponent->IsPlaying())
	{
		StopMusic(FadeInTime * 0.5f);
	}

	// Create new music component
	if (UWorld* World = GetWorld())
	{
		CurrentMusicComponent = UGameplayStatics::SpawnSound2D(World, MusicTrack, 0.0f, 1.0f, 0.0f, nullptr, true, false);
		if (CurrentMusicComponent)
		{
			CurrentMusicComponent->SetVolumeMultiplier(0.0f);
			CurrentMusicComponent->Play();

			// Fade in
			if (FadeInTime > 0.0f)
			{
				MusicFadeTarget = CalculateFinalVolume(EAudioCategory::Music, 1.0f);
				MusicFadeSpeed = MusicFadeTarget / FadeInTime;
				GetWorld()->GetTimerManager().SetTimer(MusicFadeTimer, this, &UWRAudioManager::UpdateMusicFade, 0.1f, true);
			}
			else
			{
				CurrentMusicComponent->SetVolumeMultiplier(CalculateFinalVolume(EAudioCategory::Music, 1.0f));
			}
		}
	}
}

void UWRAudioManager::StopMusic(float FadeOutTime)
{
	if (!CurrentMusicComponent || !CurrentMusicComponent->IsPlaying())
		return;

	if (FadeOutTime > 0.0f)
	{
		MusicFadeTarget = 0.0f;
		MusicFadeSpeed = CurrentMusicComponent->VolumeMultiplier / FadeOutTime;
		GetWorld()->GetTimerManager().SetTimer(MusicFadeTimer, this, &UWRAudioManager::UpdateMusicFade, 0.1f, true);
	}
	else
	{
		CurrentMusicComponent->Stop();
		CurrentMusicComponent = nullptr;
	}
}

void UWRAudioManager::CrossfadeMusic(USoundBase* NewMusicTrack, float CrossfadeTime)
{
	if (!NewMusicTrack)
		return;

	// Start new music at 0 volume
	if (UWorld* World = GetWorld())
	{
		UAudioComponent* NewMusicComponent = UGameplayStatics::SpawnSound2D(World, NewMusicTrack, 0.0f, 1.0f, 0.0f, nullptr, true, false);
		if (NewMusicComponent)
		{
			NewMusicComponent->SetVolumeMultiplier(0.0f);
			NewMusicComponent->Play();

			// Fade out old music
			if (CurrentMusicComponent && CurrentMusicComponent->IsPlaying())
			{
				StopMusic(CrossfadeTime);
			}

			// Fade in new music
			CurrentMusicComponent = NewMusicComponent;
			MusicFadeTarget = CalculateFinalVolume(EAudioCategory::Music, 1.0f);
			MusicFadeSpeed = MusicFadeTarget / CrossfadeTime;
			GetWorld()->GetTimerManager().SetTimer(MusicFadeTimer, this, &UWRAudioManager::UpdateMusicFade, 0.1f, true);
		}
	}
}

void UWRAudioManager::SetMusicVolume(float Volume)
{
	AudioSettings.MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	
	if (CurrentMusicComponent)
	{
		CurrentMusicComponent->SetVolumeMultiplier(CalculateFinalVolume(EAudioCategory::Music, 1.0f));
	}
}

UAudioComponent* UWRAudioManager::PlaySoundEffect(USoundBase* Sound, FVector Location, EAudioCategory Category, float VolumeMultiplier)
{
	if (!Sound)
		return nullptr;

	UAudioComponent* AudioComp = GetPooledAudioComponent();
	if (AudioComp)
	{
		AudioComp->SetSound(Sound);
		AudioComp->SetWorldLocation(Location);
		AudioComp->SetVolumeMultiplier(CalculateFinalVolume(Category, VolumeMultiplier));
		AudioComp->Play();
		
		ActiveAudioComponents.Add(AudioComp);
		
		// Set timer to return to pool when finished
		FTimerHandle CleanupTimer;
		GetWorld()->GetTimerManager().SetTimer(CleanupTimer, [this]() {
			CleanupFinishedAudioComponents();
		}, Sound->GetDuration() + 0.1f, false);
	}

	return AudioComp;
}

UAudioComponent* UWRAudioManager::PlaySoundEffect2D(USoundBase* Sound, EAudioCategory Category, float VolumeMultiplier)
{
	if (!Sound)
		return nullptr;

	if (UWorld* World = GetWorld())
	{
		UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(World, Sound, 
			CalculateFinalVolume(Category, VolumeMultiplier), 1.0f, 0.0f, nullptr, false, true);
		
		if (AudioComp)
		{
			ActiveAudioComponents.Add(AudioComp);
		}
		
		return AudioComp;
	}

	return nullptr;
}

UAudioComponent* UWRAudioManager::PlayEngineSound(USoundBase* EngineSound, AActor* Vehicle)
{
	if (!EngineSound || !Vehicle)
		return nullptr;

	UAudioComponent* EngineAudio = GetPooledAudioComponent();
	if (EngineAudio)
	{
		EngineAudio->SetSound(EngineSound);
		EngineAudio->AttachToComponent(Vehicle->GetRootComponent(), 
			FAttachmentTransformRules::SnapToTargetIncludingScale);
		EngineAudio->SetVolumeMultiplier(CalculateFinalVolume(EAudioCategory::Engine, 1.0f));
		EngineAudio->bAutoDestroy = false;
		EngineAudio->Play();
		
		ActiveAudioComponents.Add(EngineAudio);
	}

	return EngineAudio;
}

void UWRAudioManager::UpdateEngineSound(UAudioComponent* EngineAudio, float RPM, float Speed)
{
	if (!EngineAudio || !EngineAudio->IsPlaying())
		return;

	// Calculate pitch based on RPM (typical range 800-6000 RPM)
	float NormalizedRPM = FMath::Clamp((RPM - 800.0f) / 5200.0f, 0.0f, 1.0f);
	float Pitch = FMath::Lerp(0.8f, 2.0f, NormalizedRPM);
	EngineAudio->SetPitchMultiplier(Pitch);

	// Calculate volume based on speed
	float NormalizedSpeed = FMath::Clamp(Speed / 1200.0f, 0.3f, 1.0f);
	float Volume = CalculateFinalVolume(EAudioCategory::Engine, NormalizedSpeed);
	EngineAudio->SetVolumeMultiplier(Volume);
}

void UWRAudioManager::PlayVoiceLine(USoundBase* VoiceLine, AActor* Speaker)
{
	if (!VoiceLine)
		return;

	// Stop current voice line
	StopVoiceLine();

	if (Speaker)
	{
		CurrentVoiceComponent = PlaySoundEffect(VoiceLine, Speaker->GetActorLocation(), EAudioCategory::Voice);
	}
	else
	{
		CurrentVoiceComponent = PlaySoundEffect2D(VoiceLine, EAudioCategory::Voice);
	}
}

void UWRAudioManager::StopVoiceLine()
{
	if (CurrentVoiceComponent && CurrentVoiceComponent->IsPlaying())
	{
		CurrentVoiceComponent->Stop();
		CurrentVoiceComponent = nullptr;
	}
}

void UWRAudioManager::PlayWeaponSound(USoundBase* WeaponSound, FVector Location, float VolumeMultiplier)
{
	PlaySoundEffect(WeaponSound, Location, EAudioCategory::Weapon, VolumeMultiplier);
}

void UWRAudioManager::PlayAmbientSound(USoundBase* AmbientSound)
{
	if (!AmbientSound)
		return;

	// Stop current ambient sound
	StopAmbientSound();

	if (UWorld* World = GetWorld())
	{
		CurrentAmbientComponent = UGameplayStatics::SpawnSound2D(World, AmbientSound, 
			CalculateFinalVolume(EAudioCategory::Ambient, 1.0f), 1.0f, 0.0f, nullptr, true, false);
		
		if (CurrentAmbientComponent)
		{
			CurrentAmbientComponent->Play();
		}
	}
}

void UWRAudioManager::StopAmbientSound()
{
	if (CurrentAmbientComponent && CurrentAmbientComponent->IsPlaying())
	{
		CurrentAmbientComponent->Stop();
		CurrentAmbientComponent = nullptr;
	}
}

void UWRAudioManager::SetCategoryVolume(EAudioCategory Category, float Volume)
{
	Volume = FMath::Clamp(Volume, 0.0f, 1.0f);

	switch (Category)
	{
		case EAudioCategory::Master:
			AudioSettings.MasterVolume = Volume;
			break;
		case EAudioCategory::Music:
			AudioSettings.MusicVolume = Volume;
			if (CurrentMusicComponent)
			{
				CurrentMusicComponent->SetVolumeMultiplier(CalculateFinalVolume(EAudioCategory::Music, 1.0f));
			}
			break;
		case EAudioCategory::SFX:
			AudioSettings.SFXVolume = Volume;
			break;
		case EAudioCategory::Voice:
			AudioSettings.VoiceVolume = Volume;
			break;
		case EAudioCategory::Engine:
			AudioSettings.EngineVolume = Volume;
			break;
		case EAudioCategory::Weapon:
			AudioSettings.WeaponVolume = Volume;
			break;
		case EAudioCategory::Ambient:
			AudioSettings.AmbientVolume = Volume;
			if (CurrentAmbientComponent)
			{
				CurrentAmbientComponent->SetVolumeMultiplier(CalculateFinalVolume(EAudioCategory::Ambient, 1.0f));
			}
			break;
		case EAudioCategory::UI:
			AudioSettings.UIVolume = Volume;
			break;
	}
}

float UWRAudioManager::GetCategoryVolume(EAudioCategory Category) const
{
	switch (Category)
	{
		case EAudioCategory::Master: return AudioSettings.MasterVolume;
		case EAudioCategory::Music: return AudioSettings.MusicVolume;
		case EAudioCategory::SFX: return AudioSettings.SFXVolume;
		case EAudioCategory::Voice: return AudioSettings.VoiceVolume;
		case EAudioCategory::Engine: return AudioSettings.EngineVolume;
		case EAudioCategory::Weapon: return AudioSettings.WeaponVolume;
		case EAudioCategory::Ambient: return AudioSettings.AmbientVolume;
		case EAudioCategory::UI: return AudioSettings.UIVolume;
		default: return 1.0f;
	}
}

void UWRAudioManager::SetAudioSettings(const FWRGameAudioSettings& Settings)
{
	AudioSettings = Settings;
	
	// Update currently playing audio
	if (CurrentMusicComponent)
	{
		CurrentMusicComponent->SetVolumeMultiplier(CalculateFinalVolume(EAudioCategory::Music, 1.0f));
	}
	
	if (CurrentAmbientComponent)
	{
		CurrentAmbientComponent->SetVolumeMultiplier(CalculateFinalVolume(EAudioCategory::Ambient, 1.0f));
	}
}

void UWRAudioManager::SaveAudioSettings()
{
	// Save to game user settings or config file
	// Implementation depends on your save system
	UE_LOG(LogTemp, Warning, TEXT("Audio settings saved"));
}

void UWRAudioManager::LoadAudioSettings()
{
	// Load from game user settings or config file
	// Implementation depends on your save system
	UE_LOG(LogTemp, Warning, TEXT("Audio settings loaded"));
}

void UWRAudioManager::SetListenerLocation(FVector Location, FRotator Rotation)
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalListenerFocusParameters(World,
	1.f, 1.f,   // Azimuth focus / nonfocus
	1.f, 1.f,   // Distance focus / nonfocus
	1.f, 1.f,   // Volume focus / nonfocus
	1.f, 1.f);  // Priority focus / nonfocus
	}
}

void UWRAudioManager::UpdateAudioListener(APawn* ListenerPawn)
{
	if (ListenerPawn)
	{
		SetListenerLocation(ListenerPawn->GetActorLocation(), ListenerPawn->GetActorRotation());
	}
}

UAudioComponent* UWRAudioManager::GetPooledAudioComponent()
{
	// Try to find an available component in the pool
	for (int32 i = AudioComponentPool.Num() - 1; i >= 0; i--)
	{
		UAudioComponent* AudioComp = AudioComponentPool[i];
		if (IsValid(AudioComp) && !AudioComp->IsPlaying())
		{
			AudioComponentPool.RemoveAt(i);
			return AudioComp;
		}
	}

	// Create new component if pool is empty or all are in use
	if (UWorld* World = GetWorld())
	{
		UAudioComponent* NewAudioComp = NewObject<UAudioComponent>(World);
		if (NewAudioComp)
		{
			NewAudioComp->bAutoDestroy = false;
			return NewAudioComp;
		}
	}

	return nullptr;
}

void UWRAudioManager::ReturnAudioComponentToPool(UAudioComponent* AudioComponent)
{
	if (!AudioComponent || AudioComponentPool.Num() >= MaxPoolSize)
	{
		if (AudioComponent)
		{
			AudioComponent->DestroyComponent();
		}
		return;
	}

	AudioComponent->Stop();
	AudioComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	AudioComponentPool.Add(AudioComponent);
}

void UWRAudioManager::UpdateMusicFade()
{
	if (!CurrentMusicComponent)
	{
		GetWorld()->GetTimerManager().ClearTimer(MusicFadeTimer);
		return;
	}

	float CurrentVolume = CurrentMusicComponent->VolumeMultiplier;
	float NewVolume;

	if (MusicFadeTarget > CurrentVolume)
	{
		NewVolume = FMath::Min(CurrentVolume + (MusicFadeSpeed * 0.1f), MusicFadeTarget);
	}
	else
	{
		NewVolume = FMath::Max(CurrentVolume - (MusicFadeSpeed * 0.1f), MusicFadeTarget);
	}

	CurrentMusicComponent->SetVolumeMultiplier(NewVolume);

	// Check if fade is complete
	if (FMath::IsNearlyEqual(NewVolume, MusicFadeTarget, 0.01f))
	{
		GetWorld()->GetTimerManager().ClearTimer(MusicFadeTimer);
		
		if (MusicFadeTarget == 0.0f)
		{
			CurrentMusicComponent->Stop();
			CurrentMusicComponent = nullptr;
		}
	}
}

float UWRAudioManager::CalculateFinalVolume(EAudioCategory Category, float VolumeMultiplier) const
{
	float CategoryVolume = GetCategoryVolume(Category);
	return AudioSettings.MasterVolume * CategoryVolume * VolumeMultiplier;
}

void UWRAudioManager::CleanupFinishedAudioComponents()
{
	for (int32 i = ActiveAudioComponents.Num() - 1; i >= 0; i--)
	{
		UAudioComponent* AudioComp = ActiveAudioComponents[i];
		if (!IsValid(AudioComp) || !AudioComp->IsPlaying())
		{
			ActiveAudioComponents.RemoveAt(i);
			if (IsValid(AudioComp))
			{
				ReturnAudioComponentToPool(AudioComp);
			}
		}
	}
}

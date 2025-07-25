#include "WRCharacterData.h"
#include "WastelandRacers/Audio/WRAudioManager.h"
#include "Sound/SoundBase.h"
#include "Engine/Engine.h"

USoundBase* UWRCharacterData::GetRandomVoiceLine(const TArray<USoundBase*>& VoiceLineArray) const
{
	if (VoiceLineArray.Num() == 0)
		return nullptr;

	int32 RandomIndex = FMath::RandRange(0, VoiceLineArray.Num() - 1);
	return VoiceLineArray[RandomIndex];
}

void UWRCharacterData::PlayVoiceLine(const TArray<USoundBase*>& VoiceLineArray, AActor* Speaker) const
{
	USoundBase* VoiceLine = GetRandomVoiceLine(VoiceLineArray);
	if (!VoiceLine)
		return;

	if (UWRAudioManager* AudioManager = UWRAudioManager::GetInstance(this))
	{
		AudioManager->PlayVoiceLine(VoiceLine, Speaker);
	}
}

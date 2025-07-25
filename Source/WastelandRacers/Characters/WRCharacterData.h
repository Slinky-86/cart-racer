#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WRCharacterData.generated.h"

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Axton,
	Maya,
	Salvador,
	Zer0,
	Gaige,
	Krieg,
	TinyTina,
	Mordecai
};

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccelerationBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HandlingBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponDamageBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefenseBonus = 0.0f;
};

USTRUCT(BlueprintType)
struct FCharacterVoiceLines
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> RaceStartLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> WeaponFireLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> HitByWeaponLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> PowerUpCollectedLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> OvertakeLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> WinLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> LoseLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> TauntLines;
};

UCLASS(BlueprintType)
class WASTELANDRACERS_API UWRCharacterData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	ECharacterType CharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString CharacterDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	class UTexture2D* CharacterPortrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	class USkeletalMesh* CharacterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TSubclassOf<class UAnimInstance> CharacterAnimBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FCharacterStats CharacterStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FCharacterVoiceLines VoiceLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
	bool bIsUnlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
	int32 UnlockRequirement = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
	FString UnlockDescription;

	UFUNCTION(BlueprintPure, Category = "Character")
	class USoundBase* GetRandomVoiceLine(const TArray<class USoundBase*>& VoiceLineArray) const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void PlayVoiceLine(const TArray<class USoundBase*>& VoiceLineArray, class AActor* Speaker = nullptr) const;
};

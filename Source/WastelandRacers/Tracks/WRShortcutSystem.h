#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "WastelandRacers/Tracks/WRTrackManager.h"
#include "WRShortcutSystem.generated.h"

UENUM(BlueprintType)
enum class EShortcutType : uint8
{
	Hidden,          // Completely hidden, requires discovery
	Risky,           // Visible but dangerous (narrow, hazards)
	Skill,           // Requires precise driving or timing
	Jump,            // Requires speed boost or jump
	Underground,     // Tunnel or cave system
	Elevated,        // Bridge or elevated path
	Environmental    // Uses environment features (pipes, structures)
};

UENUM(BlueprintType)
enum class EShortcutDifficulty : uint8
{
	Easy,           // Minor risk, small time save
	Medium,         // Moderate risk, decent time save
	Hard,           // High risk, significant time save
	Expert          // Very high risk, major time save
};

USTRUCT(BlueprintType)
struct FShortcutData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EShortcutType ShortcutType = EShortcutType::Hidden;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EShortcutDifficulty Difficulty = EShortcutDifficulty::Easy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ShortcutName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeSaveSeconds = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RequiredSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequiresBoost = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasHazards = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> WaypointLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector EntryPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ExitPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDiscovered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsageCount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShortcutDiscovered, class AWRKart*, Kart, const FShortcutData&, ShortcutData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShortcutUsed, class AWRKart*, Kart, const FShortcutData&, ShortcutData);

UCLASS()
class WASTELANDRACERS_API AWRShortcutSystem : public AActor
{
	GENERATED_BODY()

public:
	AWRShortcutSystem();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Shortcuts")
	void RegisterShortcut(const FShortcutData& ShortcutData);

	UFUNCTION(BlueprintCallable, Category = "Shortcuts")
	void DiscoverShortcut(int32 ShortcutIndex, class AWRKart* DiscoveringKart);

	UFUNCTION(BlueprintCallable, Category = "Shortcuts")
	TArray<FShortcutData> GetAvailableShortcuts(FVector KartLocation, float SearchRadius = 500.0f);

	UFUNCTION(BlueprintCallable, Category = "Shortcuts")
	bool IsShortcutAccessible(int32 ShortcutIndex, class AWRKart* Kart);

	UFUNCTION(BlueprintCallable, Category = "Shortcuts")
	void CreateShortcutForTrack(ETrackType TrackType);

	UFUNCTION(BlueprintPure, Category = "Shortcuts")
	int32 GetTotalShortcuts() const { return Shortcuts.Num(); }

	UFUNCTION(BlueprintPure, Category = "Shortcuts")
	int32 GetDiscoveredShortcuts() const;

	UPROPERTY(BlueprintAssignable)
	FOnShortcutDiscovered OnShortcutDiscovered;

	UPROPERTY(BlueprintAssignable)
	FOnShortcutUsed OnShortcutUsed;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcuts")
	TArray<FShortcutData> Shortcuts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcuts")
	TArray<class AWRShortcutTrigger*> ShortcutTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	class UStaticMesh* HiddenPathMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	class UMaterialInterface* ShortcutMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* DiscoveryEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* DiscoverySound;

private:
	void CreatePandoraShortcuts();
	void CreateOpportunityShortcuts();
	void CreateEridiumShortcuts();
	void CreateWildlifeShortcuts();
	void CreateHyperionShortcuts();

	void SpawnShortcutTriggers();
	void UpdateShortcutVisibility();
};

UCLASS()
class WASTELANDRACERS_API AWRShortcutTrigger : public AActor
{
	GENERATED_BODY()

public:
	AWRShortcutTrigger();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Shortcut")
	void SetShortcutData(const FShortcutData& Data, int32 Index);

	UFUNCTION(BlueprintPure, Category = "Shortcut")
	bool IsShortcutActive() const { return bIsActive; }

	UFUNCTION(BlueprintCallable, Category = "Shortcut")
	void SetShortcutActive(bool bActive);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineComponent* ShortcutPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* VisualEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcut")
	FShortcutData ShortcutData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcut")
	int32 ShortcutIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcut")
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcut")
	bool bShowVisualHints = true;

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateVisualState();
	void PlayDiscoveryEffects();
};

UCLASS()
class WASTELANDRACERS_API AWRShortcutGate : public AActor
{
	GENERATED_BODY()

public:
	AWRShortcutGate();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Gate")
	void OpenGate();

	UFUNCTION(BlueprintCallable, Category = "Gate")
	void CloseGate();

	UFUNCTION(BlueprintCallable, Category = "Gate")
	void SetRequiredSpeed(float Speed) { RequiredSpeed = Speed; }

	UFUNCTION(BlueprintCallable, Category = "Gate")
	void SetRequiresBoost(bool bRequires) { bRequiresBoost = bRequires; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* GateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* SpeedCheckTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	float RequiredSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	bool bRequiresBoost = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	bool bIsOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GateOpenTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* OpenEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* OpenSound;

private:
	FVector ClosedPosition;
	FVector OpenPosition;
	bool bIsAnimating = false;
	float AnimationTime = 0.0f;

	UFUNCTION()
	void OnSpeedCheckBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void UpdateGateAnimation(float DeltaTime);
	bool CheckKartRequirements(class AWRKart* Kart);
};

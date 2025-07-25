#include "WRTrackHazard.h"
#include "WastelandRacers.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "EngineUtils.h"
#include "EngineUtils.h"

AWRTrackHazard::AWRTrackHazard()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create hazard trigger
	HazardTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("HazardTrigger"));
	HazardTrigger->SetSphereRadius(150.0f);
	HazardTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HazardTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	HazardTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HazardTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	RootComponent = HazardTrigger;

	// Create hazard mesh
	HazardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HazardMesh"));
	HazardMesh->SetupAttachment(RootComponent);
	HazardMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create hazard effect
	HazardEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HazardEffect"));
	HazardEffect->SetupAttachment(RootComponent);

	// Create hazard audio
	HazardAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HazardAudio"));
	HazardAudio->SetupAttachment(RootComponent);
	HazardAudio->bAutoActivate = false;

	// Bind overlap events
	HazardTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWRTrackHazard::OnHazardBeginOverlap);
	HazardTrigger->OnComponentEndOverlap.AddDynamic(this, &AWRTrackHazard::OnHazardEndOverlap);
}

void AWRTrackHazard::BeginPlay()
{
	Super::BeginPlay();
	SetupHazardAppearance();
}

void AWRTrackHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle continuous damage for affected karts
	if (bIsContinuous && bIsActive)
	{
		for (AWRKart* Kart : AffectedKarts)
		{
			if (IsValid(Kart))
			{
				ApplyHazardEffect(Kart);
			}
		}
	}
}

void AWRTrackHazard::SetHazardType(EHazardType Type)
{
	HazardType = Type;
	SetupHazardAppearance();
}

void AWRTrackHazard::ActivateHazard()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastActivationTime < ActivationCooldown)
		return;

	bIsActive = true;
	LastActivationTime = CurrentTime;

	// Play activation effects
	if (ActivationEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), ActivationEffect, GetActorLocation(), GetActorRotation());
	}

	if (ActivationSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivationSound, GetActorLocation());
	}

	// Start hazard effect
	if (HazardEffect)
	{
		HazardEffect->Activate();
	}

	if (HazardAudio)
	{
		HazardAudio->Play();
	}

	UE_LOG(LogTemp, Warning, TEXT("Hazard activated: %d"), (int32)HazardType);
}

void AWRTrackHazard::DeactivateHazard()
{
	bIsActive = false;

	// Stop hazard effect
	if (HazardEffect)
	{
		HazardEffect->Deactivate();
	}

	if (HazardAudio)
	{
		HazardAudio->Stop();
	}

	// Remove effects from all affected karts
	for (AWRKart* Kart : AffectedKarts)
	{
		if (IsValid(Kart))
		{
			RemoveHazardEffect(Kart);
		}
	}
	AffectedKarts.Empty();
}

void AWRTrackHazard::OnHazardBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActive)
		return;

	AWRKart* Kart = Cast<AWRKart>(OtherActor);
	if (!Kart)
		return;

	AffectedKarts.Add(Kart);
	ApplyHazardEffect(Kart);

	UE_LOG(LogTemp, Warning, TEXT("Kart %s entered hazard: %d"), *Kart->GetName(), (int32)HazardType);
}

void AWRTrackHazard::OnHazardEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWRKart* Kart = Cast<AWRKart>(OtherActor);
	if (!Kart)
		return;

	AffectedKarts.Remove(Kart);
	RemoveHazardEffect(Kart);

	UE_LOG(LogTemp, Warning, TEXT("Kart %s exited hazard: %d"), *Kart->GetName(), (int32)HazardType);
}

void AWRTrackHazard::ApplyHazardEffect(AWRKart* Kart)
{
	if (!Kart || !bIsActive)
		return;

	switch (HazardType)
	{
		case EHazardType::AcidPool:
			// Apply acid damage and corrosion effect
			// Kart->TakeDamage(Damage);
			// Kart->ApplySpeedMultiplier(SlowdownFactor);
			break;

		case EHazardType::ExplosiveBarrel:
			// Explosive barrels cause instant damage and knockback
			// Kart->TakeDamage(Damage * 2.0f);
			// Kart->ApplyKnockback(GetActorLocation(), 2000.0f);
			DeactivateHazard(); // One-time use
			break;

		case EHazardType::ElectricFence:
			// Electric damage and temporary stun
			// Kart->TakeDamage(Damage);
			// Kart->ApplyStun(1.0f);
			break;

		case EHazardType::OilSlick:
			// Reduce traction and handling
			// Kart->ApplyTractionMultiplier(0.3f);
			break;

		case EHazardType::SteamVent:
			// Obscure vision and slight damage
			// Kart->TakeDamage(Damage * 0.5f);
			// Kart->ApplyVisionObscure(EffectDuration);
			break;

		case EHazardType::LaserGrid:
			// High damage but predictable pattern
			// Kart->TakeDamage(Damage * 1.5f);
			break;

		case EHazardType::SpikeTrap:
			// Puncture tires, reduce speed
			// Kart->TakeDamage(Damage);
			// Kart->ApplySpeedMultiplier(SlowdownFactor);
			break;

		case EHazardType::ToxicGas:
			// Continuous damage over time
			// Kart->ApplyDamageOverTime(Damage, EffectDuration);
			break;
	}

	// Play damage sound
	if (DamageSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, Kart->GetActorLocation());
	}
}

void AWRTrackHazard::RemoveHazardEffect(AWRKart* Kart)
{
	if (!Kart)
		return;

	// Remove ongoing effects based on hazard type
	switch (HazardType)
	{
		case EHazardType::AcidPool:
			// Kart->RemoveSpeedMultiplier();
			break;

		case EHazardType::OilSlick:
			// Kart->RemoveTractionMultiplier();
			break;

		case EHazardType::SpikeTrap:
			// Kart->RemoveSpeedMultiplier();
			break;

		default:
			// Most hazards don't have persistent effects
			break;
	}
}

void AWRTrackHazard::SetupHazardAppearance()
{
	// Setup hazard-specific appearance and properties
	switch (HazardType)
	{
		case EHazardType::AcidPool:
			Damage = 15.0f;
			SlowdownFactor = 0.7f;
			bIsContinuous = true;
			HazardTrigger->SetSphereRadius(200.0f);
			break;

		case EHazardType::ExplosiveBarrel:
			Damage = 50.0f;
			bIsContinuous = false;
			HazardTrigger->SetSphereRadius(100.0f);
			break;

		case EHazardType::ElectricFence:
			Damage = 30.0f;
			bIsContinuous = true;
			HazardTrigger->SetSphereRadius(80.0f);
			break;

		case EHazardType::OilSlick:
			Damage = 0.0f;
			SlowdownFactor = 0.4f;
			bIsContinuous = true;
			HazardTrigger->SetSphereRadius(250.0f);
			break;

		case EHazardType::SteamVent:
			Damage = 10.0f;
			bIsContinuous = true;
			ActivationCooldown = 8.0f;
			HazardTrigger->SetSphereRadius(150.0f);
			break;

		case EHazardType::LaserGrid:
			Damage = 40.0f;
			bIsContinuous = false;
			ActivationCooldown = 3.0f;
			HazardTrigger->SetSphereRadius(120.0f);
			break;

		case EHazardType::SpikeTrap:
			Damage = 25.0f;
			SlowdownFactor = 0.6f;
			bIsContinuous = true;
			HazardTrigger->SetSphereRadius(100.0f);
			break;

		case EHazardType::ToxicGas:
			Damage = 8.0f;
			EffectDuration = 5.0f;
			bIsContinuous = true;
			HazardTrigger->SetSphereRadius(300.0f);
			break;
	}
}

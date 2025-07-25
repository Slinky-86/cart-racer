#include "WRShortcutSystem.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/Tracks/WRTrackManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

AWRShortcutSystem::AWRShortcutSystem()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
}

void AWRShortcutSystem::BeginPlay()
{
	Super::BeginPlay();
	SpawnShortcutTriggers();
}

void AWRShortcutSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateShortcutVisibility();
}

void AWRShortcutSystem::RegisterShortcut(const FShortcutData& ShortcutData)
{
	Shortcuts.Add(ShortcutData);
	UE_LOG(LogTemp, Warning, TEXT("Registered shortcut: %s"), *ShortcutData.ShortcutName);
}

void AWRShortcutSystem::DiscoverShortcut(int32 ShortcutIndex, AWRKart* DiscoveringKart)
{
	if (ShortcutIndex >= 0 && ShortcutIndex < Shortcuts.Num())
	{
		FShortcutData& Shortcut = Shortcuts[ShortcutIndex];
		if (!Shortcut.bIsDiscovered)
		{
			Shortcut.bIsDiscovered = true;
			
			// Play discovery effects
			if (DiscoveryEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(), DiscoveryEffect, Shortcut.EntryPoint, FRotator::ZeroRotator);
			}

			if (DiscoverySound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), DiscoverySound, Shortcut.EntryPoint);
			}

			OnShortcutDiscovered.Broadcast(DiscoveringKart, Shortcut);
			UE_LOG(LogTemp, Warning, TEXT("Shortcut discovered: %s by %s"), 
				*Shortcut.ShortcutName, *DiscoveringKart->GetName());
		}

		Shortcut.UsageCount++;
		OnShortcutUsed.Broadcast(DiscoveringKart, Shortcut);
	}
}

TArray<FShortcutData> AWRShortcutSystem::GetAvailableShortcuts(FVector KartLocation, float SearchRadius)
{
	TArray<FShortcutData> AvailableShortcuts;

	for (const FShortcutData& Shortcut : Shortcuts)
	{
		float Distance = FVector::Dist(KartLocation, Shortcut.EntryPoint);
		if (Distance <= SearchRadius && Shortcut.bIsDiscovered)
		{
			AvailableShortcuts.Add(Shortcut);
		}
	}

	return AvailableShortcuts;
}

bool AWRShortcutSystem::IsShortcutAccessible(int32 ShortcutIndex, AWRKart* Kart)
{
	if (ShortcutIndex < 0 || ShortcutIndex >= Shortcuts.Num() || !Kart)
		return false;

	const FShortcutData& Shortcut = Shortcuts[ShortcutIndex];

	// Check speed requirement
	if (Shortcut.RequiredSpeed > 0.0f && Kart->GetCurrentSpeed() < Shortcut.RequiredSpeed)
		return false;

	// Check boost requirement
	if (Shortcut.bRequiresBoost && !Kart->IsBoosting())
		return false;

	return true;
}

void AWRShortcutSystem::CreateShortcutForTrack(ETrackType TrackType)
{
	Shortcuts.Empty();

	switch (TrackType)
	{
		case ETrackType::PandoraDesert:
			CreatePandoraShortcuts();
			break;
		case ETrackType::OpportunityRuins:
			CreateOpportunityShortcuts();
			break;
		case ETrackType::EridiumMines:
			CreateEridiumShortcuts();
			break;
		case ETrackType::WildlifePreserve:
			CreateWildlifeShortcuts();
			break;
		case ETrackType::HyperionMoonBase:
			CreateHyperionShortcuts();
			break;
	}

	SpawnShortcutTriggers();
}

int32 AWRShortcutSystem::GetDiscoveredShortcuts() const
{
	int32 Count = 0;
	for (const FShortcutData& Shortcut : Shortcuts)
	{
		if (Shortcut.bIsDiscovered)
			Count++;
	}
	return Count;
}

void AWRShortcutSystem::CreatePandoraShortcuts()
{
	// Hidden Cave Shortcut
	FShortcutData CaveShortcut;
	CaveShortcut.ShortcutType = EShortcutType::Hidden;
	CaveShortcut.Difficulty = EShortcutDifficulty::Medium;
	CaveShortcut.ShortcutName = TEXT("Bandit Cave Tunnel");
	CaveShortcut.TimeSaveSeconds = 3.5f;
	CaveShortcut.EntryPoint = FVector(800, 600, 100);
	CaveShortcut.ExitPoint = FVector(1200, 1000, 100);
	CaveShortcut.bHasHazards = true;
	CaveShortcut.WaypointLocations = {
		FVector(850, 650, 80),
		FVector(950, 750, 60),
		FVector(1100, 900, 80)
	};
	RegisterShortcut(CaveShortcut);

	// Scrap Pile Jump
	FShortcutData JumpShortcut;
	JumpShortcut.ShortcutType = EShortcutType::Jump;
	JumpShortcut.Difficulty = EShortcutDifficulty::Hard;
	JumpShortcut.ShortcutName = TEXT("Scrap Pile Leap");
	JumpShortcut.TimeSaveSeconds = 2.8f;
	JumpShortcut.RequiredSpeed = 900.0f;
	JumpShortcut.bRequiresBoost = true;
	JumpShortcut.EntryPoint = FVector(-500, -800, 100);
	JumpShortcut.ExitPoint = FVector(-200, -600, 100);
	RegisterShortcut(JumpShortcut);

	// Narrow Canyon Path
	FShortcutData CanyonPath;
	CanyonPath.ShortcutType = EShortcutType::Risky;
	CanyonPath.Difficulty = EShortcutDifficulty::Easy;
	CanyonPath.ShortcutName = TEXT("Narrow Canyon");
	CanyonPath.TimeSaveSeconds = 1.5f;
	CanyonPath.EntryPoint = FVector(500, -1200, 100);
	CanyonPath.ExitPoint = FVector(800, -800, 100);
	CanyonPath.bHasHazards = true;
	RegisterShortcut(CanyonPath);
}

void AWRShortcutSystem::CreateOpportunityShortcuts()
{
	// Building Interior Shortcut
	FShortcutData BuildingShortcut;
	BuildingShortcut.ShortcutType = EShortcutType::Environmental;
	BuildingShortcut.Difficulty = EShortcutDifficulty::Medium;
	BuildingShortcut.ShortcutName = TEXT("Hyperion Office Complex");
	BuildingShortcut.TimeSaveSeconds = 4.2f;
	BuildingShortcut.EntryPoint = FVector(1200, 400, 200);
	BuildingShortcut.ExitPoint = FVector(800, 800, 300);
	BuildingShortcut.WaypointLocations = {
		FVector(1100, 500, 250),
		FVector(1000, 600, 275),
		FVector(900, 700, 285)
	};
	RegisterShortcut(BuildingShortcut);

	// Elevated Highway
	FShortcutData HighwayShortcut;
	HighwayShortcut.ShortcutType = EShortcutType::Elevated;
	HighwayShortcut.Difficulty = EShortcutDifficulty::Hard;
	HighwayShortcut.ShortcutName = TEXT("Collapsed Overpass");
	HighwayShortcut.TimeSaveSeconds = 3.8f;
	HighwayShortcut.RequiredSpeed = 1000.0f;
	HighwayShortcut.EntryPoint = FVector(-400, 600, 200);
	HighwayShortcut.ExitPoint = FVector(200, 1000, 400);
	HighwayShortcut.bHasHazards = true;
	RegisterShortcut(HighwayShortcut);

	// Sewer System
	FShortcutData SewerShortcut;
	SewerShortcut.ShortcutType = EShortcutType::Underground;
	SewerShortcut.Difficulty = EShortcutDifficulty::Expert;
	SewerShortcut.ShortcutName = TEXT("Corporate Sewer Network");
	SewerShortcut.TimeSaveSeconds = 5.5f;
	SewerShortcut.EntryPoint = FVector(-800, -600, 150);
	SewerShortcut.ExitPoint = FVector(-200, -200, 180);
	SewerShortcut.bHasHazards = true;
	RegisterShortcut(SewerShortcut);
}

void AWRShortcutSystem::CreateEridiumShortcuts()
{
	// Crystal Cavern
	FShortcutData CrystalCavern;
	CrystalCavern.ShortcutType = EShortcutType::Hidden;
	CrystalCavern.Difficulty = EShortcutDifficulty::Medium;
	CrystalCavern.ShortcutName = TEXT("Eridium Crystal Cavern");
	CrystalCavern.TimeSaveSeconds = 4.0f;
	CrystalCavern.EntryPoint = FVector(600, 0, 50);
	CrystalCavern.ExitPoint = FVector(200, 600, 25);
	CrystalCavern.bHasHazards = true;
	RegisterShortcut(CrystalCavern);

	// Mining Shaft Drop
	FShortcutData MiningShaft;
	MiningShaft.ShortcutType = EShortcutType::Skill;
	MiningShaft.Difficulty = EShortcutDifficulty::Expert;
	MiningShaft.ShortcutName = TEXT("Vertical Mining Shaft");
	MiningShaft.TimeSaveSeconds = 6.2f;
	MiningShaft.EntryPoint = FVector(-400, 400, 75);
	MiningShaft.ExitPoint = FVector(-600, 800, 0);
	MiningShaft.bHasHazards = true;
	RegisterShortcut(MiningShaft);
}

void AWRShortcutSystem::CreateWildlifeShortcuts()
{
	// Overgrown Greenhouse
	FShortcutData Greenhouse;
	Greenhouse.ShortcutType = EShortcutType::Environmental;
	Greenhouse.Difficulty = EShortcutDifficulty::Easy;
	Greenhouse.ShortcutName = TEXT("Abandoned Greenhouse");
	Greenhouse.TimeSaveSeconds = 2.2f;
	Greenhouse.EntryPoint = FVector(900, 0, 120);
	Greenhouse.ExitPoint = FVector(600, 700, 140);
	RegisterShortcut(Greenhouse);

	// Tree Canopy Bridge
	FShortcutData CanopyBridge;
	CanopyBridge.ShortcutType = EShortcutType::Elevated;
	CanopyBridge.Difficulty = EShortcutDifficulty::Hard;
	CanopyBridge.ShortcutName = TEXT("Canopy Bridge Network");
	CanopyBridge.TimeSaveSeconds = 3.5f;
	CanopyBridge.RequiredSpeed = 850.0f;
	CanopyBridge.EntryPoint = FVector(-600, 700, 120);
	CanopyBridge.ExitPoint = FVector(-200, 350, 160);
	CanopyBridge.bHasHazards = true;
	RegisterShortcut(CanopyBridge);

	// Underground Root System
	FShortcutData RootTunnel;
	RootTunnel.ShortcutType = EShortcutType::Underground;
	RootTunnel.Difficulty = EShortcutDifficulty::Medium;
	RootTunnel.ShortcutName = TEXT("Root Network Tunnels");
	RootTunnel.TimeSaveSeconds = 2.8f;
	RootTunnel.EntryPoint = FVector(-900, -700, 100);
	RootTunnel.ExitPoint = FVector(-600, -350, 110);
	RegisterShortcut(RootTunnel);
}

void AWRShortcutSystem::CreateHyperionShortcuts()
{
	// Gravity Tube
	FShortcutData GravityTube;
	GravityTube.ShortcutType = EShortcutType::Environmental;
	GravityTube.Difficulty = EShortcutDifficulty::Expert;
	GravityTube.ShortcutName = TEXT("Zero-G Transport Tube");
	GravityTube.TimeSaveSeconds = 7.0f;
	GravityTube.EntryPoint = FVector(1200, 0, 300);
	GravityTube.ExitPoint = FVector(600, 1400, 500);
	RegisterShortcut(GravityTube);

	// Maintenance Shaft
	FShortcutData MaintenanceShaft;
	MaintenanceShaft.ShortcutType = EShortcutType::Hidden;
	MaintenanceShaft.Difficulty = EShortcutDifficulty::Medium;
	MaintenanceShaft.ShortcutName = TEXT("Maintenance Access Shaft");
	MaintenanceShaft.TimeSaveSeconds = 3.2f;
	MaintenanceShaft.EntryPoint = FVector(-800, 900, 300);
	MaintenanceShaft.ExitPoint = FVector(-400, 450, 350);
	MaintenanceShaft.bHasHazards = true;
	RegisterShortcut(MaintenanceShaft);

	// Atmospheric Vent Jump
	FShortcutData VentJump;
	VentJump.ShortcutType = EShortcutType::Jump;
	VentJump.Difficulty = EShortcutDifficulty::Hard;
	VentJump.ShortcutName = TEXT("Atmospheric Vent Boost");
	VentJump.TimeSaveSeconds = 4.5f;
	VentJump.RequiredSpeed = 1100.0f;
	VentJump.bRequiresBoost = true;
	VentJump.EntryPoint = FVector(-1200, -900, 300);
	VentJump.ExitPoint = FVector(-600, -400, 400);
	RegisterShortcut(VentJump);
}

void AWRShortcutSystem::SpawnShortcutTriggers()
{
	// Clean up existing triggers
	for (AWRShortcutTrigger* Trigger : ShortcutTriggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->Destroy();
		}
	}
	ShortcutTriggers.Empty();

	// Spawn new triggers for each shortcut
	for (int32 i = 0; i < Shortcuts.Num(); i++)
	{
		const FShortcutData& Shortcut = Shortcuts[i];

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		AWRShortcutTrigger* Trigger = GetWorld()->SpawnActor<AWRShortcutTrigger>(
			AWRShortcutTrigger::StaticClass(), Shortcut.EntryPoint, FRotator::ZeroRotator, SpawnParams);

		if (Trigger)
		{
			Trigger->SetShortcutData(Shortcut, i);
			ShortcutTriggers.Add(Trigger);
		}
	}
}

void AWRShortcutSystem::UpdateShortcutVisibility()
{
	for (int32 i = 0; i < ShortcutTriggers.Num(); i++)
	{
		if (AWRShortcutTrigger* Trigger = ShortcutTriggers[i])
		{
			bool bShouldBeActive = Shortcuts[i].bIsDiscovered || Shortcuts[i].ShortcutType != EShortcutType::Hidden;
			Trigger->SetShortcutActive(bShouldBeActive);
		}
	}
}

// AWRShortcutTrigger Implementation
AWRShortcutTrigger::AWRShortcutTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	RootComponent = TriggerBox;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ShortcutPath = CreateDefaultSubobject<USplineComponent>(TEXT("ShortcutPath"));
	ShortcutPath->SetupAttachment(RootComponent);

	VisualEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VisualEffect"));
	VisualEffect->SetupAttachment(RootComponent);
	VisualEffect->SetAutoActivate(false);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AWRShortcutTrigger::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AWRShortcutTrigger::OnTriggerEndOverlap);
}

void AWRShortcutTrigger::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisualState();
}

void AWRShortcutTrigger::SetShortcutData(const FShortcutData& Data, int32 Index)
{
	ShortcutData = Data;
	ShortcutIndex = Index;

	// Setup spline path
	ShortcutPath->ClearSplinePoints();
	ShortcutPath->AddSplinePoint(Data.EntryPoint, ESplineCoordinateSpace::World);
	
	for (const FVector& Waypoint : Data.WaypointLocations)
	{
		ShortcutPath->AddSplinePoint(Waypoint, ESplineCoordinateSpace::World);
	}
	
	ShortcutPath->AddSplinePoint(Data.ExitPoint, ESplineCoordinateSpace::World);
	ShortcutPath->UpdateSpline();

	UpdateVisualState();
}

void AWRShortcutTrigger::SetShortcutActive(bool bActive)
{
	bIsActive = bActive;
	UpdateVisualState();
}

void AWRShortcutTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AWRKart* Kart = Cast<AWRKart>(OtherActor))
	{
		if (AWRShortcutSystem* ShortcutSystem = Cast<AWRShortcutSystem>(GetOwner()))
		{
			if (ShortcutSystem->IsShortcutAccessible(ShortcutIndex, Kart))
			{
				ShortcutSystem->DiscoverShortcut(ShortcutIndex, Kart);
				PlayDiscoveryEffects();
			}
		}
	}
}

void AWRShortcutTrigger::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Handle exit logic if needed
}

void AWRShortcutTrigger::UpdateVisualState()
{
	if (!bIsActive || ShortcutData.ShortcutType == EShortcutType::Hidden)
	{
		VisualMesh->SetVisibility(false);
		VisualEffect->SetVisibility(false);
	}
	else
	{
		VisualMesh->SetVisibility(bShowVisualHints);
		VisualEffect->SetVisibility(bShowVisualHints);
		
		if (bShowVisualHints)
		{
			// Set different colors based on difficulty
			FLinearColor EffectColor;
			switch (ShortcutData.Difficulty)
			{
				case EShortcutDifficulty::Easy:
					EffectColor = FLinearColor::Green;
					break;
				case EShortcutDifficulty::Medium:
					EffectColor = FLinearColor::Yellow;
					break;
				case EShortcutDifficulty::Hard:
					EffectColor = FLinearColor::Red;
					break;
				case EShortcutDifficulty::Expert:
					EffectColor = FLinearColor(1.f,0.f,1.f);
					break;
			}
			
			if (VisualEffect)
			{
				VisualEffect->SetColorParameter(FName("Color"), EffectColor);
			}
		}
	}
}

void AWRShortcutTrigger::PlayDiscoveryEffects()
{
	if (VisualEffect)
	{
		VisualEffect->Activate();
	}
}

// AWRShortcutGate Implementation
AWRShortcutGate::AWRShortcutGate()
{
	PrimaryActorTick.bCanEverTick = true;

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	RootComponent = GateMesh;

	SpeedCheckTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("SpeedCheckTrigger"));
	SpeedCheckTrigger->SetupAttachment(RootComponent);
	SpeedCheckTrigger->SetBoxExtent(FVector(200.0f, 100.0f, 100.0f));
	SpeedCheckTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpeedCheckTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SpeedCheckTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);

	SpeedCheckTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWRShortcutGate::OnSpeedCheckBeginOverlap);
}

void AWRShortcutGate::BeginPlay()
{
	Super::BeginPlay();
	
	ClosedPosition = GetActorLocation();
	OpenPosition = ClosedPosition + FVector(0, 0, 300); // Move up when open
}

void AWRShortcutGate::OpenGate()
{
	if (!bIsOpen && !bIsAnimating)
	{
		bIsAnimating = true;
		AnimationTime = 0.0f;
		
		if (OpenEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), OpenEffect, GetActorLocation(), FRotator::ZeroRotator);
		}
		
		if (OpenSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenSound, GetActorLocation());
		}
	}
}

void AWRShortcutGate::CloseGate()
{
	if (bIsOpen && !bIsAnimating)
	{
		bIsAnimating = true;
		AnimationTime = 0.0f;
	}
}

void AWRShortcutGate::OnSpeedCheckBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AWRKart* Kart = Cast<AWRKart>(OtherActor))
	{
		if (CheckKartRequirements(Kart))
		{
			OpenGate();
		}
	}
}

void AWRShortcutGate::UpdateGateAnimation(float DeltaTime)
{
	if (bIsAnimating)
	{
		AnimationTime += DeltaTime;
		float Alpha = AnimationTime / GateOpenTime;
		
		if (Alpha >= 1.0f)
		{
			Alpha = 1.0f;
			bIsAnimating = false;
			bIsOpen = !bIsOpen;
		}
		
		FVector TargetPosition = bIsOpen ? OpenPosition : ClosedPosition;
		FVector CurrentPosition = FMath::Lerp(ClosedPosition, TargetPosition, Alpha);
		SetActorLocation(CurrentPosition);
	}
}

bool AWRShortcutGate::CheckKartRequirements(AWRKart* Kart)
{
	if (!Kart)
		return false;
		
	// Check speed requirement
	if (RequiredSpeed > 0.0f && Kart->GetCurrentSpeed() < RequiredSpeed)
		return false;
		
	// Check boost requirement
	if (bRequiresBoost && !Kart->IsBoosting())
		return false;
		
	return true;
}

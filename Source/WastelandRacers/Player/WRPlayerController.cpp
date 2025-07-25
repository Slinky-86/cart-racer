#include "WRPlayerController.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "WastelandRacers/UI/WRHUDWidget.h"
#include "WastelandRacers/UI/WRProShopWidget.h"
#include "WastelandRacers/Core/WRGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

AWRPlayerController::AWRPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = false;

#if PLATFORM_ANDROID || PLATFORM_IOS
	bMobileInputEnabled = true;
#endif
}

void AWRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UWRHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->SetOwningPlayer(this);
		}
	}
}

void AWRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!bMobileInputEnabled)
	{
		// Keyboard/Gamepad input
		InputComponent->BindAxis("MoveForward", this, &AWRPlayerController::MoveForward);
		InputComponent->BindAxis("MoveRight", this, &AWRPlayerController::MoveRight);
		InputComponent->BindAxis("Brake", this, &AWRPlayerController::Brake);
		
		InputComponent->BindAction("UseWeapon", IE_Pressed, this, &AWRPlayerController::UseWeapon);
		InputComponent->BindAction("Drift", IE_Pressed, this, &AWRPlayerController::Drift);
		InputComponent->BindAction("Drift", IE_Released, this, &AWRPlayerController::StopDrift);
	}
}

void AWRPlayerController::ShowHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AWRPlayerController::HideHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AWRPlayerController::OpenProShop()
{
	if (ProShopWidgetClass && !ProShopWidget)
	{
		ProShopWidget = CreateWidget<UWRProShopWidget>(this, ProShopWidgetClass);
		if (ProShopWidget)
		{
			ProShopWidget->AddToViewport();
			
			// Pause game and show cursor
			SetPause(true);
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void AWRPlayerController::CloseProShop()
{
	if (ProShopWidget)
	{
		ProShopWidget->RemoveFromParent();
		ProShopWidget = nullptr;
		
		// Resume game and hide cursor
		SetPause(false);
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

void AWRPlayerController::OpenWorldMap()
{
	// Implementation for world map
	UE_LOG(LogTemp, Warning, TEXT("Opening World Map"));
}

void AWRPlayerController::TravelToRace(ETrackType TrackType)
{
	if (UWRGameInstance* GameInstance = Cast<UWRGameInstance>(GetGameInstance()))
	{
		GameInstance->TravelToRace(TrackType);
	}
}

void AWRPlayerController::SetMobileInputEnabled(bool bEnabled)
{
	bMobileInputEnabled = bEnabled;
}

void AWRPlayerController::MoveForward(float Value)
{
	if (AWRKart* Kart = Cast<AWRKart>(GetPawn()))
	{
		Kart->SetThrottleInput(Value);
	}
}

void AWRPlayerController::MoveRight(float Value)
{
	if (AWRKart* Kart = Cast<AWRKart>(GetPawn()))
	{
		Kart->SetSteeringInput(Value);
	}
}

void AWRPlayerController::Brake(float Value)
{
	if (AWRKart* Kart = Cast<AWRKart>(GetPawn()))
	{
		Kart->SetBrakeInput(Value);
	}
}

void AWRPlayerController::UseWeapon()
{
	if (AWRKart* Kart = Cast<AWRKart>(GetPawn()))
	{
		Kart->UseWeapon();
	}
}

void AWRPlayerController::Drift()
{
	if (AWRKart* Kart = Cast<AWRKart>(GetPawn()))
	{
		Kart->StartDrift();
	}
}

void AWRPlayerController::StopDrift()
{
	if (AWRKart* Kart = Cast<AWRKart>(GetPawn()))
	{
		Kart->StopDrift();
	}
}

// Mobile Touch Input Functions
void AWRPlayerController::OnTouchAccelerate()
{
	MoveForward(1.0f);
}

void AWRPlayerController::OnTouchBrake()
{
	Brake(1.0f);
}

void AWRPlayerController::OnTouchSteer(float SteerValue)
{
	MoveRight(SteerValue);
}

void AWRPlayerController::OnTouchWeapon()
{
	UseWeapon();
}

void AWRPlayerController::OnTouchDrift(bool bStartDrift)
{
	if (bStartDrift)
	{
		Drift();
	}
	else
	{
		StopDrift();
	}
}

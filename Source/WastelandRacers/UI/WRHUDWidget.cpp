#include "WRHUDWidget.h"
#include "WastelandRacers/Player/WRPlayerController.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "Components/Button.h"
#include "Components/Widget.h"

void UWRHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind mobile control events
	if (AccelerateButton)
	{
		AccelerateButton->OnPressed.AddDynamic(this, &UWRHUDWidget::OnAcceleratePressed);
		AccelerateButton->OnReleased.AddDynamic(this, &UWRHUDWidget::OnAccelerateReleased);
	}

	if (BrakeButton)
	{
		BrakeButton->OnPressed.AddDynamic(this, &UWRHUDWidget::OnBrakePressed);
		BrakeButton->OnReleased.AddDynamic(this, &UWRHUDWidget::OnBrakeReleased);
	}

	if (WeaponButton)
	{
		WeaponButton->OnPressed.AddDynamic(this, &UWRHUDWidget::OnWeaponPressed);
	}

	if (DriftButton)
	{
		DriftButton->OnPressed.AddDynamic(this, &UWRHUDWidget::OnDriftPressed);
		DriftButton->OnReleased.AddDynamic(this, &UWRHUDWidget::OnDriftReleased);
	}

#if PLATFORM_ANDROID || PLATFORM_IOS
	// Show mobile controls
	if (AccelerateButton) AccelerateButton->SetVisibility(ESlateVisibility::Visible);
	if (BrakeButton) BrakeButton->SetVisibility(ESlateVisibility::Visible);
	if (WeaponButton) WeaponButton->SetVisibility(ESlateVisibility::Visible);
	if (DriftButton) DriftButton->SetVisibility(ESlateVisibility::Visible);
	if (SteeringWheel) SteeringWheel->SetVisibility(ESlateVisibility::Visible);
#else
	// Hide mobile controls on PC
	if (AccelerateButton) AccelerateButton->SetVisibility(ESlateVisibility::Hidden);
	if (BrakeButton) BrakeButton->SetVisibility(ESlateVisibility::Hidden);
	if (WeaponButton) WeaponButton->SetVisibility(ESlateVisibility::Hidden);
	if (DriftButton) DriftButton->SetVisibility(ESlateVisibility::Hidden);
	if (SteeringWheel) SteeringWheel->SetVisibility(ESlateVisibility::Hidden);
#endif
}

void UWRHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateHUDElements();
}

void UWRHUDWidget::SetOwningPlayer(AWRPlayerController* PlayerController)
{
	OwningPlayerController = PlayerController;
	if (OwningPlayerController)
	{
		PlayerKart = Cast<AWRKart>(OwningPlayerController->GetPawn());
	}
}

void UWRHUDWidget::OnAcceleratePressed()
{
	if (OwningPlayerController)
	{
		OwningPlayerController->OnTouchAccelerate();
	}
}

void UWRHUDWidget::OnAccelerateReleased()
{
	if (OwningPlayerController)
	{
		OwningPlayerController->OnTouchSteer(0.0f);
	}
}

void UWRHUDWidget::OnBrakePressed()
{
	if (OwningPlayerController)
	{
		OwningPlayerController->OnTouchBrake();
	}
}

void UWRHUDWidget::OnBrakeReleased()
{
	// Stop braking
}

void UWRHUDWidget::OnWeaponPressed()
{
	if (OwningPlayerController)
	{
		OwningPlayerController->OnTouchWeapon();
	}
}

void UWRHUDWidget::OnDriftPressed()
{
	if (OwningPlayerController)
	{
		OwningPlayerController->OnTouchDrift(true);
	}
}

void UWRHUDWidget::OnDriftReleased()
{
	if (OwningPlayerController)
	{
		OwningPlayerController->OnTouchDrift(false);
	}
}

void UWRHUDWidget::UpdateHUDElements()
{
	if (!PlayerKart)
		return;

	// Update speedometer
	float CurrentSpeed = PlayerKart->GetCurrentSpeed();
	UpdateSpeedometer(CurrentSpeed);

	// Update position
	int32 Position = PlayerKart->GetPosition();
	UpdatePosition(Position, 8); // Assuming max 8 players

	// Update lap counter
	int32 CurrentLap = PlayerKart->GetCurrentLap();
	UpdateLapCounter(CurrentLap, 3); // Assuming 3 laps

	// Update weapon display
	// This would need weapon component integration
}

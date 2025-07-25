#include "WRMinimalHUD.h"
#include "WastelandRacers/Player/WRPlayerController.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"

void UWRMinimalHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// Hide countdown and race finished overlays initially
	if (CountdownOverlay)
	{
		CountdownOverlay->SetVisibility(ESlateVisibility::Hidden);
	}

	if (RaceFinishedOverlay)
	{
		RaceFinishedOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWRMinimalHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateHUDElements();
}

void UWRMinimalHUD::SetOwningPlayer(AWRPlayerController* PlayerController)
{
	OwningPlayerController = PlayerController;
	if (OwningPlayerController)
	{
		PlayerKart = Cast<AWRKart>(OwningPlayerController->GetPawn());
	}
}

void UWRMinimalHUD::SetHUDVisibility(bool bVisible)
{
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UWRMinimalHUD::UpdateHUDElements()
{
	if (!PlayerKart)
		return;

	// Update position
	int32 Position = PlayerKart->GetPosition();
	UpdatePosition(Position, 8); // Assuming max 8 players

	// Update lap counter
	int32 CurrentLap = PlayerKart->GetCurrentLap();
	UpdateLapCounter(CurrentLap, 3); // Assuming 3 laps

	// Update position text directly
	if (PositionText)
	{
		FString PositionString = FString::Printf(TEXT("%d%s"), Position, *GetOrdinalSuffix(Position));
		PositionText->SetText(FText::FromString(PositionString));
	}

	// Update lap text directly
	if (LapText)
	{
		FString LapString = FString::Printf(TEXT("LAP %d/3"), CurrentLap);
		LapText->SetText(FText::FromString(LapString));
	}
}

FString UWRMinimalHUD::GetOrdinalSuffix(int32 Number) const
{
	if (Number >= 11 && Number <= 13)
	{
		return TEXT("th");
	}

	switch (Number % 10)
	{
		case 1: return TEXT("st");
		case 2: return TEXT("nd");
		case 3: return TEXT("rd");
		default: return TEXT("th");
	}
}

#include "WRMainMenuWidget.h"
#include "WastelandRacers/UI/WRSettingsWidget.h"
#include "WastelandRacers/UI/WRMultiplayerWidget.h"
#include "WastelandRacers/Core/WRGameInstance.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"

void UWRMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindButtonEvents();
}

void UWRMainMenuWidget::BindButtonEvents()
{
	if (StoryModeButton)
	{
		StoryModeButton->OnClicked.AddDynamic(this, &UWRMainMenuWidget::OnStoryModeClicked);
	}

	if (MultiplayerButton)
	{
		MultiplayerButton->OnClicked.AddDynamic(this, &UWRMainMenuWidget::OnMultiplayerClicked);
	}

	if (ProShopButton)
	{
		ProShopButton->OnClicked.AddDynamic(this, &UWRMainMenuWidget::OnProShopClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UWRMainMenuWidget::OnSettingsClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UWRMainMenuWidget::OnExitClicked);
	}
}

void UWRMainMenuWidget::OnStoryModeClicked()
{
	if (UWRGameInstance* GameInstance = Cast<UWRGameInstance>(GetGameInstance()))
	{
		GameInstance->SetGameMode(EGameMode::FreeRoam);
		GameInstance->TravelToWorld(EStoryWorld::Pandora);
	}
}

void UWRMainMenuWidget::OnMultiplayerClicked()
{
	if (MultiplayerWidgetClass)
	{
		UWRMultiplayerWidget* MultiplayerWidget = CreateWidget<UWRMultiplayerWidget>(this, MultiplayerWidgetClass);
		if (MultiplayerWidget)
		{
			MultiplayerWidget->AddToViewport();
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWRMainMenuWidget::OnProShopClicked()
{
	if (UWRGameInstance* GameInstance = Cast<UWRGameInstance>(GetGameInstance()))
	{
		GameInstance->OpenProShop();
	}
}

void UWRMainMenuWidget::OnSettingsClicked()
{
	if (SettingsWidgetClass)
	{
		UWRSettingsWidget* SettingsWidget = CreateWidget<UWRSettingsWidget>(this, SettingsWidgetClass);
		if (SettingsWidget)
		{
			SettingsWidget->AddToViewport();
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWRMainMenuWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

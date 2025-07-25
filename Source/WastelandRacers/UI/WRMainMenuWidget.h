#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WastelandRacers/Core/WRGameInstance.h"
#include "WRMainMenuWidget.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnStoryModeClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnMultiplayerClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnProShopClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnSettingsClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnExitClicked();

protected:
	// Main Menu Buttons
	UPROPERTY(meta = (BindWidget))
	class UButton* StoryModeButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* MultiplayerButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ProShopButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UWRSettingsWidget> SettingsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UWRMultiplayerWidget> MultiplayerWidgetClass;

private:
	void BindButtonEvents();
};

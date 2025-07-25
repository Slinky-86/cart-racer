#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WastelandRacers/Multiplayer/WRNetworkManager.h"
#include "WRMultiplayerWidget.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRMultiplayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void OnCreateSessionClicked();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void OnFindSessionsClicked();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void OnJoinSessionClicked();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void OnBackClicked();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void RefreshServerList();

protected:
	// Create Session Panel
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SessionNameTextBox;

	UPROPERTY(meta = (BindWidget))
	class USpinBox* MaxPlayersSpinBox;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* PasswordTextBox;

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* LANCheckBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateSessionButton;

	// Server Browser Panel
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ServerListScrollBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* RefreshButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinSelectedButton;

	// Navigation
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatusText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> ServerEntryWidgetClass;

private:
	int32 SelectedSessionIndex = -1;

	UFUNCTION()
	void OnSessionCreated(bool bWasSuccessful);

	UFUNCTION()
	void OnSessionsFound(const TArray<FSessionInfo>& Sessions);

	UFUNCTION()
	void OnSessionJoined(bool bWasSuccessful);

	void PopulateServerList(const TArray<FSessionInfo>& Sessions);
	void UpdateStatus(const FString& Message);
};

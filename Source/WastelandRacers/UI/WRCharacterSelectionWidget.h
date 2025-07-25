#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WastelandRacers/Characters/WRCharacterData.h"
#include "WRCharacterSelectionWidget.generated.h"

UCLASS()
class WASTELANDRACERS_API UWRCharacterSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Character Selection")
	void PopulateCharacterList();

	UFUNCTION(BlueprintCallable, Category = "Character Selection")
	void OnCharacterSelected(ECharacterType CharacterType);

	UFUNCTION(BlueprintCallable, Category = "Character Selection")
	void OnConfirmClicked();

	UFUNCTION(BlueprintCallable, Category = "Character Selection")
	void OnBackClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* CharacterListBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* CharacterPreviewImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CharacterNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CharacterDescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SpeedStatBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AccelerationStatBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HandlingStatBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* WeaponStatBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* DefenseStatBar;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> CharacterEntryWidgetClass;

private:
	ECharacterType SelectedCharacter = ECharacterType::Axton;

	void UpdateCharacterPreview();
	void UpdateStatBars(const FCharacterStats& Stats);
};

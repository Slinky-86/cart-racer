#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WRFreeRoamController.generated.h"

UCLASS()
class WASTELANDRACERS_API AWRFreeRoamController : public APlayerController
{
	GENERATED_BODY()

public:
	AWRFreeRoamController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Free Roam")
	void InteractWithRacePortal();

	UFUNCTION(BlueprintCallable, Category = "Free Roam")
	void OpenWorldMap();

	UFUNCTION(BlueprintCallable, Category = "Free Roam")
	void OpenProShop();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> WorldMapWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> RacePortalWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UUserWidget* CurrentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 500.0f;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);
	void Interact();
	void ToggleMap();

	class AActor* GetNearestInteractable() const;
};

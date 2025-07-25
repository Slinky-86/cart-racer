#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WRProShopWidget.generated.h"

UCLASS(BlueprintType)
class WASTELANDRACERS_API UWRProShopWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent, Category="ProShop")
    void RefreshShop();
};

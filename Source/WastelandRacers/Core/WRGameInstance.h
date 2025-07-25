#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WastelandRacers/Shop/WRProShop.h"
#include "WastelandRacers/Story/WRStoryManager.h"
#include "WastelandRacers/Core/WREngineClass.h"
#include "WRGameInstance.generated.h"

UENUM(BlueprintType)
enum class EGameMode : uint8
{
	MainMenu,
	FreeRoam,
	Race,
	Multiplayer,
	ProShop
};

UCLASS()
class WASTELANDRACERS_API UWRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UWRGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void CreateSession(int32 MaxPlayers = 8);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void FindSessions();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinSession(int32 SessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void DestroySession();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetGameMode(EGameMode NewGameMode);

	UFUNCTION(BlueprintPure, Category = "Game")
	EGameMode GetCurrentGameMode() const { return CurrentGameMode; }

	UFUNCTION(BlueprintCallable, Category = "Story")
	void TravelToWorld(EStoryWorld WorldType);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void TravelToRace(ETrackType TrackType);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void OpenProShop();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void CloseProShop();

	

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	EGameMode CurrentGameMode = EGameMode::MainMenu;

	UPROPERTY(BlueprintReadOnly, Category = "Story")
	EStoryWorld CurrentWorld = EStoryWorld::Pandora;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	class UWRProShop* ProShopData;

	UPROPERTY(BlueprintReadWrite, Category = "Engine")
	EEngineClass CurrentEngineClass = EEngineClass::EC_100cc;

	UFUNCTION(BlueprintCallable, Category = "Engine")
	void SetEngineClass(EEngineClass NewClass) { CurrentEngineClass = NewClass; }

protected:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
};

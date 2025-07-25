#include "WRGameInstance.h"
#include "WastelandRacers.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "WastelandRacers/Shop/WRProShop.h"
#include "WastelandRacers/Story/WRStoryManager.h"
#include "Kismet/GameplayStatics.h"

UWRGameInstance::UWRGameInstance()
{
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UWRGameInstance::OnCreateSessionComplete);
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UWRGameInstance::OnFindSessionsComplete);
	JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UWRGameInstance::OnJoinSessionComplete);
}

void UWRGameInstance::Init()
{
	Super::Init();

	// Initialize Pro Shop data
	ProShopData = NewObject<UWRProShop>(this);
	if (ProShopData)
	{
		ProShopData->LoadPlayerProgress();
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		UE_LOG(LogWastelandRacers, Warning, TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString());
	}
}

void UWRGameInstance::SetGameMode(EGameMode NewGameMode)
{
	CurrentGameMode = NewGameMode;
	
	switch (NewGameMode)
	{
		case EGameMode::MainMenu:
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
			break;
		case EGameMode::FreeRoam:
			// Load current world for free roam
			TravelToWorld(CurrentWorld);
			break;
		case EGameMode::ProShop:
			OpenProShop();
			break;
	}
}

void UWRGameInstance::TravelToWorld(EStoryWorld WorldType)
{
	CurrentWorld = WorldType;
	CurrentGameMode = EGameMode::FreeRoam;
	
	FString LevelName;
	switch (WorldType)
	{
		case EStoryWorld::Pandora:
			LevelName = TEXT("World_Pandora");
			break;
		case EStoryWorld::Opportunity:
			LevelName = TEXT("World_Opportunity");
			break;
		case EStoryWorld::EridiumMines:
			LevelName = TEXT("World_EridiumMines");
			break;
		case EStoryWorld::WildlifePreserve:
			LevelName = TEXT("World_WildlifePreserve");
			break;
		case EStoryWorld::HyperionMoonBase:
			LevelName = TEXT("World_HyperionMoonBase");
			break;
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

void UWRGameInstance::TravelToRace(ETrackType TrackType)
{
	CurrentGameMode = EGameMode::Race;
	
	FString LevelName;
	switch (TrackType)
	{
		case ETrackType::PandoraDesert:
			LevelName = TEXT("Track_PandoraDesert");
			break;
		case ETrackType::OpportunityRuins:
			LevelName = TEXT("Track_OpportunityRuins");
			break;
		case ETrackType::EridiumMines:
			LevelName = TEXT("Track_EridiumMines");
			break;
		case ETrackType::WildlifePreserve:
			LevelName = TEXT("Track_WildlifePreserve");
			break;
		case ETrackType::HyperionMoonBase:
			LevelName = TEXT("Track_HyperionMoonBase");
			break;
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

void UWRGameInstance::OpenProShop()
{
	CurrentGameMode = EGameMode::ProShop;
	if (ProShopData && ProShopData->EnterDealership(this))
	{
		return; // Successfully loaded dealership level
	}
	else
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("ProShop Showroom level invalid – falling back to UI only"));
		// Pro shop UI will be handled by the widget system
	}
}

void UWRGameInstance::CloseProShop()
{
	CurrentGameMode = EGameMode::FreeRoam;
	// Return to previous world
}
void UWRGameInstance::CreateSession(int32 MaxPlayers)
{
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}

		CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

		TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
		SessionSettings->bIsLANMatch = false;
		SessionSettings->NumPublicConnections = MaxPlayers;
		SessionSettings->bAllowJoinInProgress = true;
		SessionSettings->bAllowJoinViaPresence = true;
		SessionSettings->bShouldAdvertise = true;
		SessionSettings->bUsesPresence = true;
		SessionSettings->bUseLobbiesIfAvailable = true;
		SessionSettings->Set(FName("GameMode"), FString("WastelandRacers"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
		SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
	}
}

void UWRGameInstance::FindSessions()
{
	if (SessionInterface.IsValid())
	{
		FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		SessionSearch->MaxSearchResults = 20;
		SessionSearch->bIsLanQuery = false;
		SessionSearch->QuerySettings.Set(TEXT("Presence"), true, EOnlineComparisonOp::Equals);

		const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
		SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
	}
}

void UWRGameInstance::JoinSession(int32 SessionIndex)
{
	if (SessionInterface.IsValid() && SessionSearch.IsValid())
	{
		JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

		const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
		SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
	}
}

void UWRGameInstance::DestroySession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
}

void UWRGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	OnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UWRGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	OnFindSessionsComplete.Broadcast(bWasSuccessful);
}

void UWRGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		FString TravelURL;
		if (SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
		{
			APlayerController* PlayerController = GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	OnJoinSessionComplete.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
}

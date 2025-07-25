#include "WRNetworkManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UWRNetworkManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentNetworkState = ENetworkState::Offline;

	// Get Online Subsystem
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		
		if (SessionInterface.IsValid())
		{
			// Bind delegates
			CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UWRNetworkManager::OnCreateSessionComplete);
			FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UWRNetworkManager::OnFindSessionsComplete);
			JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UWRNetworkManager::OnJoinSessionComplete);
			DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UWRNetworkManager::OnDestroySessionComplete);
			SessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UWRNetworkManager::OnSessionUserInviteAccepted);
		}

		UE_LOG(LogTemp, Warning, TEXT("WRNetworkManager initialized with subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Online Subsystem"));
	}
}

void UWRNetworkManager::Deinitialize()
{
	// Clean up delegates
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(SessionUserInviteAcceptedDelegateHandle);
	}

	Super::Deinitialize();
}

UWRNetworkManager* UWRNetworkManager::GetInstance(const UObject* WorldContext)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UWRNetworkManager>();
		}
	}
	return nullptr;
}

void UWRNetworkManager::CreateSession(const FString& SessionName, int32 MaxPlayers, bool bIsLAN, const FString& Password)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionCreated.Broadcast(false);
		return;
	}

	// Destroy existing session if any
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		DestroySession();
	}

	SetNetworkState(ENetworkState::Connecting);

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = bIsLAN;
	SessionSettings->NumPublicConnections = MaxPlayers;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;

	// Set custom session data
	SessionSettings->Set(FName("SessionName"), SessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName("GameMode"), FString("WastelandRacers"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName("HasPassword"), !Password.IsEmpty(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	if (!Password.IsEmpty())
	{
		SessionSettings->Set(FName("Password"), Password, EOnlineDataAdvertisementType::DontAdvertise);
	}

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		OnSessionCreated.Broadcast(false);
		SetNetworkState(ENetworkState::Error);
	}

	CurrentSessionName = SessionName;
}

void UWRNetworkManager::FindSessions(bool bIsLAN)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionFound.Broadcast(TArray<FSessionInfo>());
		return;
	}

	SetNetworkState(ENetworkState::Connecting);

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 50;
	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->QuerySettings.Set(TEXT("Presence"), true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		OnSessionFound.Broadcast(TArray<FSessionInfo>());
		SetNetworkState(ENetworkState::Error);
	}
}

void UWRNetworkManager::JoinSession(int32 SessionIndex)
{
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid() || SessionIndex >= SessionSearch->SearchResults.Num())
	{
		OnSessionJoined.Broadcast(false);
		return;
	}

	SetNetworkState(ENetworkState::Connecting);

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[SessionIndex]))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		OnSessionJoined.Broadcast(false);
		SetNetworkState(ENetworkState::Error);
	}
}

void UWRNetworkManager::JoinSessionByName(const FString& SessionName, const FString& Password)
{
	// Find session by name first
	for (int32 i = 0; i < AvailableSessions.Num(); i++)
	{
		if (AvailableSessions[i].SessionName == SessionName)
		{
			// Validate password if required
			if (AvailableSessions[i].bHasPassword && !ValidatePassword(Password))
			{
				OnSessionJoined.Broadcast(false);
				return;
			}

			JoinSession(i);
			return;
		}
	}

	OnSessionJoined.Broadcast(false);
}

void UWRNetworkManager::LeaveSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->EndSession(NAME_GameSession);
		SetNetworkState(ENetworkState::Offline);
	}
}

void UWRNetworkManager::DestroySession()
{
	if (!SessionInterface.IsValid())
		return;

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		SetNetworkState(ENetworkState::Offline);
	}
}

void UWRNetworkManager::RefreshServerList()
{
	FindSessions(false);
}

bool UWRNetworkManager::IsHost() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetNetMode() == NM_ListenServer;
	}
	return false;
}

bool UWRNetworkManager::IsInSession() const
{
	return CurrentNetworkState == ENetworkState::InLobby || CurrentNetworkState == ENetworkState::InGame;
}

int32 UWRNetworkManager::GetPlayerCount() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetNumPlayerControllers();
	}
	return 0;
}

void UWRNetworkManager::KickPlayer(const FString& PlayerName)
{
	// Implementation for kicking players
	if (IsHost())
	{
		// Find and kick the player
		UE_LOG(LogTemp, Warning, TEXT("Kicking player: %s"), *PlayerName);
	}
}

void UWRNetworkManager::BanPlayer(const FString& PlayerName)
{
	// Implementation for banning players
	if (IsHost())
	{
		// Add to ban list and kick
		UE_LOG(LogTemp, Warning, TEXT("Banning player: %s"), *PlayerName);
	}
}

TArray<FString> UWRNetworkManager::GetConnectedPlayers() const
{
	return ConnectedPlayers;
}

void UWRNetworkManager::StartNetworkGame(const FString& MapName)
{
	if (IsHost())
	{
		SetNetworkState(ENetworkState::InGame);
		
		// Travel to the game map
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(MapName + TEXT("?listen"));
		}
	}
}

void UWRNetworkManager::EndNetworkGame()
{
	if (IsHost())
	{
		SetNetworkState(ENetworkState::InLobby);
		
		// Return to lobby map
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(TEXT("Lobby?listen"));
		}
	}
}

void UWRNetworkManager::SetGameSettings(const FString& GameMode, int32 LapCount, const FString& TrackName)
{
	if (IsHost() && SessionInterface.IsValid())
	{
		FOnlineSessionSettings* SessionSettings = SessionInterface->GetSessionSettings(NAME_GameSession);
		if (SessionSettings)
		{
			SessionSettings->Set(FName("GameMode"), GameMode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			SessionSettings->Set(FName("LapCount"), LapCount, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			SessionSettings->Set(FName("TrackName"), TrackName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			
			SessionInterface->UpdateSession(NAME_GameSession, *SessionSettings);
		}
	}
}

void UWRNetworkManager::SetNetworkState(ENetworkState NewState)
{
	if (CurrentNetworkState != NewState)
	{
		CurrentNetworkState = NewState;
		OnNetworkStateChanged.Broadcast(NewState);
	}
}

void UWRNetworkManager::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		SetNetworkState(ENetworkState::InLobby);
	}
	else
	{
		SetNetworkState(ENetworkState::Error);
	}

	OnSessionCreated.Broadcast(bWasSuccessful);
}

void UWRNetworkManager::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		ConvertSearchResultsToSessionInfo();
		SetNetworkState(ENetworkState::Connected);
	}
	else
	{
		AvailableSessions.Empty();
		SetNetworkState(ENetworkState::Error);
	}

	OnSessionFound.Broadcast(AvailableSessions);
}

void UWRNetworkManager::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		if (Result == EOnJoinSessionCompleteResult::Success)
		{
			FString TravelURL;
			if (SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
			{
				if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
					SetNetworkState(ENetworkState::InLobby);
				}
			}
		}
		else
		{
			SetNetworkState(ENetworkState::Error);
		}
	}

	OnSessionJoined.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
}

void UWRNetworkManager::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	SetNetworkState(ENetworkState::Offline);
}

void UWRNetworkManager::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, 
	TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful && SessionInterface.IsValid())
	{
		// Join the session from invite
		const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
		SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, InviteResult);
	}
}

void UWRNetworkManager::ConvertSearchResultsToSessionInfo()
{
	AvailableSessions.Empty();

	if (!SessionSearch.IsValid())
		return;

	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		FSessionInfo SessionInfo;
		
		// Get session name
		FString SessionName;
		SearchResult.Session.SessionSettings.Get(FName("SessionName"), SessionName);
		SessionInfo.SessionName = SessionName;

		// Get host name
		SessionInfo.HostName = SearchResult.Session.OwningUserName;

		// Get player counts
		SessionInfo.CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
		SessionInfo.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;

		// Get ping
		SessionInfo.Ping = SearchResult.PingInMs;

		// Check for password
		bool bHasPassword = false;
		SearchResult.Session.SessionSettings.Get(FName("HasPassword"), bHasPassword);
		SessionInfo.bHasPassword = bHasPassword;

		// Get game mode
		FString GameMode;
		SearchResult.Session.SessionSettings.Get(FName("GameMode"), GameMode);
		SessionInfo.GameMode = GameMode;

		// Get map name
		FString MapName;
		SearchResult.Session.SessionSettings.Get(FName("TrackName"), MapName);
		SessionInfo.MapName = MapName;

		AvailableSessions.Add(SessionInfo);
	}
}

bool UWRNetworkManager::ValidatePassword(const FString& Password) const
{
	// Simple password validation - in a real implementation,
	// you'd want to hash and compare passwords securely
	return !Password.IsEmpty();
}

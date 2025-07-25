#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WRNetworkManager.generated.h"

UENUM(BlueprintType)
enum class ENetworkState : uint8
{
	Offline,
	Connecting,
	Connected,
	InLobby,
	InGame,
	Disconnected,
	Error
};

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString SessionName;

	UPROPERTY(BlueprintReadWrite)
	FString HostName;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentPlayers;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite)
	int32 Ping;

	UPROPERTY(BlueprintReadWrite)
	bool bHasPassword;

	UPROPERTY(BlueprintReadWrite)
	FString GameMode;

	UPROPERTY(BlueprintReadWrite)
	FString MapName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkStateChanged, ENetworkState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionFound, const TArray<FSessionInfo>&, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionJoined, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, const FString&, PlayerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, const FString&, PlayerName);

UCLASS()
class WASTELANDRACERS_API UWRNetworkManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Network")
	static UWRNetworkManager* GetInstance(const UObject* WorldContext);

	// Session Management
	UFUNCTION(BlueprintCallable, Category = "Network|Session")
	void CreateSession(const FString& SessionName, int32 MaxPlayers = 8, bool bIsLAN = false, const FString& Password = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Network|Session")
	void FindSessions(bool bIsLAN = false);

	UFUNCTION(BlueprintCallable, Category = "Network|Session")
	void JoinSession(int32 SessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Network|Session")
	void JoinSessionByName(const FString& SessionName, const FString& Password = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Network|Session")
	void LeaveSession();

	UFUNCTION(BlueprintCallable, Category = "Network|Session")
	void DestroySession();

	// Server Browser
	UFUNCTION(BlueprintCallable, Category = "Network|Browser")
	void RefreshServerList();

	UFUNCTION(BlueprintPure, Category = "Network|Browser")
	TArray<FSessionInfo> GetAvailableSessions() const { return AvailableSessions; }

	// Network State
	UFUNCTION(BlueprintPure, Category = "Network|State")
	ENetworkState GetNetworkState() const { return CurrentNetworkState; }

	UFUNCTION(BlueprintPure, Category = "Network|State")
	bool IsHost() const;

	UFUNCTION(BlueprintPure, Category = "Network|State")
	bool IsInSession() const;

	UFUNCTION(BlueprintPure, Category = "Network|State")
	int32 GetPlayerCount() const;

	UFUNCTION(BlueprintPure, Category = "Network|State")
	FString GetSessionName() const { return CurrentSessionName; }

	// Player Management
	UFUNCTION(BlueprintCallable, Category = "Network|Players")
	void KickPlayer(const FString& PlayerName);

	UFUNCTION(BlueprintCallable, Category = "Network|Players")
	void BanPlayer(const FString& PlayerName);

	UFUNCTION(BlueprintPure, Category = "Network|Players")
	TArray<FString> GetConnectedPlayers() const;

	// Game Management
	UFUNCTION(BlueprintCallable, Category = "Network|Game")
	void StartNetworkGame(const FString& MapName);

	UFUNCTION(BlueprintCallable, Category = "Network|Game")
	void EndNetworkGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Game")
	void SetGameSettings(const FString& GameMode, int32 LapCount, const FString& TrackName);

	// Events
	UPROPERTY(BlueprintAssignable)
	FOnNetworkStateChanged OnNetworkStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnSessionCreated OnSessionCreated;

	UPROPERTY(BlueprintAssignable)
	FOnSessionFound OnSessionFound;

	UPROPERTY(BlueprintAssignable)
	FOnSessionJoined OnSessionJoined;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerJoined OnPlayerJoined;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerLeft OnPlayerLeft;

protected:
	UPROPERTY()
	ENetworkState CurrentNetworkState;

	UPROPERTY()
	FString CurrentSessionName;

	UPROPERTY()
	TArray<FSessionInfo> AvailableSessions;

	UPROPERTY()
	TArray<FString> ConnectedPlayers;

	// Online Subsystem
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	// Delegates
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FOnSessionUserInviteAcceptedDelegate SessionUserInviteAcceptedDelegate;

	// Delegate Handles
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FDelegateHandle SessionUserInviteAcceptedDelegateHandle;

private:
	void SetNetworkState(ENetworkState NewState);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, 
		TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

	void ConvertSearchResultsToSessionInfo();
	bool ValidatePassword(const FString& Password) const;
};

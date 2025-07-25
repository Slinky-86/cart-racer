#include "WRMultiplayerWidget.h"
#include "WastelandRacers/Multiplayer/WRNetworkManager.h"
#include "Components/EditableTextBox.h"
#include "Components/SpinBox.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

void UWRMultiplayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (CreateSessionButton)
	{
		CreateSessionButton->OnClicked.AddDynamic(this, &UWRMultiplayerWidget::OnCreateSessionClicked);
	}

	if (RefreshButton)
	{
		RefreshButton->OnClicked.AddDynamic(this, &UWRMultiplayerWidget::OnFindSessionsClicked);
	}

	if (JoinSelectedButton)
	{
		JoinSelectedButton->OnClicked.AddDynamic(this, &UWRMultiplayerWidget::OnJoinSessionClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UWRMultiplayerWidget::OnBackClicked);
	}

	// Bind network events
	if (UWRNetworkManager* NetworkManager = UWRNetworkManager::GetInstance(GetWorld()))
	{
		NetworkManager->OnSessionCreated.AddDynamic(this, &UWRMultiplayerWidget::OnSessionCreated);
		NetworkManager->OnSessionFound.AddDynamic(this, &UWRMultiplayerWidget::OnSessionsFound);
		NetworkManager->OnSessionJoined.AddDynamic(this, &UWRMultiplayerWidget::OnSessionJoined);
	}

	// Initialize UI
	if (MaxPlayersSpinBox)
	{
		MaxPlayersSpinBox->SetValue(8);
	}

	RefreshServerList();
}

void UWRMultiplayerWidget::OnCreateSessionClicked()
{
	if (UWRNetworkManager* NetworkManager = UWRNetworkManager::GetInstance(GetWorld()))
	{
		FString SessionName = SessionNameTextBox ? SessionNameTextBox->GetText().ToString() : TEXT("Wasteland Race");
		int32 MaxPlayers = MaxPlayersSpinBox ? (int32)MaxPlayersSpinBox->GetValue() : 8;
		FString Password = PasswordTextBox ? PasswordTextBox->GetText().ToString() : TEXT("");
		bool bIsLAN = LANCheckBox ? LANCheckBox->IsChecked() : false;

		NetworkManager->CreateSession(SessionName, MaxPlayers, bIsLAN, Password);
		UpdateStatus(TEXT("Creating session..."));
	}
}

void UWRMultiplayerWidget::OnFindSessionsClicked()
{
	RefreshServerList();
}

void UWRMultiplayerWidget::OnJoinSessionClicked()
{
	if (SelectedSessionIndex >= 0)
	{
		if (UWRNetworkManager* NetworkManager = UWRNetworkManager::GetInstance(GetWorld()))
		{
			NetworkManager->JoinSession(SelectedSessionIndex);
			UpdateStatus(TEXT("Joining session..."));
		}
	}
	else
	{
		UpdateStatus(TEXT("Please select a session to join"));
	}
}

void UWRMultiplayerWidget::OnBackClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
	RemoveFromParent();
}

void UWRMultiplayerWidget::RefreshServerList()
{
	if (UWRNetworkManager* NetworkManager = UWRNetworkManager::GetInstance(GetWorld()))
	{
		bool bIsLAN = LANCheckBox ? LANCheckBox->IsChecked() : false;
		NetworkManager->FindSessions(bIsLAN);
		UpdateStatus(TEXT("Searching for sessions..."));
	}
}

void UWRMultiplayerWidget::OnSessionCreated(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UpdateStatus(TEXT("Session created successfully!"));
		// Transition to lobby
	}
	else
	{
		UpdateStatus(TEXT("Failed to create session"));
	}
}

void UWRMultiplayerWidget::OnSessionsFound(const TArray<FSessionInfo>& Sessions)
{
	PopulateServerList(Sessions);
	UpdateStatus(FString::Printf(TEXT("Found %d sessions"), Sessions.Num()));
}

void UWRMultiplayerWidget::OnSessionJoined(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UpdateStatus(TEXT("Joined session successfully!"));
		// Transition to lobby
	}
	else
	{
		UpdateStatus(TEXT("Failed to join session"));
	}
}

void UWRMultiplayerWidget::PopulateServerList(const TArray<FSessionInfo>& Sessions)
{
	if (!ServerListScrollBox)
		return;

	// Clear existing entries
	ServerListScrollBox->ClearChildren();

	// Add session entries
	for (int32 i = 0; i < Sessions.Num(); i++)
	{
		const FSessionInfo& Session = Sessions[i];
		
		if (ServerEntryWidgetClass)
		{
			UUserWidget* EntryWidget = CreateWidget<UUserWidget>(this, ServerEntryWidgetClass);
			if (EntryWidget)
			{
				// Set session data on entry widget
				// This would be implemented in the Blueprint
				ServerListScrollBox->AddChild(EntryWidget);
			}
		}
	}
}

void UWRMultiplayerWidget::UpdateStatus(const FString& Message)
{
	if (StatusText)
	{
		StatusText->SetText(FText::FromString(Message));
	}
}

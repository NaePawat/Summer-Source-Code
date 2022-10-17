// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameInstance.h"

#include <string>

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

USGameInstance::USGameInstance()
{
}

void USGameInstance::Init()
{
	//Super::Init();
	if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			//Bind Delegate
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&USGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&USGameInstance::OnJoinSessionComplete);
		}
	}
}

void USGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	if(Succeeded)
	{
		UE_LOG(LogTemp,Warning,TEXT("Session Created!"));
		GetWorld()->ServerTravel("/Game/StarterContent/Maps/Minimal_Default?listen",true);
	}
}

void USGameInstance::OnFindSessionComplete(bool Succeeded)
{
	if (Succeeded && SessionSearch.IsValid())
	{
		//TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp,Warning,TEXT("Search: %d"),SessionSearch->SearchResults.Num());
		FString texty = FString::FromInt(SessionSearch->SearchResults.Num());
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Search: "+texty));
		
		if(SessionSearch->SearchResults.Num()>0) 
		{
			SessionInterface->JoinSession(0,"My Session",SessionSearch->SearchResults[0]);
		}
	}
	//else UE_LOG(LogTemp,Warning,TEXT("AAAAAAAAAA"));
}

void USGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(),0))
	{
		FString JoinAddress= "";
		SessionInterface->GetResolvedConnectString(SessionName,JoinAddress);
		if(JoinAddress != "") PController->ClientTravel(JoinAddress,ETravelType::TRAVEL_Absolute);
	}
}

void USGameInstance::CreateServer()
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch= true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 6;

	SessionInterface->CreateSession(0, FName("My Session"),SessionSettings);
}

void USGameInstance::JoinServer()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if(SessionSearch.IsValid())
	{
		SessionSearch->bIsLanQuery = true; //ISLAN
		SessionSearch->MaxSearchResults = 10000;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);

		TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();
		SessionInterface->FindSessions(0, SearchSettingsRef);
	}
}

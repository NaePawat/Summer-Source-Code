// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameStateBase.h"
#include "EngineUtils.h"
#include "Components/SRole.h"
#include "SGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

//set default value
ASGameStateBase::ASGameStateBase()
{
	SetReplicates(true);
}


void ASGameStateBase::StartWorldTimer_Implementation()
{
	UWorld* const World = GetWorld();
	if (World != nullptr) GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ASGameStateBase::EndGame, 1.0f, false, GameTime);
}


void ASGameStateBase::EndGame()
{
	UE_LOG(LogTemp, Error, TEXT("Time's Up BRUH!"));
	EndGameResult(false);
}

void ASGameStateBase::EndGameResult_Implementation(bool Result)
{
	bWinResult = Result;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if (PC)
		{
			ASCharacter* PlayerPawn = Cast<ASCharacter>(PC->GetPawn());
			if(PlayerPawn)
			{
				UCharacterMovementComponent* PlayerMovementComp = Cast<UCharacterMovementComponent>(PlayerPawn->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
				if(PlayerMovementComp)
				{
					PlayerMovementComp->DisableMovement();
				}
			}
		}
	}
	if(Result)
	{
		UE_LOG(LogTemp, Error, TEXT("The survivors win!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The hunters win!"));
	}
	TravelTo();
}

void ASGameStateBase::SetIsNightTime(bool Value)
{
	bIsNightTime = Value;
	OnRep_IsNightTime();
}

void ASGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(GetLocalRole()==ROLE_Authority) GameTimeRemaining = GetWorldTimerManager().GetTimerRemaining(GameTimerHandle);
}

void ASGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameStateBase, GameTimeRemaining);
	DOREPLIFETIME(ASGameStateBase, GoodGuyCount);
	DOREPLIFETIME(ASGameStateBase, BadGuyCount);
	DOREPLIFETIME(ASGameStateBase, bIsNightTime);
	DOREPLIFETIME(ASGameStateBase, bWinResult);
	
}
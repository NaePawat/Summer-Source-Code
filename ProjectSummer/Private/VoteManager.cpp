// Fill out your copyright notice in the Description page of Project Settings.


#include "VoteManager.h"

#include "Components/CapsuleComponent.h"
#include "Components/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AVoteManager::AVoteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

}

// Called when the game starts or when spawned
void AVoteManager::BeginPlay()
{
	Super::BeginPlay();
	bOngoingVote = false;
}

void AVoteManager::SetVoteTimer()
{
	if(GetLocalRole()==ROLE_Authority)
	{
		VoteBeginWidgetHandle();
		bOngoingVote = true;
		UWorld* const World = GetWorld();
		
		TArray<AActor*> FoundPlayers;
		int32 Count = 0;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASCharacter::StaticClass(), FoundPlayers);
		for(auto Player : FoundPlayers)
		{
			if(ASCharacter* CastedPlayer = Cast<ASCharacter>(Player))
			{
				if(CastedPlayer->bIsAlive)
				{
					Count++;
				}
			}
		}
		PlayerLength = Count - BannedCharacters.Num();
		if (World != nullptr) GetWorldTimerManager().SetTimer(VoteTimerHandle, this, &AVoteManager::EndVote, 1.0f, false, VoteTime);
	}
}

void AVoteManager::OnInteract(ACharacter* Character)
{
}

void AVoteManager::VoteBeginWidgetHandle_Implementation()
{
	VoteBeginWidgetHandleEvent();
}

void AVoteManager::VoteEndWidgetHandle_Implementation()
{
	VoteEndWidgetHandleEvent();
}

void AVoteManager::EndVote()
{
	if(GetLocalRole()==ROLE_Authority)
	{
		VoteEndWidgetHandle();
		if(VoteInitializer)
		{
			VoteInitializer = nullptr;
		}
		if(VoteTarget)
		{
			//UE_LOG(LogTemp, Error, TEXT("all/2:%f"), PlayerLength/(double)2)
			
			if(Yes >= PlayerLength/(double)2)
			{
				VoteTarget->bCanUseSafeRoom = false;
				VoteTarget->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
				BannedCharacters.Add(VoteTarget);
				KickBanGuy(VoteTarget);
			} else
			{
				VoteTarget->bCanUseSafeRoom = true;
			}
			bOngoingVote = false;
			VoteTarget = nullptr;
			Yes = 0;
			No = 0;
		}
	}
}

// Called every frame
void AVoteManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(GetLocalRole()==ROLE_Authority) VoteTimeRemaining = GetWorldTimerManager().GetTimerRemaining(VoteTimerHandle);
	//if(GetLocalRole()==ROLE_Authority) UE_LOG(LogTemp, Error, TEXT("all:%d"), PlayerLength);
}

void AVoteManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVoteManager, bOngoingVote);
	DOREPLIFETIME(AVoteManager, VoteTimeRemaining);
	DOREPLIFETIME(AVoteManager, VoteTarget);
	DOREPLIFETIME(AVoteManager, VoteInitializer);
	DOREPLIFETIME(AVoteManager, Yes);
	DOREPLIFETIME(AVoteManager, No);
	DOREPLIFETIME(AVoteManager, BannedCharacters);
	DOREPLIFETIME(AVoteManager, PlayerLength);
}
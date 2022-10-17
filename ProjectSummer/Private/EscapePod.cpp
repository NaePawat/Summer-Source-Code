// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapePod.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEscapePod::AEscapePod()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SubmitCount = 0;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AEscapePod::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEscapePod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AEscapePod::SubmitIngredient(TSubclassOf<UInventoryItem> SubmittedIngredient,ASCharacter* Submitter)
{
	int Index = 0;
	int SearchedIndex = -1;
	bool Alldone = true;
	for(auto Ingredient : Ingredients)
	{
		if(Ingredient == SubmittedIngredient)
		{
			SearchedIndex = Index;
			break;
		}
		Index++;
	}
	
	if(SearchedIndex != -1)
	{
		if(Quantities[SearchedIndex] > 0)
		{
			if(UInventoryItem* ItemInInventory = Submitter->PlayerInventory->FindInventoryItemByClass(SubmittedIngredient))
			{
				Submitter->PlayerInventory->ConsumeItem(ItemInInventory, 1);
				Quantities[SearchedIndex] = Quantities[SearchedIndex]-1;
				SubmitCount++;
				for(auto i:Quantities)
				{
					if (Quantities[i]!=0) Alldone = false;
				}
				if(Alldone && (SubmitCount == 4))
				{
					Submitter->SendEndGameToGameState();
				}
				if(Submitter != UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
				{
					bQuantitiesDone = true;
					bInventoryDone = true;
					OnRep_Quantities();
					UE_LOG(LogTemp, Error, TEXT("Hi"));
				}
				return true;
			}
		}
	}
	return false;
}

void AEscapePod::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEscapePod, Ingredients);
	DOREPLIFETIME(AEscapePod, Quantities);
}

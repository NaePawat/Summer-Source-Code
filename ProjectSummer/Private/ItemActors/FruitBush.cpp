// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActors/FruitBush.h"

#include "SCharacter.h"
#include "SDecoyCharacter.h"
#include "Components/InventoryComponent.h"

AFruitBush::AFruitBush()
{
	FruitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FruitMesh"));
	FruitMesh->SetupAttachment(RootComponent);
	FruitMesh->SetVisibility(false);
}

void AFruitBush::OnInteract(ACharacter* Character)
{
	Super::OnInteract(Character);
	
	if(bFruit)
	{
		TakeFruit(Character);
	}
}

bool AFruitBush::TakeFruit(ACharacter* Character)
{
	if(Character->HasAuthority())
	{
		if(FruitInventoryItem)
		{
			FInventoryItemAddResult AddResult;
			if(ASCharacter* SCharacter = Cast<ASCharacter>(Character))
			{
				AddResult = SCharacter->PlayerInventory->TryAddInventoryItem(FruitInventoryItem);
			}
			else if(ASDecoyCharacter* DCharacter = Cast<ASDecoyCharacter>(Character))
			{
				if(ASCharacter* OCharacter = Cast<ASCharacter>(DCharacter->OwnerCharacter))
				{
					AddResult = OCharacter->PlayerInventory->TryAddInventoryItem(FruitInventoryItem);
				}
			}
			
			if(AddResult.Result == EInventoryItemAddResult::IAR_AllItemsAdded)
			{
				RemoveFruit();
			}
			else if(AddResult.Result == EInventoryItemAddResult::IAR_NoItemsAdded)
			{
				return false;
			}
			GetWorldTimerManager().SetTimer(TimerHandle_FruitRespawn, this,
				&AFruitBush::RespawnFruit, FMath::RandRange(FruitMinRespawnTime,FruitMaxRespawnTime), false);
			return true;
		}
	}
	return false;
}

void AFruitBush::RemoveFruit()
{
	bFruit = false;
	SetFruitVisibility(false);
}

void AFruitBush::RespawnFruit()
{
	bFruit = true;
	FruitInventoryItem = NewObject<UInventoryItem>(this, FruitType);
	SetFruitVisibility(true);
}

void AFruitBush::SetFruitVisibility_Implementation(bool Value)
{
	FruitMesh->SetVisibility(Value);
}

void AFruitBush::BeginPlay()
{
	Super::BeginPlay();
	RespawnFruit();
}
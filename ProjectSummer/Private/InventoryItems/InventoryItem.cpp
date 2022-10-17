// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/InventoryItem.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "InventoryItem"

void UInventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItem, Quantity);
	DOREPLIFETIME(UInventoryItem, Capacity);
	DOREPLIFETIME(UInventoryItem, Volume);
	DOREPLIFETIME(UInventoryItem, OwningInventory);
}

bool UInventoryItem::IsSupportedForNetworking() const
{
	return true;
}

#if WITH_EDITOR
void UInventoryItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if(ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UInventoryItem, Quantity))
	{
		Quantity = FMath::Clamp(Quantity, 1, bStackable ? MaxStackSize : 1);
	}
}
#endif

UInventoryItem::UInventoryItem()
{
	ItemDisplayName = LOCTEXT("ItemName", "Item");
	UseActionText = LOCTEXT("ItemUseActionText", "Use");
	bStackable = true;
	Quantity = 1;
	MaxStackSize = 2;
	RepKey = 0;
	bCapacity = false;
}

void UInventoryItem::OnRep_Item()
{
	OnInventoryItemModified.Broadcast();
	//UE_LOG(LogTemp, Error, TEXT("umeh"));
	if(OwningInventory)
	{
		//UE_LOG(LogTemp, Error, TEXT("umeh2"));
		if(ASCharacter* Character = Cast<ASCharacter>(OwningInventory->GetOwner()))
		{
			//UE_LOG(LogTemp, Error, TEXT("umeh3"));
			if(Character->PendingCraftWidget.Num() != 0)
			{
				Character->PostCraftUpdate();
			}

			if(Character->InteractingEscapePod)
			{
				Character->InteractingEscapePod->InventoryHasModified();
			}
		}
	}
}

void UInventoryItem::SetQuantity(const int32 NewQuantity)
{
	if(NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, bStackable ? MaxStackSize : 1);
		MarkDirtyForReplication();
	}
}

void UInventoryItem::SetCapacity(const float NewCapacity)
{
	if(NewCapacity != Capacity)
	{
		Capacity = NewCapacity;
		MarkDirtyForReplication();
	}
}

void UInventoryItem::SetAmount(const float NewAmount)
{
	if(NewAmount != Volume)
	{
		Volume = NewAmount;
		MarkDirtyForReplication();
	}
}

bool UInventoryItem::ShouldShowInInventory() const
{
	return true;
}

void UInventoryItem::MarkDirtyForReplication()
{
	++RepKey;
	if(OwningInventory)
	{
		++OwningInventory->ReplicatedInventoryItemsKey;
	}
}

void UInventoryItem::AddedToInventory(UInventoryComponent* Inventory)
{
}

#undef LOCTEXT_NAMESPACE

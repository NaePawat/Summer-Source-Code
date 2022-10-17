// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/InventoryUpgradeInventoryItem.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"

void UInventoryUpgradeInventoryItem::Use(ASCharacter* Character)
{
	if(Character)
	{
		UInventoryComponent* Inventory = Character->PlayerInventory;
		if(Inventory->GetCapacity() != NewCapacity)
		{
			Inventory->SetCapacity(NewCapacity);
			Inventory->ConsumeItem(this);
		}
	}
}

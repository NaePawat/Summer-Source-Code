// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/HealInventoryItem.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "FoodItem"

UHealInventoryItem::UHealInventoryItem()
{
	HealAmount = 20.f;
	UseActionText = LOCTEXT("ItemUseActionText", "Consume");
}

void UHealInventoryItem::Use(ASCharacter* Character)
{
	OnUse(Character);
	Character->PlayerInventory->ConsumeItem(this, 1);
}

#undef LOCTEXT_NAMESPACE
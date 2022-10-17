// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/EquipableInventoryItem.h"

#include "SCharacter.h"

void UEquipableInventoryItem::Equip(ASCharacter* Character)
{
	Character->BeginEquipAnimationTrigger();
	Character->bEquipping = true;
}

void UEquipableInventoryItem::UnEquip(ASCharacter* Character)
{
	Character->EndEquipAnimationTrigger();
	Character->bEquipping = false;
}

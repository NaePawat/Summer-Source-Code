// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/invincibilityInventoryItem.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"

void UinvincibilityInventoryItem::Use(ASCharacter* Character)
{
	if(Character)
	{
		Character->ActivateInvincibility(EffectDuration);
		Character->PlayerInventory->ConsumeItem(this, 1);
	}
}

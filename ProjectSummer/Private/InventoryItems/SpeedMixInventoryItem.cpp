// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/SpeedMixInventoryItem.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"

void USpeedMixInventoryItem::Use(ASCharacter* Character)
{
	if(Character)
	{
		Character->ActiveSpeedItemBuff(SpeedMultiplier, EffectDuration);
		Character->PlayerInventory->ConsumeItem(this, 1);
	}
}

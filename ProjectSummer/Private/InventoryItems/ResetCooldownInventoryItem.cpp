// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/ResetCooldownInventoryItem.h"

#include "Components/InventoryComponent.h"
#include "SCharacter.h"

void UResetCooldownInventoryItem::Use(class ASCharacter* Character)
{
	if(Character)
	{
		Character->bCanUseAbility = true;
		Character->PlayerInventory->ConsumeItem(this, 1);
		//UE_LOG(LogTemp, Warning, TEXT("Use") );
	}
}
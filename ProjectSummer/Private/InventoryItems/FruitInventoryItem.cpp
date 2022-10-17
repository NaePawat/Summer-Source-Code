// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/FruitInventoryItem.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/SHealth.h"

void UFruitInventoryItem::Use(ASCharacter* Character)
{
	if(Character)
	{
		if(USHealth* SHealth = Cast<USHealth>(Character->GetComponentByClass(USHealth::StaticClass())))
		{
			if(bPercentHeal)
			{
				SHealth->Heal(SHealth->DefaultHealth * HealAmount);
			}
			else
			{
				SHealth->Heal(HealAmount);
			}
			Character->PlayerInventory->ConsumeItem(this, 1);
		}
	}
}
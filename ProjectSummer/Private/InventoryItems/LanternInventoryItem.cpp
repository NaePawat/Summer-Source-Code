// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/LanternInventoryItem.h"

#include "Lantern.h"
#include "SCharacter.h"
#include "Components/InventoryComponent.h"

void ULanternInventoryItem::Use(ASCharacter* Character)
{
	if(Character->HasAuthority())
	{
		if(!Character->bEquipLantern)
		{
			const FActorSpawnParameters SpawnParams;
			ALantern* Lantern = Character->GetWorld()->SpawnActor<ALantern>(LanternClass, Character->LanternSlot->GetComponentLocation(),
				Character->LanternSlot->GetComponentRotation(), SpawnParams);
			if(Lantern)
			{
				Lantern->AttachToActor(Character, FAttachmentTransformRules::KeepWorldTransform);
				Lantern->StartDestroyTimer(Character, LanternDurantion);
				Character->bEquipLantern = true;
				Character->PlayerInventory->ConsumeItem(this, 1);
			}
		}
	}
}

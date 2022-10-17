// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/PortableO2InventoryItem.h"

#include "SCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/SO2.h"

UPortableO2InventoryItem::UPortableO2InventoryItem()
{
	bCapacity = true;
}

void UPortableO2InventoryItem::Use(ASCharacter* Character)
{
	OnUse(Character);
	/*
	doesnt work because o2 is in bp... so gotta do it in bp
	if(Character)
	{
		if(USO2* O2 = Cast<USO2>(Character))
		{
			const float Consume = Character->PlayerInventory->ConsumeAmount(this, O2->RefillO2(Amount));
			UE_LOG(LogTemp, Error, TEXT("%f"), Consume)
		}
		UE_LOG(LogTemp, Error, TEXT("Use") );
	}
	*/
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems/GunInventoryItem.h"

#include "Gun.h"
#include "SCharacter.h"
#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

void UGunInventoryItem::Use(ASCharacter* Character)
{
	if(Character)
	{
		if(Character->Equipping)
		{
			UnEquip(Character);
		}
		else
		{
			Equip(Character);
		}
	}
}

void UGunInventoryItem::SetGunClass(const TSubclassOf<AGun> NewGunClass)
{
	if(NewGunClass != GunClass)
	{
		GunClass = NewGunClass;
	}
}

void UGunInventoryItem::Equip(ASCharacter* Character)
{
	Super::Equip(Character);
	if(Character->HasAuthority())
	{
		if(Character->GetWorld())
		{
			FTransform SpawnTransform = Character->GetActorTransform();
			SpawnTransform.SetLocation(Character->EquipableSlot->GetComponentLocation());
			FActorSpawnParameters SpawnParams;
			//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepWorld,
				EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
		
			Character->Equipping = Character->GetWorld()->SpawnActor<AGun>(GunClass, SpawnTransform, SpawnParams);
			Character->Equipping->AttachToActor(Character, AttachmentTransformRules);
			Character->EquippingInventoryItem = this;
		}
	}
}

void UGunInventoryItem::UnEquip(ASCharacter* Character)
{
	Super::UnEquip(Character);
	UE_LOG(LogTemp, Error, TEXT("%s"), Character->HasAuthority() ? TEXT("true") : TEXT("false"));
	if(Character->HasAuthority())
	{
		if(Character->Equipping)
		{
			Character->Equipping->Destroy();
			Character->Equipping = nullptr;
			Character->EquippingInventoryItem = nullptr;
		}
	}
}

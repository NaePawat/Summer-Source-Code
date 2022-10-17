// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "EquipableInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UEquipableInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class ASCharacter* EquippedBy;
	
	UFUNCTION(BlueprintCallable)
	virtual void Equip(class ASCharacter* Character);

	UFUNCTION(BlueprintCallable)
	virtual void UnEquip(class ASCharacter* Character);
};

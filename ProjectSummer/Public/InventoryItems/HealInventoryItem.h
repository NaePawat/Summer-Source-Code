// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "HealInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UHealInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:

	UHealInventoryItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Healing")
	float HealAmount;

	virtual void Use(class ASCharacter* Character) override;
	
};

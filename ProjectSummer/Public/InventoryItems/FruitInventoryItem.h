// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "FruitInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UFruitInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	float HealAmount = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool bPercentHeal;
	
	virtual void Use(ASCharacter* Character) override;
};

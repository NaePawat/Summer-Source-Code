// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "InventoryUpgradeInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UInventoryUpgradeInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NewCapacity;
	
protected:
	UFUNCTION()
	virtual void Use(class ASCharacter* Character) override;
};

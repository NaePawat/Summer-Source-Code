// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "MaterialInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UMaterialInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	virtual void Use(class ASCharacter* Character) override;
};

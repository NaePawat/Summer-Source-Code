// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "PortableO2InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UPortableO2InventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:	

	UPortableO2InventoryItem();
	
protected:

	UFUNCTION()
	virtual void Use(class ASCharacter* Character) override;
};

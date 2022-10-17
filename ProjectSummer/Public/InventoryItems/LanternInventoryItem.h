// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "LanternInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API ULanternInventoryItem : public UInventoryItem
{
	GENERATED_BODY()
	
public:

    virtual void Use(class ASCharacter* Character) override;

	UPROPERTY(EditDefaultsOnly, Category="Lantern")
	TSubclassOf<class ALantern> LanternClass;

	UPROPERTY(EditDefaultsOnly, Category="Lantern")
	float LanternDurantion;
};

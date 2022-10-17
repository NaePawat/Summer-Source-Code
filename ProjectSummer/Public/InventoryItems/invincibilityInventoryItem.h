// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "invincibilityInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UinvincibilityInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	float EffectDuration = 5;
	
	virtual void Use(ASCharacter* Character) override;
	
};

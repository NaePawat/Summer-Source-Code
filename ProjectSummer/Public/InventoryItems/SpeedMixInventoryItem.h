// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/InventoryItem.h"
#include "SpeedMixInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API USpeedMixInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	float SpeedMultiplier = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float EffectDuration = 5;
	
	virtual void Use(ASCharacter* Character) override;
};

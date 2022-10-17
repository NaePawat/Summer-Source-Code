// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "ResetCooldownInventoryItem.generated.h"

/**
 *
 */
UCLASS()
class PROJECTSUMMER_API UResetCooldownInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

	
public:

protected:
	UFUNCTION()
	virtual void Use(class ASCharacter* Character) override;

};

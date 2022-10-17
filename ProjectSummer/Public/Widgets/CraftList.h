// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Craft.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItems/InventoryItem.h"
#include "CraftList.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTSUMMER_API UCraftList : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FCraftInstruction> InstructionsList;

	UPROPERTY(BlueprintReadWrite)
	TArray<UUserWidget*> CraftWidgets;
};

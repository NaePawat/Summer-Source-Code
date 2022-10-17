// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItems/InventoryItem.h"
#include "Craft.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCraftInstruction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<TSubclassOf<UInventoryItem>, int32> IngredientsList;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<TSubclassOf<UInventoryItem>, int32> OutputList;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText InstructionDisplayName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText InstructionDescription;
	
};

UCLASS()
class PROJECTSUMMER_API UCraft : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FCraftInstruction CraftInstruction;

	UFUNCTION(BlueprintImplementableEvent)
	void PostCraftEvent();
};

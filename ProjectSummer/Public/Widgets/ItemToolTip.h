// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "ToolTip", meta = (ExposeOnSpawn = true))
	class UInventoryItemWidget* InventoryItemWidget;
	
};

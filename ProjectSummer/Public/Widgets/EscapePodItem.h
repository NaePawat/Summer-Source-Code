// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscapePodItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UEscapePodItem : public UUserWidget
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintImplementableEvent)
    void PostInsertEvent();
	
	
};

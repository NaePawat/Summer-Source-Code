// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheatWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UOverheatWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void Overheating();

	UFUNCTION(BlueprintImplementableEvent)
	void Cool();

	UPROPERTY(BlueprintReadOnly)
	class UOverheatWidgetComponent* OwningOverheatWidgetComponent;
	
	void Init(UOverheatWidgetComponent* OverheatWidgetComponent);
	
};

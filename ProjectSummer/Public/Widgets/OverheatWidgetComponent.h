// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "OverheatWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UOverheatWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	
	void Overheating();
	
	void Cool();
	
	void UpdateHeatWidget(float NewHeat, float NewHeatThreshold);
	
	void UpdateHeatWidget(float NewHeat);

	UPROPERTY(BlueprintReadWrite)
	float Heat;

	UPROPERTY(BlueprintReadOnly)
	float HeatThreshold;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OverheatWidgetComponent.h"

#include "Net/UnrealNetwork.h"
#include "Widgets/OverheatWidget.h"

class UOverheatWidget;

void UOverheatWidgetComponent::Overheating()
{
	if(UOverheatWidget* OverheatWidget = Cast<UOverheatWidget>(GetUserWidgetObject()))
	{
		OverheatWidget->Overheating();
	}
}

void UOverheatWidgetComponent::Cool()
{
	if(UOverheatWidget* OverheatWidget = Cast<UOverheatWidget>(GetUserWidgetObject()))
	{
		OverheatWidget->Cool();
	}
}

void UOverheatWidgetComponent::UpdateHeatWidget(float NewHeat, float NewHeatThreshold)
{
	HeatThreshold = NewHeatThreshold;
	UpdateHeatWidget(NewHeat);
}

void UOverheatWidgetComponent::UpdateHeatWidget(float NewHeat)
{
	Heat = NewHeat;
	if(UOverheatWidget* OverheatWidget = Cast<UOverheatWidget>(GetUserWidgetObject()))
	{
		if(!OverheatWidget->OwningOverheatWidgetComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("HI"));
			OverheatWidget->Init(this);
		}
	}
}
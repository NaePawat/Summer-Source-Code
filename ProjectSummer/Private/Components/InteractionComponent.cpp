// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractionComponent.h"

#include "Widgets/InteractionWidget.h"
#include "SCharacter.h"
#include "SDecoyCharacter.h"

UInteractionComponent::UInteractionComponent()
{
	SetComponentTickEnabled(false);

	InteractionTime = 0.f;
	InteractionDistance = 200.f;
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");
	bAllowMultipleInteractors = true;

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(400,100);
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true);
}

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableNameText = NewNameText;
	RefreshWidget();
}

void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	RefreshWidget();
}

void UInteractionComponent::Deactivate()
{
	Super::Deactivate();

	for(int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if(ACharacter* Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}

	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(ACharacter* Character) const
{
	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() >= 1;
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

float UInteractionComponent::GetInteractPercentage()
{
	if(Interactors.IsValidIndex(0))
	{
		if(ACharacter* Interactor = Interactors[0])
		{
			if(ASCharacter* SCharacter = Cast<ASCharacter>(Interactor))
			{
				if(SCharacter && SCharacter->IsInteracting())
				{
					return 1.f - FMath::Abs(SCharacter->GetRemainingInteractTime() / InteractionTime);
				}
			}
			else if(ASDecoyCharacter* DCharacter = Cast<ASDecoyCharacter>(Interactor))
			{
				if(DCharacter && DCharacter->IsInteracting())
				{
					return 1.f - FMath::Abs(DCharacter->GetRemainingInteractTime() / InteractionTime);
				}
			}
		}
	}
	return 0.f;
}

void UInteractionComponent::RefreshWidget()
{
	//if(!bHiddenInGame && GetOwner()->GetNetMode() != NM_DedicatedServer)
	if(!bHiddenInGame)
	{
		if(UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
		{
			InteractionWidget->UpdateInteractionWidget(this);
		}
	}
}

void UInteractionComponent::BeginFocus(ACharacter* Character)
{
	if(!IsActive() || !GetOwner() || !Character)
	{
		return;
	}

	OnBeginFocus.Broadcast(Character);

	if(Character->Controller)
	{
		if(Character->Controller->IsLocalController())
		{
			SetHiddenInGame(false);
		}
	}

    /* for outline
	if (!GetOwner()->HasAuthority())
	{
		for(auto& VisualComp:GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			if(UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(true);
			}
		}
	}
	*/

	RefreshWidget();
}

void UInteractionComponent::EndFocus(ACharacter* Character)
{
	OnEndFocus.Broadcast(Character);

	if(Character->Controller)
	{
		if(Character->Controller->IsLocalController())
		{
			SetHiddenInGame(true);
		}
	}

	/*for outline
	if (!GetOwner()->HasAuthority())
	{
		for(auto& VisualComp:GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			if(UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(false);
			}
		}
	}
	*/
}

void UInteractionComponent::BeginInteract(ACharacter* Character)
{
	if(CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(ACharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(ACharacter* Character)
{
	if(CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"

#include "SCharacter.h"
#include "Engine/ActorChannel.h"
#include "InventoryItems/EquipableInventoryItem.h"
#include "InventoryItems/InventoryItem.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "Inventory"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	Capacity = 20;
	SetIsReplicatedByDefault(true);
}

FInventoryItemAddResult UInventoryComponent::TryAddInventoryItem(UInventoryItem* InventoryItem)
{
	return TryAddInventoryItem_Internal(InventoryItem);
}

FInventoryItemAddResult UInventoryComponent::TryAddInventoryItemFromClass(
	TSubclassOf<UInventoryItem> InventoryItemClass, const int32 Quantity)
{
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>(GetOwner(), InventoryItemClass);
	InventoryItem->SetQuantity(Quantity);
	return TryAddInventoryItem_Internal(InventoryItem);
}

int32 UInventoryComponent::ConsumeItem(UInventoryItem* InventoryItem)
{
	if(InventoryItem)
	{
		ConsumeItem(InventoryItem, InventoryItem->GetQuantity());
	}
	return 0;
}

int32 UInventoryComponent::ConsumeItem(UInventoryItem* InventoryItem, const int32 Quantity)
{
	if(GetOwner() && GetOwner()->HasAuthority() && InventoryItem)
	{
		const int32 RemoveQuantity = FMath::Min(Quantity, InventoryItem->GetQuantity());
		ensure(!(InventoryItem->GetQuantity() - RemoveQuantity < 0));
		InventoryItem->SetQuantity(InventoryItem->GetQuantity() - RemoveQuantity);

		if(InventoryItem->GetQuantity() <= 0)
		{
			RemoveInventoryItem(InventoryItem);
		}
		RefreshInventory();
		return RemoveQuantity;
	}
	return 0;
}

float UInventoryComponent::ConsumeAmount(UInventoryItem* InventoryItem, const float Amount)
{
	if(GetOwner() && GetOwner()->HasAuthority() && InventoryItem)
	{
		const float RemoveAmount = FMath::Min(Amount, InventoryItem->GetAmount());
		ensure(!(InventoryItem->GetAmount() - RemoveAmount < 0));
		InventoryItem->SetAmount(InventoryItem->GetAmount() - RemoveAmount);
		RefreshInventory();
		return RemoveAmount;
	}
	return 0;
}

bool UInventoryComponent::RemoveInventoryItem(UInventoryItem* InventoryItem)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(InventoryItem)
		{
			if(UEquipableInventoryItem* EquipableInventoryItem = Cast<UEquipableInventoryItem>(InventoryItem))
			{
				RemoveEquipable(EquipableInventoryItem);
			}
			InventoryItems.RemoveSingle(InventoryItem);
			ReplicatedInventoryItemsKey++;
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::RemoveEquipable(UEquipableInventoryItem* Equipable)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(Equipable)
		{
			EquipableList.RemoveSingle(Equipable);
			ReplicatedInventoryItemsKey++;
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::HasInventoryItem(TSubclassOf<UInventoryItem> InventoryItemClass, const int32 Quantity) const
{
	if(UInventoryItem* InventoryItemToFind = FindInventoryItemByClass(InventoryItemClass))
	{
		return InventoryItemToFind->GetQuantity() >= Quantity;
	}
	return false;
}

UInventoryItem* UInventoryComponent::FindInventoryItem(UInventoryItem* InventoryItem) const
{
	for(auto& InvItem : InventoryItems)
	{
		if(InvItem && InvItem->GetClass() == InventoryItem->GetClass())
		{
			return InvItem;
		}
	}
	return nullptr;
}

UInventoryItem* UInventoryComponent::FindInventoryItemByClass(TSubclassOf<UInventoryItem> InventoryItemClass) const
{
	for(auto& InvItem : InventoryItems)
	{
		if(InvItem && InvItem->GetClass() == InventoryItemClass)
		{
			return InvItem;
		}
	}
	return nullptr;
}

TArray<UInventoryItem*> UInventoryComponent::FindInventoryItemsByClass(
	TSubclassOf<UInventoryItem> InventoryItemClass) const
{
	TArray<UInventoryItem*> InventoryItemsOfClass;
	for(auto& InvItem : InventoryItems)
	{
		if(InvItem && InvItem->GetClass() == InventoryItemClass)
		{
			InventoryItemsOfClass.Add(InvItem);
		}
	}
	return InventoryItemsOfClass;
}

void UInventoryComponent::SetCapacity(const int32 NewCapacity)
{
	Capacity = NewCapacity;
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::RefreshInventory_Implementation()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryItems);
	DOREPLIFETIME(UInventoryComponent, EquipableList);
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if(Channel->KeyNeedsToReplicate(0, ReplicatedInventoryItemsKey))
	{
		for(auto& InventoryItem:InventoryItems)
		{
			if(Channel->KeyNeedsToReplicate(InventoryItem->GetUniqueID(), InventoryItem->RepKey))
			{
				bWroteSomething |= Channel->ReplicateSubobject(InventoryItem, *Bunch, *RepFlags);
			}
		}
	}
	return bWroteSomething;
}

UInventoryItem* UInventoryComponent::AddInventoryItem(UInventoryItem* InventoryItem)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		UInventoryItem* NewInventoryItem = NewObject<UInventoryItem>(GetOwner(), InventoryItem->GetClass());
		NewInventoryItem->SetQuantity(InventoryItem->GetQuantity());
		NewInventoryItem->SetCapacity(InventoryItem->GetCapacity());
		NewInventoryItem->SetAmount(InventoryItem->GetAmount());
		NewInventoryItem->OwningInventory = this;
		NewInventoryItem->AddedToInventory(this);
		InventoryItems.Add(NewInventoryItem);
		RefreshInventory();

		if(UEquipableInventoryItem* EquipableInventoryItem = Cast<UEquipableInventoryItem>(NewInventoryItem))
		{
			AddEquipableInventoryItem(EquipableInventoryItem);
		}
		
		NewInventoryItem->MarkDirtyForReplication();

		return NewInventoryItem;
	}
	return nullptr;
}

UEquipableInventoryItem* UInventoryComponent::AddEquipableInventoryItem(UEquipableInventoryItem* Equipable)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		EquipableList.Add(Equipable);
		return Equipable;
	}
	return nullptr;
}

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();
	if(ASCharacter* Character = Cast<ASCharacter>(GetOwner()))
	{
		if(Character->PendingCraftWidget.Num() != 0)
		{
			Character->PostCraftUpdate();
		}

		if(Character->InteractingEscapePod)
		{
			Character->InteractingEscapePod->InventoryHasModified();
		}
	}
}

FInventoryItemAddResult UInventoryComponent::TryAddInventoryItem_Internal(UInventoryItem* InventoryItem)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		const int32 AddAmount = InventoryItem->GetQuantity();
		
		if(InventoryItem->bStackable)
		{
			ensure(InventoryItem->GetQuantity() <= InventoryItem->MaxStackSize);

			TArray<UInventoryItem*> ExistingInventoryItems = FindInventoryItemsByClass(InventoryItem->GetClass());
			if(ExistingInventoryItems.Num() > 0)
			{
				int32 ActualAddAmount = 0;
				//for every stack of this item in inventory
				for(int i = 0; i < ExistingInventoryItems.Num(); i++)
				{
					//if not max stack
					if(ExistingInventoryItems[i]->GetQuantity() < ExistingInventoryItems[i]->MaxStackSize)
					{
						const int32 CapacityMaxAddAmount = ExistingInventoryItems[i]->MaxStackSize - ExistingInventoryItems[i]->GetQuantity();
						const int32 ActualAddAmountForStack = FMath::Min(AddAmount, CapacityMaxAddAmount);
						ActualAddAmount = ActualAddAmount + ActualAddAmountForStack;

						ExistingInventoryItems[i]->SetQuantity(ExistingInventoryItems[i]->GetQuantity() + ActualAddAmountForStack);
						ensure(ExistingInventoryItems[i]->GetQuantity() <= ExistingInventoryItems[i]->MaxStackSize);

						//Can add all in this stack
						if(ActualAddAmount == AddAmount)
						{
							//UE_LOG(LogTemp, Error, TEXT("existing stack fully add"));
							return FInventoryItemAddResult::AddedAll(AddAmount);
						}
					}
				}
				if(InventoryItems.Num() + 1 > GetCapacity())
				{
					//UE_LOG(LogTemp, Error, TEXT("there is existing stack but cant add anymore %d"), ActualAddAmount);
					return FInventoryItemAddResult::AddedSome(AddAmount, ActualAddAmount,
						FText::Format(LOCTEXT("InventoryCapacityFullText",
						"Coundn't add entire stack of {ItemName} to inventory."),
						InventoryItem->ItemDisplayName));
				}
				InventoryItem->SetQuantity(InventoryItem->GetQuantity() - ActualAddAmount);
				AddInventoryItem(InventoryItem);
				//UE_LOG(LogTemp, Error, TEXT("there is existing stack + new stack"));
				return FInventoryItemAddResult::AddedAll(AddAmount);
			}
			//no existing stack
			if(InventoryItems.Num() + 1 > GetCapacity())
			{
				//UE_LOG(LogTemp, Error, TEXT("no existing stack + cant add"));
				return FInventoryItemAddResult::AddedNone(AddAmount,
					LOCTEXT("InventoryCapacityFullText", "Coundn't add item to inventory, inventory is full."));
			}
			AddInventoryItem(InventoryItem);
			//UE_LOG(LogTemp, Error, TEXT("no existing stack + new stack"));
			return FInventoryItemAddResult::AddedAll(AddAmount);
		}

		if(InventoryItems.Num() + 1 > GetCapacity())
		{
			//UE_LOG(LogTemp, Error, TEXT("full, cant add"));
			return FInventoryItemAddResult::AddedNone(AddAmount,
				LOCTEXT("InventoryCapacityFullText", "Coundn't add item to inventory, inventory is full."));
		}
		
		ensure(InventoryItem->GetQuantity() == 1);
		AddInventoryItem(InventoryItem);
		//UE_LOG(LogTemp, Error, TEXT("add all"));
		return FInventoryItemAddResult::AddedAll(AddAmount);
	}
	check(false);
	return FInventoryItemAddResult::AddedNone(-1, LOCTEXT("ErrorMessage", ""));
}

#undef LOCTEXT_NAMESPACE
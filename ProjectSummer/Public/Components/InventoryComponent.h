// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

//called when the inventory is changed and the ui need to be update.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UENUM(BlueprintType)
enum class EInventoryItemAddResult : uint8
{
	IAR_NoItemsAdded UMETA(DisplayName = "No item added"),
	IAR_SomeItemsAdded UMETA(DisplayName = "Some items added"),
	IAR_AllItemsAdded UMETA(DisplayName = "All items added")
};

USTRUCT(BlueprintType)
struct FInventoryItemAddResult
{
	GENERATED_BODY()
	
	FInventoryItemAddResult(){};
	FInventoryItemAddResult(int32 InItemQuantity) : AmountToGive(InItemQuantity), ActualAmountGiven(0) {};
	FInventoryItemAddResult(int32 InItemQuantity, int32 InQuantityAdded) : AmountToGive(InItemQuantity), ActualAmountGiven(InQuantityAdded) {};

	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem Add Result")
	int32 AmountToGive;

	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem Add Result")
	int32 ActualAmountGiven;

	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem Add Result")
	EInventoryItemAddResult Result;

	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem Add Result")
	FText ErrorText;

	static FInventoryItemAddResult AddedNone(const int32 InItemQuantity, const FText& ErrorText)
	{
		FInventoryItemAddResult AddedNoneResult(InItemQuantity);
		AddedNoneResult.Result = EInventoryItemAddResult::IAR_NoItemsAdded;
		AddedNoneResult.ErrorText = ErrorText;
		return AddedNoneResult;
	}
	static FInventoryItemAddResult AddedSome(const int32 InItemQuantity, const int32 ActualAmountGiven, const FText& ErrorText)
	{
		FInventoryItemAddResult AddedSomeResult(InItemQuantity, ActualAmountGiven);
		AddedSomeResult.Result = EInventoryItemAddResult::IAR_SomeItemsAdded;
		AddedSomeResult.ErrorText = ErrorText;
		return AddedSomeResult;
	}
	static FInventoryItemAddResult AddedAll(const int32 InItemQuantity)
	{
		FInventoryItemAddResult AddedAllResult(InItemQuantity, InItemQuantity);
		AddedAllResult.Result = EInventoryItemAddResult::IAR_AllItemsAdded;
		return AddedAllResult;
	}
	
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSUMMER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UInventoryItem;

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventoryItemAddResult TryAddInventoryItem(class UInventoryItem* InventoryItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventoryItemAddResult TryAddInventoryItemFromClass(TSubclassOf<class UInventoryItem> InventoryItemClass, const int32 Quantity);
	
	int32 ConsumeItem(class UInventoryItem* InventoryItem);

	UFUNCTION(BlueprintCallable)
	int32 ConsumeItem(class UInventoryItem* InventoryItem, const int32 Quantity);

	UFUNCTION(BlueprintCallable)
	float ConsumeAmount(class UInventoryItem* InventoryItem, const float Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveInventoryItem(class UInventoryItem* InventoryItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveEquipable(class UEquipableInventoryItem* Equipable);

	//Return true if we have a given amount of item
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasInventoryItem(TSubclassOf<class UInventoryItem> InventoryItemClass, const int32 Quantity = 1) const;

	//Return the first inventory item with the same class as a given item
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryItem* FindInventoryItem(class UInventoryItem* InventoryItem) const;

	//Return the first inventory item with the same class as InventoryItemClass
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryItem* FindInventoryItemByClass(TSubclassOf<class UInventoryItem> InventoryItemClass) const;
	
	//Return all the inventory item with the same class as InventoryItemClass
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UInventoryItem*> FindInventoryItemsByClass(TSubclassOf<class UInventoryItem> InventoryItemClass) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetCapacity(const int32 NewCapacity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE int32 GetCapacity() const { return Capacity; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE int32 GetCurrentCapacity() const { return Capacity - InventoryItems.Num(); }
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE TArray<class UInventoryItem*> GetInventoryItems() const { return InventoryItems; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE TArray<class UEquipableInventoryItem*> GetEquipableList() const { return EquipableList; }

	UFUNCTION(Server, Reliable)
	void RefreshInventory();
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0, ClampMax = 200))
	int32 Capacity;
	
	UPROPERTY(ReplicatedUsing = OnRep_Items, VisibleAnywhere, Category = "Inventory")
	TArray<class UInventoryItem*> InventoryItems;

	UPROPERTY(ReplicatedUsing = OnRep_Items, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<class UEquipableInventoryItem*> EquipableList;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

private:

	UFUNCTION()
	UInventoryItem* AddInventoryItem(class UInventoryItem* InventoryItem);

	UFUNCTION()
	UEquipableInventoryItem* AddEquipableInventoryItem(class UEquipableInventoryItem* Equipable);

	UFUNCTION()
	void OnRep_Items();

	UPROPERTY()
	int32 ReplicatedInventoryItemsKey;

	FInventoryItemAddResult TryAddInventoryItem_Internal(class UInventoryItem* InventoryItem);
	
	
};

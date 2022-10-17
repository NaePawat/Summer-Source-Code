// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryItemModified);

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	IR_Common UMETA(DisplayName = "Common"),
	IR_Rare UMETA(DisplayName = "Rare"),
	IR_Epic UMETA(DisplayName = "Epic"),
	IR_Legendary UMETA(DisplayName = "Legendary")
}; 

/**
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class PROJECTSUMMER_API UInventoryItem : public UObject
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> &OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:

	UInventoryItem();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
	class UWorld* World;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	class UStaticMesh* PickUpMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	FVector MeshScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	class UTexture2D* Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	FText ItemDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem", meta = (MultiLine = true))
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	FText UseActionText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	EItemRarity Rarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	bool bStackable;

	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	bool bCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem", meta = (ClampMin = 2, EditCondition = bStackable))
	int32 MaxStackSize;

	UPROPERTY(ReplicatedUsing = OnRep_Item, EditAnywhere, Category = "InventoryItem", meta = (UIMin = 1, EditCondition = bStackable))
	int32 Quantity;

	UPROPERTY(ReplicatedUsing = OnRep_Item, EditAnywhere, Category = "InventoryItem", meta = (UIMin = 1, EditCondition = bCapacity))
	float Capacity;

	UPROPERTY(ReplicatedUsing = OnRep_Item, EditAnywhere, Category = "InventoryItem", meta = (UIMin = 1, EditCondition = bCapacity))
	float Volume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventoryItem")
	TSubclassOf<class UItemToolTip> ItemToolTip;
	
	UPROPERTY(ReplicatedUsing = OnRep_Item, BlueprintReadOnly)
	class UInventoryComponent* OwningInventory;

	UPROPERTY()
	int32 RepKey;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemModified OnInventoryItemModified;

	UFUNCTION()
	void OnRep_Item();

	UFUNCTION()
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION()
	void SetCapacity(const float NewCapacity);

	UFUNCTION()
	void SetAmount(const float NewAmount);

	UFUNCTION(BlueprintPure, Category = "InventoryItem")
	FORCEINLINE int32 GetQuantity() const { return Quantity; }

	UFUNCTION(BlueprintPure, Category = "InventoryItem")
	FORCEINLINE float GetAmount() const { return Volume; }

	UFUNCTION(BlueprintPure, Category = "InventoryItem")
	FORCEINLINE float GetCapacity() const { return Capacity; }

	UFUNCTION(BlueprintPure, Category = "InventoryItem")
	virtual bool ShouldShowInInventory() const;

	void MarkDirtyForReplication();

	virtual void Use(class ASCharacter* Character) PURE_VIRTUAL(UItem, );
	virtual void AddedToInventory(class UInventoryComponent* Inventory);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class ASCharacter* Character);
};

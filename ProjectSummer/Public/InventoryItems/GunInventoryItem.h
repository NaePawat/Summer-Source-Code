// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipableInventoryItem.h"
#include "InventoryItems/InventoryItem.h"
#include "GunInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API UGunInventoryItem : public UEquipableInventoryItem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "InventoryItem")
	FORCEINLINE TSubclassOf<class AGun> GetGunClass() const { return GunClass; }

	UFUNCTION()
	void SetGunClass(const TSubclassOf<class AGun> NewGunClass);
	
	virtual void Equip(ASCharacter* Character) override;
	virtual void UnEquip(ASCharacter* Character) override;

protected:

	UFUNCTION()
	virtual void Use(class ASCharacter* Character) override;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TSubclassOf<class AGun> GunClass;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActors/Interactable.h"
#include "FruitBush.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API AFruitBush : public AInteractable
{
	GENERATED_BODY()

public:
	
	AFruitBush();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* FruitMesh;

	UPROPERTY(BlueprintReadWrite)
	bool bFruit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FruitMaxRespawnTime = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FruitMinRespawnTime = 0;

	FTimerHandle TimerHandle_FruitRespawn;
	
	virtual void OnInteract(ACharacter* Character) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UInventoryItem> FruitType;

	UPROPERTY()
	class UInventoryItem* FruitInventoryItem;
	
	UFUNCTION()
	bool TakeFruit(ACharacter* Character);

	UFUNCTION()
	void RemoveFruit();

	UFUNCTION()
	void RespawnFruit();

	UFUNCTION(NetMulticast, Reliable)
	void SetFruitVisibility(bool Value);

protected:

	virtual void BeginPlay() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ThirdParty/PhysX3/PxShared/src/foundation/include/PsArray.h>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItems/InventoryItem.h"
#include "ItemActors/Interactable.h"
#include "EscapePod.generated.h"

UCLASS()
class PROJECTSUMMER_API AEscapePod : public AInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEscapePod();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	int SubmitCount;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="IngredientsList")
	TArray<TSubclassOf<UInventoryItem>> Ingredients;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_Quantities, Category="IngredientsList")
	TArray<int> Quantities;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_Quantities();

	UFUNCTION(BlueprintImplementableEvent)
	void InventoryHasModified();

	UFUNCTION(BlueprintCallable)
	bool SubmitIngredient(TSubclassOf<UInventoryItem> SubmittedIngredient, ASCharacter* Submitter);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WidgetUpdateChecking")
	bool bQuantitiesDone = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WidgetUpdateChecking")
	bool bInventoryDone = true;
};

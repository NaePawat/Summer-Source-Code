// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Interactable.h"
#include "InventoryItems/GunInventoryItem.h"
#include "Pickable.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API APickable : public AActor
{
	GENERATED_BODY()

public:
	
	APickable();

    UFUNCTION(BlueprintCallable)
	void InitializePickable(const TSubclassOf<class UInventoryItem> InventoryItemClass,
		const int32 Quantity, const float Capacity, const float Amount);
	//for gun
	void InitializeGun(const TSubclassOf<class AGun> GunClass);

	UFUNCTION(BlueprintImplementableEvent)
	void AllignWithGround();
	
	UFUNCTION(BlueprintImplementableEvent)
	void Pickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	class UInventoryItem* InventoryItemTemplate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing = OnRep_Item)
	class UInventoryItem* InventoryItem;

	UPROPERTY(EditAnywhere)
	USceneComponent* _RootComponent;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* InteractionBox;
	
protected:

	

	UFUNCTION()
	void OnRep_Item();

	UFUNCTION()
	void OnItemModified();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION()
	void OnTakePickable(class ACharacter* Taker);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* PickableMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UInteractionComponent* InteractionComponent;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EscapePod.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UParticleSystem;

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
	{
		ViewdInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}

	//current interactable component that player looking
	UPROPERTY()
	class UInteractionComponent* ViewdInteractionComponent;

	//time when player last check for an interactable
	UPROPERTY()
	float LastInteractionCheckTime;

	//is player holding interact
	UPROPERTY()
	bool bInteractHeld;
};

UCLASS()
class PROJECTSUMMER_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	UFUNCTION(NetMulticast,Reliable)
	void CallVote();

	UFUNCTION(BlueprintImplementableEvent)
	void Vote();
	
	UFUNCTION(BlueprintCallable)
	void RequestVote(ASCharacter* Target);

	UFUNCTION(Server,Reliable)
	void ServerRequestVote(ASCharacter* Target);

	UFUNCTION(BlueprintImplementableEvent)
	void VoteInitializedEvent();

	void SetBCanUseAbility(bool Value);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bCanUseAbility;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Vote")
	bool bCanUseSafeRoom;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Invincibility")
	bool bIsInvincible;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Status")
	bool bIsAlive = true;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bEquipping = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* PlayerInventory;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Equipable")
	class AActor* Equipping = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable")
	class UEquipableInventoryItem* EquippingInventoryItem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable")
	class USceneComponent* EquipableSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lantern")
	class USceneComponent* LanternSlot;

	UPROPERTY(BlueprintReadWrite, Category = "Lantern")
	bool bEquipLantern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable")
	int EquipableIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EscapePod")
	class AEscapePod* InteractingEscapePod;

	UFUNCTION(BlueprintImplementableEvent, Category = "AnimationTrigger")
	void BeginInteractAnimationTrigger();

	UFUNCTION(BlueprintImplementableEvent, Category = "AnimationTrigger")
	void EndInteractAnimationTrigger();

	UFUNCTION(BlueprintImplementableEvent, Category = "AnimationTrigger")
	void BeginEquipAnimationTrigger();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "AnimationTrigger")
	void EndEquipAnimationTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Interaction
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance;
	
	void PerformInteractionCheck();
	void CouldNotFindInteractable();
	void FoundNewInteractable(UInteractionComponent* Interactable);
	void BeginInteract();
	void EndInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteract();
	
	void Interact();

	UPROPERTY()
	FInteractionData InteractionData;

	FORCEINLINE class UInteractionComponent* GetInteractable() const { return InteractionData.ViewdInteractionComponent; }
	
	FTimerHandle TimerHandle_Interact;

	UPROPERTY()
	bool bIsShooting = false;

public:

	//Interaction
	bool IsInteracting() const;
	float GetRemainingInteractTime() const;

	UPROPERTY(BlueprintReadWrite, Category = "DragAndDrop")
	bool bIsDragging = false;

	//UsingItemsInInventory
	UFUNCTION(BlueprintCallable, Category = "InventoryItems")
	void UseItem(class UInventoryItem* Item);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(class UInventoryItem* Item);
	
	UFUNCTION(BlueprintCallable, Category = "InventoryItems")
	void DropItem(class UInventoryItem* Item, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "InventoryItems")
	void DropItemSpecificTransform(class UInventoryItem* Item, const int32 Quantity, FTransform SpawnTransform);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropItemSpecificTransform(class UInventoryItem* Item, const int32 Quantity, FTransform SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "DropAll")
	void DropAll();

	UFUNCTION()
	FVector InternalDropAllTrace();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DropAll")
	float DropAllSpawnInnerRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DropAll")
	float DropAllSpawnOuterRadius;

	UFUNCTION(BlueprintCallable, Category = "Chest")
	void InsertItemToChest(class UInventoryItem* Item, const int32 Quantity, UInventoryComponent* ChestInventory);
	
	UFUNCTION(Server, Reliable, Category = "Chest")
	void ServerInsertItemToChest(class UInventoryItem* Item, const int32 Quantity, UInventoryComponent* ChestInventory);
	
	UFUNCTION(BlueprintCallable, Category = "Chest")
    void InsertItemFromChest(class UInventoryItem* Item, const int32 Quantity, UInventoryComponent* ChestInventory);
    	
    UFUNCTION(Server, Reliable, Category = "Chest")
    void ServerInsertItemFromChest(class UInventoryItem* Item, const int32 Quantity, UInventoryComponent* ChestInventory);

	UPROPERTY(EditDefaultsOnly, Category = "InventoryItems")
	TSubclassOf<class APickable> PickableClass;
	
	UFUNCTION()
	void CallServerAbility();

	UFUNCTION(BlueprintImplementableEvent, Category="PlayerDeath")
	void HandleDeath(AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category="PlayerDeath")
	void HandleDeathSetup();

	UFUNCTION(BlueprintImplementableEvent)
	void SendEndGameToGameState();
	
	UFUNCTION(BlueprintCallable, Category="Craft")
	void Craft(
		const TArray<TSubclassOf<UInventoryItem>>& IngredientsList,
		const TArray<int>& IngredientsQuantity, 
		const TArray<TSubclassOf<UInventoryItem>>& OutputsList,
		const TArray<int>& OutputsQuantity,
		UInventoryComponent* ChestInventory
		);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Craft")
	void ServerCraft(
		const TArray<TSubclassOf<UInventoryItem>>& IngredientsList,
		const TArray<int>& IngredientsQuantity, 
		const TArray<TSubclassOf<UInventoryItem>>& OutputsList,
		const TArray<int>& OutputsQuantity,
		UInventoryComponent* ChestInventory
		);

	UFUNCTION(Client, Reliable, Category="Craft")
	void PostCraftUpdate();

	UPROPERTY(BlueprintReadWrite, Category="Craft")
	TArray<class UCraft*> PendingCraftWidget;

	UPROPERTY(BlueprintReadWrite, Category="CraftTableDragAndDrop")
	class UCraftTableWidget* InteractingCraftTableWidget;

	UFUNCTION(BlueprintCallable, Category="CraftTableDragAndDrop")
	void SetIsDragging(bool IsDrag);

	UPROPERTY(BlueprintReadWrite)
	bool IsOpenedEscapePodWidget;

	UFUNCTION(BlueprintCallable, Category="EscapePod")
	void ClickSubmitEscapePodItem(TSubclassOf<UInventoryItem> SubmittedIngredient);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category="EscapePod")
	void ServerClickSubmitEscapePodItem(TSubclassOf<UInventoryItem> SubmittedIngredient);

	UFUNCTION(BlueprintPure, Category = "Camera")
	FORCEINLINE UCameraComponent* GetCamera() const { return CameraComp; }

	UPROPERTY(BlueprintReadOnly, Category="ItemEffect")
	bool bSpeedItemBuffActive;

	UFUNCTION(BlueprintCallable, Category="ItemEffect")
	void ActiveSpeedItemBuff(float SpeedMultiplier, float Duration);

	FTimerHandle SpeedItemBuffTimer;
	float AddedItemBuffSpeed;
	void DeactivateSpeedItemBuff();

	UPROPERTY(BlueprintReadOnly, Category="ItemEffect")
	bool bInvincibilityItemActive;

	UFUNCTION(BlueprintCallable, Category="Invincibility")
	void ActivateInvincibility(float Duration);

	void DeactivateInvincibility();
	FTimerHandle InvincibilityTimer;
	
	

protected:
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();
	void ActivateAbility();
	void Shoot();

	UPROPERTY(Replicated)
	bool bStartShooting;

	UFUNCTION()
	void SetStartShooting(bool Value);

	UFUNCTION(Server, Reliable)
	void ServerSetStartShooting(bool Value);

	UFUNCTION(BlueprintCallable)
	void BeginShooting();

	UFUNCTION(BlueprintCallable)
	void EndShooting();

	void SwitchEquipable(float Value);

	UFUNCTION(Server, Reliable)
	void ServerSwitchEquipable(float Value);
	
	UPROPERTY(BlueprintReadWrite, Category="GuideBook")
	bool bIsGuideBookOpen;

	UFUNCTION(Server,Reliable,WithValidation)
	void ServerShoot();
	
	//send to server
	UFUNCTION(Server,Reliable)
	void ServerActivateAbility();

	//server multicast to clients
	UFUNCTION(NetMulticast,Reliable)
	void RPCActivateAbility();

	UFUNCTION(BlueprintImplementableEvent, Category="Ability")
	void CheckCanUseAbility();
	
	//FTimerHandle CooldownTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* EXeffect;

	UPROPERTY(BlueprintReadWrite)
	class UBoxComponent* InteractionBox;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

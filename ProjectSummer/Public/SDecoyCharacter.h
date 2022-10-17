// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.h"
#include "SDecoyCharacter.generated.h"

UCLASS()
class PROJECTSUMMER_API ASDecoyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASDecoyCharacter();

	bool IsInteracting() const;
	float GetRemainingInteractTime() const;

	UPROPERTY(BlueprintReadWrite,Category="Time")
	AActor* OwnerCharacter = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	UBoxComponent* InteractionBox;
	
	FTimerHandle PossessTimer;

	UPROPERTY(EditDefaultsOnly,Category="Time")
	float PossessTime;

	UFUNCTION(BlueprintImplementableEvent)
	void SetUpMesh(AActor* PlayerCharacter);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RePossess();
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetupPossess(AActor* Player,ASDecoyCharacter* SpawnedActor);

};

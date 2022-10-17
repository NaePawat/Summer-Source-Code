// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItems/ResetCooldownInventoryItem.h"
#include "Components/WidgetComponent.h"
#include "Interactable.generated.h"

UCLASS()
class PROJECTSUMMER_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* _RootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* InteractionWidget;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsActivated = false;

	UPROPERTY(EditAnywhere)
	class UInteractionComponent* InteractionComponent;

	UFUNCTION()
	virtual void OnBeginFocus(class ACharacter* Character);

	UFUNCTION()
	virtual void OnEndFocus(class ACharacter* Character);
	
	UFUNCTION()
	virtual void OnBeginInteract(class ACharacter* Character);

	UFUNCTION()
	virtual void OnEndInteract(class ACharacter* Character);

	UFUNCTION()
	virtual void OnInteract(class ACharacter* Character);
};

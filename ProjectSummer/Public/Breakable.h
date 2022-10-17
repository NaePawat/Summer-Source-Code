// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActors/Pickable.h"
#include "Breakable.generated.h"

UCLASS() 
class PROJECTSUMMER_API ABreakable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties 
	ABreakable();

	UPROPERTY(EditAnywhere, Category="Components")
	class UStaticMeshComponent* Mesh;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, Replicated)
	float Health;

	UPROPERTY(EditDefaultsOnly, Replicated)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	bool bDropMat;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickable> PickupIngredient;

	UPROPERTY(Instanced, EditDefaultsOnly, Category="Material")
	UInventoryItem* Material;

	UPROPERTY(EditDefaultsOnly, Category="Drop Rate")
	float DropRateThreshold;

	UPROPERTY()
	float DropRate;

	UFUNCTION()
	void HandleDropRate(float Damage);

	UPROPERTY(EditDefaultsOnly, Category="Level")
	bool bIsLv2Required;

	UPROPERTY(EditDefaultsOnly, Category="Level")
	float Lv1DamageMultiplier = 1;
	
	UPROPERTY(EditDefaultsOnly, Category="Level")
	float Lv2DamageMultiplier = 1;
	
	UPROPERTY(EditDefaultsOnly, Category="Breakable")
	bool bIsBreakable = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Material", meta = (EditCondition = "!bUseBoundingBox"))
	float SpawnOuterRadius;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Material", meta = (EditCondition = "!bUseBoundingBox"))
	float SpawnInnerRadius;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Material")
	bool bUseBoundingBox;

	FVector TraceFloor();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

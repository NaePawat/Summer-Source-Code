// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacter.h"
#include "Components/ActorComponent.h"
#include "SHealth.generated.h"


UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class PROJECTSUMMER_API USHealth : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealth();

	UFUNCTION(Server,Reliable)
	void ServerHealth(float StartHealth);
	
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "HealthComponent")
	float Health;

	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* Actor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};

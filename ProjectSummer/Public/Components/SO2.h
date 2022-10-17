// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHealth.h"
#include "Components/ActorComponent.h"
#include "SO2.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSUMMER_API USO2 : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USO2();

	UPROPERTY(BlueprintReadWrite, Category="O2")
	bool bIsInO2FreeZone;

	UPROPERTY(EditDefaultsOnly, Category="O2")
	float DepleteRate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated,EditDefaultsOnly, BlueprintReadWrite, Category="O2")
	float O2Volume;

	UPROPERTY(EditDefaultsOnly, Category="O2")
	float DepleteDamage;

	UPROPERTY(EditDefaultsOnly, Category="O2")
	float RefillRate;

	UPROPERTY(EditDefaultsOnly, Category="O2")
	float ZoneInterval;

	UPROPERTY()
	float LastCheckTime;

	UPROPERTY()
	USHealth* MyHealth;

	UPROPERTY()
	ASCharacter* MyOwner;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="O2")
	float MaxO2Volume;

	FTimerHandle O2TimerHandle;

	UFUNCTION()
	void ZoneDepleteO2();

	UFUNCTION()
	void ZoneRefillO2();
	
	UFUNCTION(Server,Unreliable)
	void SetO2Volume(const float Volume);

public:
	
	UFUNCTION(BlueprintCallable, Category="O2")
	float RefillO2(const float RefillVolume);
	
private:
	//return actual amount add to O2Volume
	UFUNCTION()
	float RefillO2_Internal(const float RefillVolume);

	//return actual amount remove from O2Volume
	UFUNCTION()
	float DepleteO2_Internal(const float DepleteVolume);
	
public:
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};

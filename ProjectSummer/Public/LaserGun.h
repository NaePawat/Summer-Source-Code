// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "GameFramework/Actor.h"
#include "LaserGun.generated.h"

UCLASS()
class PROJECTSUMMER_API ALaserGun : public AGun
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Shoot(AController* Shooter, bool Begin) override;

	class ALaserBullet* CurrentLaser;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 1.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ALaserBullet> LaserClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* Lv2RequiredEffect;

	UFUNCTION(NetMulticast, Reliable)
	void SpawnEffect(bool IsLv2Required);

	UFUNCTION(BlueprintImplementableEvent, Category="Effect")
	void SpawnEffectBP(bool IsLv2Required);

	UPROPERTY(Replicated)
	FVector EffectLocation;
	
};

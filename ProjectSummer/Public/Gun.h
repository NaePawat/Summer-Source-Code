// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bullet.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class PROJECTSUMMER_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere, Category="Components")
	class UOverheatWidgetComponent* OverheatWidgetComponent;

	UPROPERTY(EditAnywhere, Category="Shooting")
	TSubclassOf<ABullet> Bullet;

	FORCEINLINE float GetHeat() const { return Heat; }
	FORCEINLINE float GetHeatThreshold() const { return HeatThreshold; }

	UPROPERTY(EditAnywhere)
	class UArrowComponent *Arrow;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="ShootCooldown")
	float ShootDistance = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category="ShootCooldown")
	float ShootCooldown = 0.2;

	bool bCooldown = false;
	void ReShootCooldown();

	FTimerHandle ShootCooldownTimer;

	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	bool bIsHeatable = true;
	
	bool bOverheat;
	void ReOverheadCooldown();

	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	float OverheatCooldown = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	float HeatPerShot = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	float HeatThreshold = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	float HeatDeduct = 0.2;

	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	float HeatDeductPeriod = 0.1;
	
	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	float FirstHeatDeductPeriod = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="OverheatCooldown")
	float Heat = 0.f;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateWidget(float NewHeat);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCool();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOverheating();

	void HeatReducing();

	FTimerHandle HeatReducingTimer;

	FTimerHandle OverheatCooldownTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Shoot(AController* Shooter, bool Begin);

	UFUNCTION(BlueprintImplementableEvent)
	void GunNoise();

};

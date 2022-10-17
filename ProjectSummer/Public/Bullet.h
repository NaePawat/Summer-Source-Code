// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraCommon.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class PROJECTSUMMER_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	class UProjectileMovementComponent* BulletMovement;

	UPROPERTY(EditAnywhere, Category="Component")
	class UCapsuleComponent* CapComp;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage;

	UPROPERTY()
	FVector HitInfo;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageType;
	
	UPROPERTY()
	AController* Shooter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* SystemTemplate;

	UFUNCTION(NetMulticast, Reliable)
	void SpawnEffect(FVector Location);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

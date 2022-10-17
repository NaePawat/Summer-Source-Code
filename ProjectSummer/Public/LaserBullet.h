// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserBullet.generated.h"

UCLASS()
class PROJECTSUMMER_API ALaserBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserBullet();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent *StaticMesh;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* _RootComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle LaserTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Lifetime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Die();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void RenewFading();

	UFUNCTION(NetMulticast, Reliable)
	void SetScale(FVector Scale);

	UFUNCTION(BlueprintImplementableEvent)
	void RenewFadingImple();

};


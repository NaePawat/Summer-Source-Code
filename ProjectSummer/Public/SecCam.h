// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SecCam.generated.h"

UCLASS()

class PROJECTSUMMER_API ASecCam : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASecCam();

	UFUNCTION()
	void SetLooker(AActor* Player, int CamNum);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	AActor* CamOwner;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	int CamIndex;

	UFUNCTION(BlueprintCallable)
	void StopMovement();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// player input plz
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

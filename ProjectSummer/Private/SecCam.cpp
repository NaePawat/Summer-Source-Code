// Fill out your copyright notice in the Description page of Project Settings.

#include "SecCam.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASecCam::ASecCam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CamOwner = nullptr;
	CamIndex = 0;
}

void ASecCam::SetLooker(AActor* Player,int CamNum)
{
	if(Player)
	{
		CamOwner = Player;
		CamIndex = CamNum;
	}
}

// Called when the game starts or when spawned
void ASecCam::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASecCam::StopMovement()
{
	USphereComponent* SphereComp = FindComponentByClass<USphereComponent>();
	UProjectileMovementComponent* ProjectileComp = FindComponentByClass<UProjectileMovementComponent>();
	if(SphereComp && ProjectileComp)
	{
		SphereComp->SetSimulatePhysics(false);
		ProjectileComp->StopMovementImmediately();
	}
}

// Called every frame
void ASecCam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASecCam::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("LookUp", this, &ASecCam::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ASecCam::AddControllerYawInput);
}

void ASecCam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASecCam, CamIndex);
	DOREPLIFETIME(ASecCam, CamOwner);
}

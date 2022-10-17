// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserBullet.h"

// Sets default values
ALaserBullet::ALaserBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static mesh"));
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = _RootComponent;
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StaticMesh->CastShadow = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ALaserBullet::BeginPlay()
{
	Super::BeginPlay();
	
	//GetWorldTimerManager().SetTimer(LaserTimerHandle, this, &ALaserBullet::Die, 1.0f, false, Lifetime);
	
}

// Called every frame
void ALaserBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaserBullet::Die()
{
	Destroy();
}

void ALaserBullet::SetScale_Implementation(FVector Scale)
{
	SetActorScale3D(Scale);
	//UE_LOG(LogTemp,Error,TEXT("%s"), *Scale.ToString());
}

void ALaserBullet::RenewFading_Implementation()
{
	RenewFadingImple();
}


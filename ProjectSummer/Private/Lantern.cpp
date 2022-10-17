// Fill out your copyright notice in the Description page of Project Settings.


#include "Lantern.h"

#include "SCharacter.h"

// Sets default values
ALantern::ALantern()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = _RootComponent;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	bReplicates = true;
}

// Called when the game starts or when spawned
void ALantern::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALantern::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALantern::StartDestroyTimer(ASCharacter* Character, float Duration)
{
	if(Character->HasAuthority())
	{
		OwnerCharacter = Character;
		GetWorldTimerManager().SetTimer(DestroyTimer, this,
			&ALantern::DestroySelf, Duration, false);
	}
}

void ALantern::DestroySelf()
{
	OwnerCharacter->bEquipLantern = false;
	Destroy();
}
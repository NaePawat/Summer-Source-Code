// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActors/Interactable.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InteractionComponent.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = _RootComponent;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction Component"));
	InteractionComponent->SetupAttachment(RootComponent);

	InteractionComponent->OnBeginFocus.AddDynamic(this, &AInteractable::OnBeginFocus);
	InteractionComponent->OnEndFocus.AddDynamic(this, &AInteractable::OnEndFocus);
	InteractionComponent->OnBeginInteract.AddDynamic(this, &AInteractable::OnBeginInteract);
	InteractionComponent->OnEndInteract.AddDynamic(this, &AInteractable::OnEndInteract);
	InteractionComponent->OnInteract.AddDynamic(this, &AInteractable::OnInteract);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//when player can interact (the interaction card show)
void AInteractable::OnBeginFocus(ACharacter* Character)
{
}

void AInteractable::OnEndFocus(ACharacter* Character)
{
}

//when player interacting (holding interact)
void AInteractable::OnBeginInteract(ACharacter* Character)
{
}

void AInteractable::OnEndInteract(ACharacter* Character)
{
}

//when interact
void AInteractable::OnInteract(ACharacter* Character)
{
	if(bIsActivated)
	{
		bIsActivated = false;
	}
	else
	{
		bIsActivated = true;
	}
}

void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AInteractable, bIsActivated);
}

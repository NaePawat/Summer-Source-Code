// Fill out your copyright notice in the Description page of Project Settings.

#include "SDecoyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/InteractionComponent.h"

// Sets default values
ASDecoyCharacter::ASDecoyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Interaction System
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Box"));
	InteractionBox->SetRelativeScale3D(FVector(5.0,5.0,1.0));
	InteractionBox->SetRelativeLocation(FVector(0,0,-60));
	InteractionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	InteractionBox->SetupAttachment(GetRootComponent());
	
	InteractionCheckFrequency = 0.f;
	
	
}

bool ASDecoyCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ASDecoyCharacter::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

// Called when the game starts or when spawned
void ASDecoyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASDecoyCharacter::SetupPossess(AActor* Player,ASDecoyCharacter* SpawnedActor)
{
	if(Player && SpawnedActor)
	{
		Player->GetInstigatorController()->Possess(SpawnedActor);
		OwnerCharacter = Player;
		SetUpMesh(Player);
		GetWorld()->GetTimerManager().SetTimer(PossessTimer,this,&ASDecoyCharacter::RePossess,1.0f,false,PossessTime);
	}
}

void ASDecoyCharacter::MoveForward(float Value)
{
	const FRotator temp = GetControlRotation();
    const FRotator temp2 = FRotator(0,temp.Yaw,0);
    AddMovementInput(UKismetMathLibrary::GetForwardVector(temp2) * Value);
}

void ASDecoyCharacter::MoveRight(float Value)
{
	const FRotator temp = GetControlRotation();
    const FRotator temp2 = FRotator(0,temp.Yaw,0);
    AddMovementInput(UKismetMathLibrary::GetRightVector(temp2) * Value);
}

void ASDecoyCharacter::BeginCrouch()
{
	Crouch();
}

void ASDecoyCharacter::EndCrouch()
{
	UnCrouch();
}

void ASDecoyCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
	
	TArray<AActor*>OverlappingActors;
	InteractionBox->GetOverlappingActors(OverlappingActors);
	if(OverlappingActors.Num() == 0)
	{
		CouldNotFindInteractable();
		return;
	}
	
	float ClosestDistance = -1;
	UInteractionComponent* ClosestInteractable = nullptr;
	for(auto CurrentInteractable:OverlappingActors)
	{
		if(UInteractionComponent* InteractionComponent =
			Cast<UInteractionComponent>(CurrentInteractable->GetComponentByClass(UInteractionComponent::StaticClass())))
		{
			float CurrentDistance = GetDistanceTo(CurrentInteractable);
			if(CurrentDistance <= InteractionComponent->InteractionDistance)
			{
				if(ClosestInteractable == nullptr || CurrentDistance < ClosestDistance)
				{
					ClosestInteractable = InteractionComponent;
					ClosestDistance = CurrentDistance;
				}
			}
		}
	}
	if(ClosestInteractable)
	{
		if(ClosestInteractable != GetInteractable())
		{
			FoundNewInteractable(ClosestInteractable);
		}
		return;
	}
	CouldNotFindInteractable();
}

void ASDecoyCharacter::CouldNotFindInteractable()
{
	if(GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	if(UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);

		if(InteractionData.bInteractHeld)
		{
			EndInteract();
		}
	}

	InteractionData.ViewdInteractionComponent = nullptr;
}

void ASDecoyCharacter::FoundNewInteractable(UInteractionComponent* Interactable)
{
	EndInteract();

	if(UInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}
	InteractionData.ViewdInteractionComponent = Interactable;
	Interactable->BeginFocus(this);
}

void ASDecoyCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

bool ASDecoyCharacter::ServerBeginInteract_Validate()
{
	return true;
}

void ASDecoyCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

bool ASDecoyCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ASDecoyCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if(UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

void ASDecoyCharacter::BeginInteract()
{
	if(!HasAuthority())
	{
		ServerBeginInteract();
	}

	if(HasAuthority())
	{
		PerformInteractionCheck();
	}

	InteractionData.bInteractHeld = true;

	if(UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);

		if(FMath::IsNearlyZero(Interactable->InteractionTime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this,
				&ASDecoyCharacter::Interact, Interactable->InteractionTime, false);
		}
	}
}

void ASDecoyCharacter::EndInteract()
{
	if(!HasAuthority())
	{
		ServerEndInteract();
	}

	InteractionData.bInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}



// Called every frame
void ASDecoyCharacter::Tick(float DeltaTime)
{
	if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

void ASDecoyCharacter::RePossess_Implementation()
{
	ASCharacter* OwnerCharacterPawn = Cast<ASCharacter>(OwnerCharacter);
	if(OwnerCharacterPawn)
	{
		if(GetInstigatorController())
		{
			GetInstigatorController()->Possess(OwnerCharacterPawn);
			UCharacterMovementComponent* OwnerMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
			if(OwnerMovement)
			{
				OwnerMovement->SetMovementMode(EMovementMode::MOVE_Walking);
			}
			Destroy();
		}
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Unable to repossess!"));
	}
}

// Called to bind functionality to input
void ASDecoyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASDecoyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASDecoyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASDecoyCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ASDecoyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASDecoyCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASDecoyCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASDecoyCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASDecoyCharacter::EndInteract);

}
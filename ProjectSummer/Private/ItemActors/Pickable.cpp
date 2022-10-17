// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActors/Pickable.h"

#include "SCharacter.h"
#include "SDecoyCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/InteractionComponent.h"
#include "Components/InventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "InventoryItems/GunInventoryItem.h"
#include "Net/UnrealNetwork.h"


APickable::APickable()
{
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = _RootComponent;
	PickableMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickableMesh");
	//PickableMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	PickableMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	PickableMesh->SetupAttachment(RootComponent);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("PickableInteractionComponent");
	InteractionComponent->InteractionTime = 0.f;
	InteractionComponent->InteractionDistance = 200.f;
	InteractionComponent->InteractableNameText = FText::FromString("Pickable");
	InteractionComponent->InteractableActionText = FText::FromString("Take");
	InteractionComponent->OnInteract.AddDynamic(this, &APickable::OnTakePickable);
	InteractionComponent->SetupAttachment(RootComponent);

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Box"));
	InteractionBox->SetupAttachment(GetRootComponent());
	

	SetReplicates(true);
}

void APickable::InitializePickable(const TSubclassOf<UInventoryItem> InventoryItemClass,
	const int32 Quantity, const float Capacity, const float Amount)
{
	if(HasAuthority() && InventoryItemClass && Quantity > 0)
	{
		InventoryItem = NewObject<UInventoryItem>(this, InventoryItemClass);
		InventoryItem->SetQuantity(Quantity);
		InventoryItem->SetAmount(Amount);
		InventoryItem->SetCapacity(Capacity);

		OnRep_Item();
		InventoryItem->MarkDirtyForReplication();
	}
}

void APickable::InitializeGun(const TSubclassOf<class AGun> GunClass)
{
	Cast<UGunInventoryItem>(InventoryItem)->SetGunClass(GunClass);
	OnRep_Item();
	InventoryItem->MarkDirtyForReplication();
}

void APickable::OnRep_Item()
{
	if(InventoryItem)
	{
		PickableMesh->SetStaticMesh(InventoryItem->PickUpMesh);
		PickableMesh->SetWorldScale3D(InventoryItem->MeshScale);
		InteractionComponent->InteractableNameText = InventoryItem->ItemDisplayName;

		InventoryItem->OnInventoryItemModified.AddDynamic(this, &APickable::OnItemModified);
	}

	InteractionComponent->RefreshWidget();
}

void APickable::OnItemModified()
{
	if(InteractionComponent)
	{
		InteractionComponent->RefreshWidget();
	}
}

void APickable::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority() && InventoryItemTemplate && bNetStartup)
	{
		InitializePickable(InventoryItemTemplate->GetClass(),
			InventoryItemTemplate->GetQuantity(), InventoryItemTemplate->GetCapacity(), InventoryItemTemplate->GetAmount());
		//if gun pickable
		if(UGunInventoryItem* GunTemplate = Cast<UGunInventoryItem>(InventoryItemTemplate))
		{
			InitializeGun(GunTemplate->GetGunClass());
		}
	}
	if(!bNetStartup)
	{
		AllignWithGround();
	}
	if(InventoryItem)
	{
		InventoryItem->MarkDirtyForReplication();
	}
}

void APickable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickable, InventoryItem);
}

bool APickable::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if(InventoryItem && Channel->KeyNeedsToReplicate(InventoryItem->GetUniqueID(), InventoryItem->RepKey))
	{
		bWroteSomething |= Channel->ReplicateSubobject(InventoryItem, *Bunch, *RepFlags);
	}
	return bWroteSomething;
}

#if WITH_EDITOR
void APickable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if(PropertyName == GET_MEMBER_NAME_CHECKED(APickable, InventoryItemTemplate))
	{
		if(InventoryItemTemplate)
		{
			PickableMesh->SetStaticMesh(InventoryItemTemplate->PickUpMesh);
		}
	}
}
#endif

void APickable::OnTakePickable(ACharacter* Taker)
{
	if(!Taker)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickable was taken but player was no valid"))
		return;
	}

	if(HasAuthority() && !IsPendingKill() && InventoryItem)
	{
		UInventoryComponent* PlayerInventory;
		if(ASCharacter* SCharacter = Cast<ASCharacter>(Taker))
		{
			PlayerInventory = SCharacter->PlayerInventory;
			if(PlayerInventory)
			{
				const FInventoryItemAddResult AddResult = PlayerInventory->TryAddInventoryItem(InventoryItem);

				if(AddResult.ActualAmountGiven < InventoryItem->GetQuantity())
				{
					InventoryItem->SetQuantity(InventoryItem->GetQuantity() - AddResult.ActualAmountGiven);
				}
				else if(AddResult.ActualAmountGiven >= InventoryItem->GetQuantity())
				{
					Pickup();
					Destroy();
				}
			}
		}
		else if(ASDecoyCharacter* DCharacter = Cast<ASDecoyCharacter>(Taker))
		{
			if(ASCharacter* OCharacter = Cast<ASCharacter>(DCharacter->OwnerCharacter))
			{
				PlayerInventory = OCharacter->PlayerInventory;
				if(PlayerInventory)
				{
					const FInventoryItemAddResult AddResult = PlayerInventory->TryAddInventoryItem(InventoryItem);

					if(AddResult.ActualAmountGiven < InventoryItem->GetQuantity())
					{
						InventoryItem->SetQuantity(InventoryItem->GetQuantity() - AddResult.ActualAmountGiven);
					}
					else if(AddResult.ActualAmountGiven >= InventoryItem->GetQuantity())
					{
						Pickup();
						Destroy();
					}
				}
			}
		}
	}
	
}

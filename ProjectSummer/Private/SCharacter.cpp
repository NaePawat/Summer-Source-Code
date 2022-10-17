// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "Gun.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SRole.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItems/InventoryItem.h"
#include "Components/InteractionComponent.h"
#include "VoteManager.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SO2.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ItemActors/Pickable.h"
#include "Widgets/Craft.h"
#include "Widgets/CraftTableWidget.h"
//#include "ToolBuilderUtil.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	//Interaction System
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Box"));
	InteractionBox->SetRelativeScale3D(FVector(5.0,5.0,1.0));
	InteractionBox->SetRelativeLocation(FVector(0,0,-60));
	InteractionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	InteractionBox->SetupAttachment(GetRootComponent());
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);

	InteractionCheckFrequency = 0.f;
	//InteractionCheckDistance = 1000.f

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetCapacity(10);

	DropAllSpawnInnerRadius = 0;
	DropAllSpawnOuterRadius = 200;

	EquipableSlot = CreateDefaultSubobject<USceneComponent>(TEXT("EquipableSlot"));
	EquipableSlot->SetupAttachment(GetRootComponent());

	LanternSlot = CreateDefaultSubobject<USceneComponent>(TEXT("LanternSlot"));
	LanternSlot->SetupAttachment(GetRootComponent());
	
	SetReplicates(true);
	SetReplicateMovement(true);
}


void ASCharacter::RequestVote(ASCharacter* Target)
{
	AVoteManager* VM = Cast<AVoteManager>(UGameplayStatics::GetActorOfClass(GetWorld(),AVoteManager::StaticClass()));
	if(VM)
	{
		if(Target)
		{
			ServerRequestVote(Target);
		}
	}
}

void ASCharacter::ServerRequestVote_Implementation(ASCharacter* Target)
{
	AVoteManager* VM = Cast<AVoteManager>(UGameplayStatics::GetActorOfClass(GetWorld(),AVoteManager::StaticClass()));
	if(!VM->bOngoingVote)
	{
		TArray<AActor*> Players;
		const TSubclassOf<ASCharacter> Character;
		VM->VoteTarget = Target;
		VM->VoteInitializer = this;
		
		VM->SetVoteTimer();
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PC = Cast<APlayerController>(*Iterator);
			if (PC)
			{
				ASCharacter* Char = Cast<ASCharacter>(PC->GetCharacter());
				if(Char)
				{
					Char->CallVote();
				}
			}
		}
		VoteInitializedEvent();
	}	
}

void ASCharacter::CallVote_Implementation()
{
	Vote();
}

void ASCharacter::SetBCanUseAbility(bool Value)
{
	bCanUseAbility = Value;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool ASCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ASCharacter::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void ASCharacter::UseItem(UInventoryItem* Item)
{
	if(!HasAuthority() && Item)
	{
		ServerUseItem(Item);
	}

	if(HasAuthority())
	{
		if(PlayerInventory && !PlayerInventory->FindInventoryItem(Item))
		{
			return;
		}
		
		if(Item)
		{
			Item->Use(this);
		}
	}
}

void ASCharacter::ServerUseItem_Implementation(UInventoryItem* Item)
{
	UseItem(Item);
}

bool ASCharacter::ServerUseItem_Validate(UInventoryItem* Item)
{
	return true;
}

void ASCharacter::DropItem(UInventoryItem* Item, const int32 Quantity)
{
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
	SpawnTransform.SetScale3D(Item->MeshScale);
	DropItemSpecificTransform(Item, Quantity, SpawnTransform);
}

void ASCharacter::DropItemSpecificTransform(UInventoryItem* Item, const int32 Quantity, FTransform SpawnTransform)
{
	if(PlayerInventory && Item && PlayerInventory->FindInventoryItem(Item))
	{
		if(!HasAuthority())
		{
			ServerDropItemSpecificTransform(Item, Quantity, SpawnTransform);
			return;
		}

		if(UEquipableInventoryItem* EquipableInventoryItem = Cast<UEquipableInventoryItem>(Item))
		{
			EquipableInventoryItem->UnEquip(this);
		}
		
		const int32 ItemQuantity = Item->GetQuantity();
		const int32 DroppedQuantity = PlayerInventory->ConsumeItem(Item, Quantity);
		const float DroppedCapacity = Item->GetCapacity();
		const float DroppedAmount = Item->GetAmount();

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.bNoFail = true;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ensure(PickableClass);

		APickable* Pickable = GetWorld()->SpawnActor<APickable>(PickableClass, SpawnTransform, SpawnParameters);
		Pickable->InitializePickable(Item->GetClass(), DroppedQuantity, DroppedCapacity, DroppedAmount);
	}
}

void ASCharacter::ServerDropItemSpecificTransform_Implementation(UInventoryItem* Item, const int32 Quantity,
	FTransform SpawnTransform)
{
	DropItemSpecificTransform(Item, Quantity, SpawnTransform);
}

bool ASCharacter::ServerDropItemSpecificTransform_Validate(UInventoryItem* Item, const int32 Quantity,
	FTransform SpawnTransform)
{
	return true;
}

void ASCharacter::DropAll()
{
	TArray<UInventoryItem*> ItemLists = PlayerInventory->GetInventoryItems();
	for(auto Item : ItemLists)
	{
		FVector SpawnLocation = InternalDropAllTrace();
		const FRotator SpawnRotation = FRotator(0,FMath::RandRange(0,360),0);
		FTransform SpawnTransform(SpawnRotation, SpawnLocation);
		DropItemSpecificTransform(Item, Item->GetQuantity(), SpawnTransform);
	}
}

FVector ASCharacter::InternalDropAllTrace()
{
	FVector TraceStart = GetActorLocation() + FVector(0,0, 200);
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	auto RandVec2D = FVector2D(FMath::VRand()); 
	RandVec2D.Normalize();
	RandVec2D *= FMath::RandRange(DropAllSpawnInnerRadius,DropAllSpawnOuterRadius);
		
	const FVector DropRandom = FVector(RandVec2D.X,RandVec2D.Y,0);

	FVector RandomEnd = GetActorLocation()+DropRandom;
	FVector TraceEnd = (RandomEnd - TraceStart) * 1000;

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	
	if (HitResult.GetActor())
	{
		
		return HitResult.ImpactPoint;
	}
	return InternalDropAllTrace();
}

void ASCharacter::InsertItemToChest(UInventoryItem* Item, const int32 Quantity, UInventoryComponent* ChestInventory)
{
	if(HasAuthority())
	{
		
		if(UEquipableInventoryItem* EquipableInventoryItem = Cast<UEquipableInventoryItem>(Item))
		{
			EquipableInventoryItem->UnEquip(this);
		}
		
		if(ChestInventory)
		{
			FInventoryItemAddResult AddResult = ChestInventory->TryAddInventoryItemFromClass(Item->GetClass(), Quantity);
			if(AddResult.Result == EInventoryItemAddResult::IAR_AllItemsAdded)
			{
				float Remaining = Quantity;
				while (Remaining > 0)
				{
					UInventoryItem* TempItem = PlayerInventory->FindInventoryItemByClass(Item->GetClass());
					int32 TempItemQuantity = TempItem->Quantity;
					if(TempItemQuantity > Remaining)
					{
						PlayerInventory->ConsumeItem(TempItem, Remaining);
						Remaining = 0;
					}
					else
					{
						PlayerInventory->ConsumeItem(TempItem, TempItemQuantity);
						Remaining -= TempItemQuantity;
					}
				}
				ChestInventory->RefreshInventory();
				PlayerInventory->RefreshInventory();
			}
			else if(AddResult.Result == EInventoryItemAddResult::IAR_SomeItemsAdded)
			{
				float Remaining = AddResult.ActualAmountGiven;
				while (Remaining > 0)
				{
					UInventoryItem* TempItem = PlayerInventory->FindInventoryItemByClass(Item->GetClass());
					int32 TempItemQuantity = TempItem->Quantity;
					if(TempItemQuantity > Remaining)
					{
						PlayerInventory->ConsumeItem(TempItem, Remaining);
						Remaining = 0;
					}
					else
					{
						PlayerInventory->ConsumeItem(TempItem, TempItemQuantity);
						Remaining -= TempItemQuantity;
					}
				}
				ChestInventory->RefreshInventory();
				PlayerInventory->RefreshInventory();
			}
		}
	}
	else
	{
		ServerInsertItemToChest(Item, Quantity, ChestInventory);
	}
}

void ASCharacter::InsertItemFromChest(UInventoryItem* Item, const int32 Quantity, UInventoryComponent* ChestInventory)
{
	if(HasAuthority())
	{
		if(ChestInventory)
		{
			const FInventoryItemAddResult AddResult = PlayerInventory->TryAddInventoryItemFromClass(Item->GetClass(), Quantity);
			if(AddResult.Result == EInventoryItemAddResult::IAR_AllItemsAdded)
			{
				float Remaining = Quantity;
				while (Remaining > 0)
				{
					UInventoryItem* TempItem = ChestInventory->FindInventoryItemByClass(Item->GetClass());
					int32 TempItemQuantity = TempItem->Quantity;
					if(TempItemQuantity > Remaining)
					{
						ChestInventory->ConsumeItem(TempItem, Remaining);
						Remaining = 0;
					}
					else
					{
						ChestInventory->ConsumeItem(TempItem, TempItemQuantity);
						Remaining -= TempItemQuantity;
					}
				}
				ChestInventory->RefreshInventory();
				PlayerInventory->RefreshInventory();
			}
			else if(AddResult.Result == EInventoryItemAddResult::IAR_SomeItemsAdded)
			{
				float Remaining = Quantity;
				while (Remaining > 0)
				{
					UInventoryItem* TempItem = ChestInventory->FindInventoryItemByClass(Item->GetClass());
					int32 TempItemQuantity = TempItem->Quantity;
					if(TempItemQuantity > Remaining)
					{
						ChestInventory->ConsumeItem(TempItem, Remaining);
						Remaining = 0;
					}
					else
					{
						ChestInventory->ConsumeItem(TempItem, TempItemQuantity);
						Remaining -= TempItemQuantity;
					}
				}
				ChestInventory->RefreshInventory();
				PlayerInventory->RefreshInventory();
			}
		}
	}
	else
	{
		ServerInsertItemFromChest(Item, Quantity, ChestInventory);
	}
}

void ASCharacter::ServerInsertItemFromChest_Implementation(UInventoryItem* Item, int32 Quantity,
	UInventoryComponent* ChestInventory)
{
	InsertItemFromChest(Item, Quantity, ChestInventory);
}

void ASCharacter::ServerInsertItemToChest_Implementation(UInventoryItem* Item, int32 Quantity, UInventoryComponent* ChestInventory)
{
	InsertItemToChest(Item, Quantity, ChestInventory);
}

void ASCharacter::CallServerAbility()
{
	ServerActivateAbility();
}

void ASCharacter::Craft(
	const TArray<TSubclassOf<UInventoryItem>>& IngredientsList,
	const TArray<int>& IngredientsQuantity, 
	const TArray<TSubclassOf<UInventoryItem>>& OutputsList,
	const TArray<int>& OutputsQuantity,
	UInventoryComponent* ChestInventory
	)
{
	if(HasAuthority())
	{
		int AvailabilityPlayerCapacity = PlayerInventory->GetCurrentCapacity();
		int AvailabilityChestCapacity = ChestInventory->GetCurrentCapacity();
		bool Enough = true;

		//Checking
		for(int i = 0; i < OutputsList.Num(); i++)
		{
			if(const UInventoryItem* DefaultItem = Cast<UInventoryItem>(OutputsList[i]->GetDefaultObject()))
			{
				if(DefaultItem->bStackable)
				{
					int AddQuantity = OutputsQuantity[i];
					if(UInventoryItem* PlayerItem = PlayerInventory->FindInventoryItemByClass(OutputsList[i]))
					{
						if(AddQuantity + PlayerItem->GetQuantity() > PlayerItem->MaxStackSize)
						{
							if(AvailabilityPlayerCapacity > 0)
							{
								AvailabilityPlayerCapacity = AvailabilityPlayerCapacity - 1;
							}
							else if(UInventoryItem* ChestItem = ChestInventory->FindInventoryItemByClass(OutputsList[i]))
							{
								if(AddQuantity + ChestItem->GetQuantity() > ChestItem->MaxStackSize)
								{
									if(AvailabilityChestCapacity > 0)
									{
										AvailabilityChestCapacity = AvailabilityChestCapacity - 1;
									}
									else
									{
										Enough = false;
										break;
									}
								}
							}
							else if(AvailabilityChestCapacity > 0)
							{
								AvailabilityChestCapacity = AvailabilityChestCapacity - 1;
							}
							else
							{
								Enough = false;
								break;
							}
						}
					}
					else
					{
						if(AvailabilityPlayerCapacity > 0)
						{
							AvailabilityPlayerCapacity = AvailabilityPlayerCapacity - 1;
						}
						else if(UInventoryItem* ChestItem = ChestInventory->FindInventoryItemByClass(OutputsList[i]))
						{
							if(AddQuantity + ChestItem->GetQuantity() > ChestItem->MaxStackSize)
							{
								if(AvailabilityChestCapacity > 0)
								{
									AvailabilityChestCapacity = AvailabilityChestCapacity - 1;
								}
								else
								{
									Enough = false;
									break;
								}
							}
						}
						else if(AvailabilityChestCapacity > 0)
						{
							AvailabilityChestCapacity = AvailabilityChestCapacity - 1;
						}
						else
						{
							Enough = false;
							break;
						}
					}
				}
				
				else
				{
					if(AvailabilityPlayerCapacity >= OutputsQuantity[i])
					{
						AvailabilityPlayerCapacity = AvailabilityPlayerCapacity-OutputsQuantity[i];
					}
					else
					{
						const int Remaining = OutputsQuantity[i] - AvailabilityPlayerCapacity;
						AvailabilityPlayerCapacity = 0;
						if(AvailabilityChestCapacity >= Remaining)
						{
							AvailabilityChestCapacity = AvailabilityChestCapacity - Remaining;
						}
						else
						{
							Enough = false;
							break;
						}
					}
				}
			}
		}
		
		if(Enough)
		{
			//consume item from player inventory
			for(int j = 0; j < IngredientsList.Num(); j++)
			{
				if(const UInventoryItem* DefaultItem = Cast<UInventoryItem>(IngredientsList[j]->GetDefaultObject()))
				{
					if(UEquipableInventoryItem* EquipableInventoryItem = Cast<UEquipableInventoryItem>(PlayerInventory->FindInventoryItemByClass(IngredientsList[j])))
					{
						EquipableInventoryItem->UnEquip(this);
					}
					if(DefaultItem->bStackable)
					{
						int32 ConsumedQuantity = 0;
						while(ConsumedQuantity < IngredientsQuantity[j])
						{
							ConsumedQuantity += PlayerInventory->ConsumeItem(PlayerInventory->FindInventoryItemByClass(IngredientsList[j]),IngredientsQuantity[j] - ConsumedQuantity);
						}
					}
					else
					{
						for(int l = 0; l < IngredientsQuantity[j]; l++)
						{
							PlayerInventory->ConsumeItem(PlayerInventory->FindInventoryItemByClass(IngredientsList[j]));
						}
					}
				}
			}

			//create output item
			for(int k = 0; k < OutputsList.Num(); k++)
			{
				UInventoryItem* NewItem = NewObject<UInventoryItem>(this, OutputsList[k]);
				NewItem->SetQuantity(OutputsQuantity[k]);
				const FInventoryItemAddResult PlayerAddResult = PlayerInventory->TryAddInventoryItem(NewItem);

				if(PlayerAddResult.Result == EInventoryItemAddResult::IAR_SomeItemsAdded)
				{
					UInventoryItem* AnotherNewItem = NewObject<UInventoryItem>(this, OutputsList[k]);
					AnotherNewItem->SetQuantity(OutputsQuantity[k] - PlayerAddResult.ActualAmountGiven);
					const FInventoryItemAddResult ChestAddResult = ChestInventory->TryAddInventoryItem(AnotherNewItem);
				}
				else if(PlayerAddResult.Result == EInventoryItemAddResult::IAR_NoItemsAdded)
				{
					UInventoryItem* AnotherNewItem = NewObject<UInventoryItem>(this, OutputsList[k]);
					AnotherNewItem->SetQuantity(OutputsQuantity[k]);
					const FInventoryItemAddResult ChestAddResult = ChestInventory->TryAddInventoryItem(AnotherNewItem);
				}
			}
			PlayerInventory->RefreshInventory();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("not enough space"));
		}
	}
	else
	{
		ServerCraft(IngredientsList, IngredientsQuantity, OutputsList, OutputsQuantity, ChestInventory);
	}
}

void ASCharacter::PostCraftUpdate_Implementation()
{
	for(auto Widget : PendingCraftWidget)
	{
		Widget->PostCraftEvent();
	}
	PendingCraftWidget.Empty();
}

void ASCharacter::ServerCraft_Implementation(
	const TArray<TSubclassOf<UInventoryItem>>& IngredientsList,
	const TArray<int>& IngredientsQuantity, 
	const TArray<TSubclassOf<UInventoryItem>>& OutputsList,
	const TArray<int>& OutputsQuantity,
	UInventoryComponent* ChestInventory
	)
{
	Craft(IngredientsList, IngredientsQuantity, OutputsList, OutputsQuantity, ChestInventory);
}

void ASCharacter::SetIsDragging(bool IsDrag)
{
	bIsDragging = IsDrag;
	if(InteractingCraftTableWidget)
	{
		InteractingCraftTableWidget->RefreshDragAndDropVisibility();
	}
}

void ASCharacter::ClickSubmitEscapePodItem(TSubclassOf<UInventoryItem> SubmittedIngredient)
{
	if(HasAuthority())
	{
		if(InteractingEscapePod)
		{
			InteractingEscapePod->SubmitIngredient(SubmittedIngredient,this);
		}
	}
	else
	{
		ServerClickSubmitEscapePodItem(SubmittedIngredient);
	}
}

void ASCharacter::ServerClickSubmitEscapePodItem_Implementation(TSubclassOf<UInventoryItem> SubmittedIngredient)
{
	ClickSubmitEscapePodItem(SubmittedIngredient);
}

void ASCharacter::ActiveSpeedItemBuff(float SpeedMultiplier, float Duration)
{
	if(!bSpeedItemBuffActive)
	{
		if(USRole* SRole = this->FindComponentByClass<USRole>())
		{
			AddedItemBuffSpeed = SRole->DefaultSpeed * SpeedMultiplier;
			if(UCharacterMovementComponent* MovementComp = this->FindComponentByClass<UCharacterMovementComponent>())
			{
				MovementComp->MaxWalkSpeed += AddedItemBuffSpeed;
			}
				bSpeedItemBuffActive = true;
				GetWorld()->GetTimerManager().SetTimer(SpeedItemBuffTimer, this,
					&ASCharacter::DeactivateSpeedItemBuff, 1.0f, false, Duration);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(SpeedItemBuffTimer);
		GetWorld()->GetTimerManager().SetTimer(SpeedItemBuffTimer, this,
			&ASCharacter::DeactivateSpeedItemBuff, 1.0f, false, Duration);
	}
}

void ASCharacter::DeactivateSpeedItemBuff()
{
	if(UCharacterMovementComponent* MovementComp = this->FindComponentByClass<UCharacterMovementComponent>())
	{
		MovementComp->MaxWalkSpeed -= AddedItemBuffSpeed;
		bSpeedItemBuffActive = false;
	}
}

void ASCharacter::ActivateInvincibility(float Duration)
{
	if(bIsInvincible)
	{
		if(GetWorld()->GetTimerManager().GetTimerRemaining(InvincibilityTimer) < Duration)
		{
			GetWorld()->GetTimerManager().ClearTimer(InvincibilityTimer);
			GetWorld()->GetTimerManager().SetTimer(InvincibilityTimer, this,
				&ASCharacter::DeactivateInvincibility, 1.0f, false, Duration);
		}
	}
	else
	{
		bIsInvincible = true;
		USO2* SelfO2 = FindComponentByClass<USO2>();
		if(SelfO2) SelfO2->DepleteRate = 0.f;
		GetWorld()->GetTimerManager().SetTimer(InvincibilityTimer, this,
			&ASCharacter::DeactivateInvincibility, 1.0f, false, Duration);
	}
}

void ASCharacter::DeactivateInvincibility()
{
	bIsInvincible = false;
	USO2* SelfO2 = FindComponentByClass<USO2>();
	if(SelfO2) SelfO2->DepleteRate = 1.f;
}

void ASCharacter::MoveForward(float Value)
{
	const FRotator temp = GetControlRotation();
	const FRotator temp2 = FRotator(0,temp.Yaw,0);
	AddMovementInput(UKismetMathLibrary::GetForwardVector(temp2) * Value);
	//getactorforwardvector
}

void ASCharacter::MoveRight(float Value)
{
	const FRotator temp = GetControlRotation();
	const FRotator temp2 = FRotator(0,temp.Yaw,0);
	AddMovementInput(UKismetMathLibrary::GetRightVector(temp2) * Value);
}

void ASCharacter::SwitchEquipable(float Value)
{
	if(!bIsGuideBookOpen)
	{
		if(HasAuthority())
		{
			TArray<class UEquipableInventoryItem*> EquipableList = PlayerInventory->GetEquipableList();
			if(Equipping)
			{
				if(EquipableList.Num() != 0)
				{
					int index = EquipableList.Find(EquippingInventoryItem);
					if(index > 0)
					{
						EquipableIndex = index;
					}
				}
				else
				{
					return;
				}
			}

			if(Value > 0)
			{
				EquipableIndex = (EquipableIndex+1) % (EquipableList.Num()+1);
			}
			else if(Value < 0)
			{
				EquipableIndex = (EquipableIndex-1) < 0 ? EquipableList.Num() : (EquipableIndex-1);
			}
			else
			{
				return;
			}

			if(Equipping)
			{
				Equipping->Destroy();
				Equipping = nullptr;
			}
			if(EquipableIndex >= EquipableList.Num())
			{
				EndEquipAnimationTrigger();
				bEquipping = false;
				return;
			}
			if(EquipableList[EquipableIndex])
			{
				EquipableList[EquipableIndex]->Equip(this);
				bEquipping = true;
				BeginEquipAnimationTrigger();
			}
		}
		else
		{
			ServerSwitchEquipable(Value);
		}
	}
}

void ASCharacter::ServerSwitchEquipable_Implementation(float Value)
{
	SwitchEquipable(Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::ActivateAbility()
{
	USRole* role = FindComponentByClass<USRole>();
	if(role)
	{
		if(role->NameRole == "Medic")
		{
			role->WidgetHandle.Broadcast();
			return;
		}
		if(role->NameRole == "Warper" || role->NameRole == "SecOP")
		{
			role->ThrowHandle.Broadcast();
			return;
		}
	}
	ServerActivateAbility();
}

void ASCharacter::ServerActivateAbility_Implementation()
{
	if (bCanUseAbility) {
		//RPCActivateAbility();
		USRole* role = FindComponentByClass<USRole>();
		if(role)
		{
			role->UseAbility();
		}
		bCanUseAbility = false;
	}
	CheckCanUseAbility();
}

void ASCharacter::BeginShooting()
{
	bIsShooting = true;
	SetStartShooting(true);
}

void ASCharacter::EndShooting()
{
	bIsShooting = false;
}

void ASCharacter::Shoot()
{
	if(HasAuthority())
	{
		if(AGun* Gun = Cast<AGun>(Equipping))
		{
			Gun->Shoot(this->GetController(), bStartShooting);
			SetStartShooting(false);
		}
		return;
	}
	ServerShoot();
}

void ASCharacter::SetStartShooting(bool Value)
{
	if(HasAuthority())
	{
		bStartShooting = Value;
	}
	else
	{
		ServerSetStartShooting(Value);
	}
}

void ASCharacter::ServerSetStartShooting_Implementation(bool Value)
{
	SetStartShooting(Value);
}

void ASCharacter::ServerShoot_Implementation()
{
	Shoot();
}

bool ASCharacter::ServerShoot_Validate()
{
	return true;
}

void ASCharacter::RPCActivateAbility_Implementation()
{
	UWorld* const World = GetWorld();
	if (World != nullptr) {
		UGameplayStatics::SpawnEmitterAtLocation(World, EXeffect, GetActorLocation());
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* for interaction using dedicated server
	const bool bIsInteractingOnServer = (HasAuthority() && IsInteracting());
	if((!HasAuthority() || bIsInteractingOnServer) &&
		GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}*/
	
	if((GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency) && InteractionBox && bIsAlive)
	{
		PerformInteractionCheck();
	}

	//shoot
	if(Equipping)
	{
		if(Equipping->IsA(AGun::StaticClass()))
		{
			if(bIsShooting)
			{
				Shoot();
				//UE_LOG(LogTemp, Error, TEXT("shoot"));
			}
		}
	}
}

void ASCharacter::PerformInteractionCheck()
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

void ASCharacter::CouldNotFindInteractable()
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

void ASCharacter::FoundNewInteractable(UInteractionComponent* Interactable)
{
	EndInteract();

	if(UInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}
	InteractionData.ViewdInteractionComponent = Interactable;
	Interactable->BeginFocus(this);
}

void ASCharacter::BeginInteract()
{
	if(!HasAuthority())
	{
		ServerBeginInteract();
	}

	if(HasAuthority())
	{
		PerformInteractionCheck();
		BeginInteractAnimationTrigger();
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
				&ASCharacter::Interact, Interactable->InteractionTime, false);
		}
	}
}

void ASCharacter::EndInteract()
{
	if(!HasAuthority())
	{
		ServerEndInteract();
	}
	if(HasAuthority())
	{
		EndInteractAnimationTrigger();
	}

	InteractionData.bInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void ASCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

bool ASCharacter::ServerBeginInteract_Validate()
{
	return true;
}

void ASCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

bool ASCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ASCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if(UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAxis("SwitchEquipable", this, &ASCharacter::SwitchEquipable);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ASCharacter::ActivateAbility);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ASCharacter::BeginShooting);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ASCharacter::EndShooting);

	//Interaction
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASCharacter::EndInteract);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, bIsInvincible);
	DOREPLIFETIME(ASCharacter, bCanUseAbility);
	DOREPLIFETIME(ASCharacter, bCanUseSafeRoom);
	DOREPLIFETIME(ASCharacter, bIsAlive);
	DOREPLIFETIME(ASCharacter, bStartShooting);
	DOREPLIFETIME(ASCharacter, Equipping);
	DOREPLIFETIME(ASCharacter, bEquipping);
}

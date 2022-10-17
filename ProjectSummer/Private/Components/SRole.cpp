// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SRole.h"
#include "SGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SHealth.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SDecoyCharacter.h"
#include "SGameStateBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Foliage/Public/ProceduralFoliageVolume.h"
#include "Foliage/Public/ProceduralFoliageBlockingVolume.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CollisionQueryParams.h"
#include "Components/TextRenderComponent.h"

// Sets default values for this component's properties
USRole::USRole()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	WarpDelay = 1.0f;
	WarpDistance = 3000.f;
	DefaultSpeed = 600.f;
	FastSpeed = 3000.f;
	SprintTime = 4.f;
	InvincTime = 4.f;

	ActorToSpawn = ASDecoyCharacter::StaticClass();
	CameraToSpawn = ASecCam::StaticClass();

	ThrowPitchAngle = 30.f;
	ThrowDistance = 1000.f;
	LaunchPower = 3000.f;

	CamNum = 0;
	
	DetectiveCheckDis = 3000.f;
	ShowHealthTime = 5.f;

	ActivateLineTrace = false;
	ActivateParabolicTrace = false;
	
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USRole::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Actor1;
	TArray<AActor*> Actor2;
	const TSubclassOf<AProceduralFoliageVolume> ProceduralFoliageVolume;
	const TSubclassOf<AProceduralFoliageBlockingVolume> ProceduralFoliageBlockingVolume;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ProceduralFoliageVolume,Actor1);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ProceduralFoliageBlockingVolume,Actor2);
	for(int i =0; i<Actor1.Num();i++)
	{
		ActorsToIgnore.Add(Actor1[i]);
	}
	for(int i =0;i<Actor2.Num();i++)
	{
		ActorsToIgnore.Add(Actor2[i]);
	}
	//GetWorld()->GetTimerManager().SetTimer(StartDelayTimer, [this]() {ServerHealth(); }, 0.2, 1); //deploy laew boom
	AddFastSpeed = FastSpeed - DefaultSpeed;
	AActor* Player = GetOwner();
	OwnerCharacter = Cast<ASCharacter>(Player);
	if(Player)
	{
		UCharacterMovementComponent* MovementComp = Player->FindComponentByClass<UCharacterMovementComponent>();
		if(MovementComp)
		{
			MovementComp->MaxWalkSpeed = DefaultSpeed;
		}
	}
}

// Called every frame
void USRole::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(ActivateLineTrace)
	{
		ServerSetHitComp();
		if(HitComp.GetActor() != nullptr && HitComp.Distance <= WarpDistance)
        {
        	DrawDebugSphere(GetWorld(),HitComp.Location,100,2,FColor::Red);
        }
	}
	if(ActivateParabolicTrace)
	{
		ProjectileHandle.Broadcast();
	}
	// ...
}

//NameRole should be Enum, Use switch instead of if
void USRole::UseAbility()
{
	AActor* Player = GetOwner();
	if(Player)
	{
		if(NameRole=="Warper")
		{
			UCharacterMovementComponent* MovementComp = Player->FindComponentByClass<UCharacterMovementComponent>();
			if(MovementComp)
			{
				MovementComp->DisableMovement();
			}
			//const FVector newLocation = Player->GetActorLocation() + Player->GetActorRotation().Vector()*WarpDistance;
			GetWorld()->GetTimerManager().SetTimer(AbilityTimer,this,&USRole::Warp,1.0f,false,WarpDelay);
			//Player->SetActorLocation(newLocation,false,nullptr,ETeleportType::None);
		}
		if(NameRole=="Octane")
		{
			UCharacterMovementComponent* MovementComp = Player->FindComponentByClass<UCharacterMovementComponent>();
			if(MovementComp)
			{
				MovementComp->MaxWalkSpeed += AddFastSpeed;
				OctaneHandle.Broadcast(true);
				GetWorld()->GetTimerManager().SetTimer(AbilityTimer,this,&USRole::ResetWalkSpeed,1.0f,false,SprintTime);
			}
		}
		if(NameRole=="Bamboozler")
		{
			const FVector spawnLocation = FVector(50,0,0);
			FActorSpawnParameters SpawnParams;
			UCharacterMovementComponent* MovementComp = Player->FindComponentByClass<UCharacterMovementComponent>();
			if(MovementComp)
			{
				MovementComp->DisableMovement();
			}
			ASDecoyCharacter* SpawnedActor = GetWorld()->SpawnActor<ASDecoyCharacter>(ActorToSpawn,Player->GetActorLocation()+spawnLocation,Player->GetActorRotation(), SpawnParams);
			SpawnedActor->SetupPossess(Player,SpawnedActor);
		}
		if(NameRole=="Medic")
		{
			const TArray<APlayerState*> PlayerList = UGameplayStatics::GetGameState(GetWorld())->PlayerArray;
			if(PlayerList.Num()>0)
			{
				if(Index >= 0)
				{
					ASCharacter* Boi =Cast<ASCharacter>(PlayerList[Index]->GetPawn());
					if(Boi)
					{
						Boi->ActivateInvincibility(InvincTime);
						Index = -1;
					}
				}
			}
			UE_LOG(LogTemp, Error, TEXT("%d"), Index);
		}
		if(NameRole=="SecOP")
		{
			/*const FVector spawnLocation = FVector(-50,0,0);
			FActorSpawnParameters SpawnParams;
			ASecCam* Cam = GetWorld()->SpawnActor<ASecCam>(CameraToSpawn,Player->GetActorLocation()+spawnLocation,Player->GetActorRotation(),SpawnParams);
			FRotator ThrowDirection = Player->GetActorRotation();
			ThrowDirection.Pitch += ThrowPitchAngle;
			FVector ThrowVelocity = ThrowDirection.Vector() * 5000;
			UStaticMeshComponent* CamComp = Cast<UStaticMeshComponent>(Cam->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if(CamComp)
			{
				CamComp->AddImpulse(ThrowVelocity,NAME_None,true);
				Cam->SetLooker(Player);
			}
			FVector Loc;
			FRotator Rot;
			FHitResult Hit;
			Player->GetInstigatorController()->GetPlayerViewPoint(Loc,Rot);

			FVector Start =Loc;
			FVector End = Start+(Rot.Vector()*2000);
			FCollisionQueryParams TraceParams;
			GetWorld()->LineTraceSingleByChannel(Hit, Start, End,ECC_Visibility,TraceParams);
			DrawDebugLine(GetWorld(),Start,End,FColor::Orange,false,2.0f);
			AActor* ActorHit = Hit.GetActor();
			*/
			if(ProjectileComp.GetActor() != nullptr)
			{
				UCameraComponent* ControllerCam = Player->FindComponentByClass<UCameraComponent>();
				if(ControllerCam)
				{
					FVector LaunchPos = FVector(ControllerCam->GetForwardVector().X,ControllerCam->GetForwardVector().Y,ControllerCam->GetForwardVector().Z+0.5);
					const FRotator CamRotate = FRotator(Player->GetActorRotation().Roll-180,Player->GetActorRotation().Pitch,Player->GetActorRotation().Yaw-180);
					const FActorSpawnParameters SpawnParams;
					ASecCam* Cam = GetWorld()->SpawnActor<ASecCam>(CameraToSpawn,Player->GetActorLocation(),CamRotate,SpawnParams);
					UProjectileMovementComponent* CamProjMovement = Cam->FindComponentByClass<UProjectileMovementComponent>();
					if(CamProjMovement)
					{
						CamProjMovement->Velocity = LaunchPos*LaunchPower;
						UE_LOG(LogTemp, Error,TEXT("YES"));
					}
					
					UE_LOG(LogTemp, Error,TEXT("hi"));
					if(Cam)
					{
						Cam->SetLooker(Player,CamNum);
						CamNum++;
					}
				}
			}
		}
		if(NameRole=="Detective")
		{
			const TArray<APlayerState*> PlayerList = UGameplayStatics::GetGameState(GetWorld())->PlayerArray;
			if(PlayerList.Num()>0)
			{
				for(int i =0;i<PlayerList.Num();i++)
				{
					if(DetectiveCheckDis > Player->GetDistanceTo(PlayerList[i]->GetPawn()))
					{
						ASCharacter* OtherCharacter = Cast<ASCharacter>(PlayerList[i]->GetPawn());
						if(OtherCharacter && OtherCharacter->bIsAlive)
						{
							USHealth* CharacterHealth = OtherCharacter->FindComponentByClass<USHealth>();
							if(CharacterHealth)
							{
								UE_LOG(LogTemp,Log,TEXT("Health %s"), *FString::SanitizeFloat(CharacterHealth->DefaultHealth));
								PlayerPawns.Add(OtherCharacter);
							}
						}
						else if (OtherCharacter && !OtherCharacter->bIsAlive)
						{
							//DeathHandle.Broadcast(OtherCharacter);
						}
					}
				}
				ShowMinnaSanHealth(PlayerPawns);
				PlayersLoHandle.Broadcast();
			}
			FindEvidenceHandle.Broadcast();
		}
		
	}
}

void USRole::ApplyMedic()
{
	ASCharacter* Player = Cast<ASCharacter>(GetOwner());
	Player->CallServerAbility();
}

void USRole::Deploy()
{
	ASCharacter* Player = Cast<ASCharacter>(GetOwner());
	Player->CallServerAbility();
}

void USRole::SetIndex(int NewIndex)
{
	SetIndexServer(NewIndex);
}

void USRole::ShowMinnaSanHealth_Implementation(const TArray<ASCharacter*>& TargetPlayers)
{
	for(auto Player:TargetPlayers)
	{
		if(Player)
		{
			UTextRenderComponent* HealthTextComp = Player->FindComponentByClass<UTextRenderComponent>();
			USHealth* HealthComp = Player->FindComponentByClass<USHealth>();
			if(HealthTextComp && HealthComp)
			{
				HealthTextComp->SetVisibility(true);
				HealthTextComp->Text = FText::AsNumber(HealthComp->Health);
			}
		}
	}
	GetWorld()->GetTimerManager().SetTimer(ShowHealthTimer,this,&USRole::ResetShowMinnaSanHealth,1.0f,false,ShowHealthTime);
}

void USRole::SetIndexServer_Implementation(int NewIndex)
{
	Index = NewIndex;
}

void USRole::ResetWalkSpeed()
{
	OctaneHandle.Broadcast(false);
	UCharacterMovementComponent* MovementComp = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	MovementComp->MaxWalkSpeed -= AddFastSpeed;
}

void USRole::ResetShowMinnaSanHealth()
{
	for(auto Player:PlayerPawns)
	{
		UTextRenderComponent* HealthTextComp = Player->FindComponentByClass<UTextRenderComponent>();
		HealthTextComp->SetVisibility(false);
	}
	PlayerPawns.Reset();
}

void USRole::Warp()
{
	UCapsuleComponent* CapsuleComp = GetOwner()->FindComponentByClass<UCapsuleComponent>();
	DashWarpEffectHandle.Broadcast(HitComp);
	/*if(CapsuleComp && HitComp.GetActor() != nullptr)
	{
		const FVector newLocation = FVector(HitComp.Location.X,HitComp.Location.Y,HitComp.Location.Z+100);
		GetOwner()->SetActorLocation(newLocation);
	}*/
	UCharacterMovementComponent* MovementComp = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	MovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
}

void USRole::ServerSetHitComp_Implementation()
{
	FVector Loc;
	FRotator Rot;
	GetOwner()->GetInstigatorController()->GetPlayerViewPoint(Loc,Rot);
	const FVector Start =Loc;
	const FVector End = Start+(Rot.Vector()*WarpDistance);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(GetOwner());
	if(OwnerCharacter->Equipping) OwnerEquipping = OwnerCharacter->Equipping;
	if(OwnerEquipping) TraceParams.AddIgnoredActor(OwnerEquipping);
	TraceParams.AddIgnoredActors(ActorsToIgnore);
	GetWorld()->LineTraceSingleByChannel(HitComp, Start, End,ECC_Visibility,TraceParams);
	//DrawDebugLine(GetWorld(),Start,End,FColor::Orange,false,2.0f);
	if(HitComp.GetActor() != nullptr && HitComp.Distance <= WarpDistance)
	{
		DrawDebugSphere(GetWorld(),HitComp.Location,100,2,FColor::Red);
	}
}

void USRole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USRole, NameRole);
	DOREPLIFETIME(USRole, Team);
	DOREPLIFETIME(USRole, AbilityCooldown);
	DOREPLIFETIME(USRole, Index);
	DOREPLIFETIME(USRole, ProjectileComp);
	DOREPLIFETIME(USRole, HitComp);
}


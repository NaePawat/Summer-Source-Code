// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable.h"

#include "DrawDebugHelpers.h"
#include "LaserGun.h"
#include "FamageType/Laser2DT.h"
#include "FamageType/LaserDT.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABreakable::ABreakable() 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	SpawnInnerRadius = 100;
	SpawnOuterRadius = 200;
	bUseBoundingBox = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABreakable::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ABreakable::HandleTakeAnyDamage);

	if(bUseBoundingBox)
	{
		FBox BoundingBox = Mesh->GetStaticMesh()->GetBoundingBox();
		FVector MeshBound = BoundingBox.Max-BoundingBox.Min;
		SpawnInnerRadius = FMath::Max(MeshBound.X * GetActorScale3D().X, MeshBound.Y * GetActorScale3D().Y) /2;
		SpawnOuterRadius = SpawnInnerRadius + 100;
	}
	 
}

void ABreakable::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
    if(bIsBreakable == false){
        return;
    }
    
	if(DamageType->IsA(ULaserDT::StaticClass()))
	{
		if(!bIsLv2Required)
		{
			const float ActualDamage = Damage*Lv1DamageMultiplier;
			//UE_LOG(LogTemp, Error, TEXT("hit once %f"), Damage);
			Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);
			if(ALaserGun* LaserGun = Cast<ALaserGun>(DamageCauser))
			{
				LaserGun->SpawnEffect(false);
			}
			if (Health <= 0)
			{
				Destroy();
			}
			if(bDropMat)
			{
				HandleDropRate(ActualDamage);
			}
		}
		else
		{
			if(ALaserGun* LaserGun = Cast<ALaserGun>(DamageCauser))
			{
				LaserGun->SpawnEffect(true);
				//LaserGun->SpawnEffectBP(true);
			}
		}
	}
	
	else if(DamageType->IsA(ULaser2DT::StaticClass()))
	{
		const float ActualDamage = Damage*Lv2DamageMultiplier;
		//UE_LOG(LogTemp, Error, TEXT("hit once %f"), Damage);
		Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);
		if(ALaserGun* LaserGun = Cast<ALaserGun>(DamageCauser))
		{
			LaserGun->SpawnEffect(false);
			//LaserGun->SpawnEffectBP(false);
		}
		if (Health <= 0)
		{
			Destroy();
		}
		if(bDropMat)
		{
			HandleDropRate(ActualDamage);
		}
	}
	

}

void ABreakable::HandleDropRate(float Damage)
{
	DropRate = FMath::Clamp(DropRate + Damage, 0.f, DropRateThreshold);
	if(DropRate == DropRateThreshold)
	{
		FVector SpawnLocation = TraceFloor();
		const FRotator DropRotation = FRotator(0,FMath::RandRange(0,360),0);
		const FActorSpawnParameters SpawnParams;
		APickable* MatDrop = GetWorld()->SpawnActor<APickable>(PickupIngredient,SpawnLocation,DropRotation,SpawnParams);
		if(MatDrop)
		{
			MatDrop->SetActorScale3D(Material->MeshScale);
			MatDrop->InitializePickable(Material->GetClass(),1,0.f,0.f);
			//MatDrop->InventoryItemTemplate = Material;
		}
		DropRate=0.f;
	}
}

FVector ABreakable::TraceFloor()
{
	FVector TraceStart = GetActorLocation() + FVector(0,0, 200);
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	auto RandVec2D = FVector2D(FMath::VRand()); 
	RandVec2D.Normalize();
	RandVec2D *= FMath::RandRange(SpawnInnerRadius,SpawnOuterRadius);
		
	const FVector DropRandom = FVector(RandVec2D.X,RandVec2D.Y,0);

	FVector RandomEnd = GetActorLocation()+DropRandom;
	FVector TraceEnd = (RandomEnd - TraceStart) * 1000;

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	
	if (HitResult.GetActor())
	{
		
		return HitResult.ImpactPoint;
	}
	return TraceFloor();
}

// Called every frame
void ABreakable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABreakable, Health);
	DOREPLIFETIME(ABreakable, MaxHealth);
}
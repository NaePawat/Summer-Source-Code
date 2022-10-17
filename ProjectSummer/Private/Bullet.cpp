// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	SetRootComponent(BulletMesh);

	CapComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Comp");
	CapComp->SetupAttachment(BulletMesh);

	Damage = 20.f;

	BulletMovement = CreateDefaultSubobject<UProjectileMovementComponent>("BulletMovement");
	BulletMovement->InitialSpeed = 5000.f;
	BulletMovement->MaxSpeed = 5000.f;
	BulletMovement->SetUpdatedComponent(BulletMesh);

    AActor::SetReplicateMovement(true);
	bReplicates = true;

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	CapComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::HandleOnOverlapBegin);
}

void ABullet::HandleOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASCharacter* OtherCharacter = Cast<ASCharacter>(OtherActor);
	if(Shooter)
	{
		if(ACharacter* Character = Cast<ACharacter>(Shooter->GetPawn()))
		{
			if (OtherActor && (OtherActor != this) && OtherComp && OtherActor != Character && OtherCharacter)
			{
				if(OtherCharacter->bIsAlive)
				{
					UGameplayStatics::ApplyDamage(OtherActor, Damage, Shooter, this, DamageType);
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap Begin"));
					//UE_LOG(LogTemp, Error, TEXT("overlap %s"), *OtherActor->GetName());
					SpawnEffect(GetActorLocation());
					Destroy();
				}
			}
		}
	}
}

void ABullet::SpawnEffect_Implementation(FVector Location)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemTemplate,
		Location, FRotator(0,0,0), FVector(0.25, 0.25, 0.25), true,
		true, ENCPoolMethod::None, true);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include <string>

#include "DrawDebugHelpers.h"
#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/OverheatWidgetComponent.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>("GunMesh");
	SetRootComponent(GunMesh);
	OverheatWidgetComponent = CreateDefaultSubobject<UOverheatWidgetComponent>("OverheatWidget");
	OverheatWidgetComponent->SetupAttachment(RootComponent);
	OverheatWidgetComponent->UpdateHeatWidget(Heat, HeatThreshold);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
	Arrow->SetupAttachment(GetRootComponent());
	
	bReplicates=true;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	OverheatWidgetComponent->UpdateHeatWidget(Heat, HeatThreshold);
}

void AGun::ReShootCooldown()
{
	bCooldown = false;
}

void AGun::ReOverheadCooldown()
{
	bOverheat = false;
	Heat = 0;
	MulticastUpdateWidget(Heat);
	MulticastCool();
}

void AGun::MulticastUpdateWidget_Implementation(float NewHeat)
{
	OverheatWidgetComponent->UpdateHeatWidget(NewHeat);
	/*
	if(HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("yay"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("clienfe"));
	}
	*/
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, GetNetOwner()->GetUniqueID().);
}

void AGun::MulticastCool_Implementation()
{
	OverheatWidgetComponent->Cool();
}

void AGun::MulticastOverheating_Implementation()
{
	OverheatWidgetComponent->Overheating();
}

void AGun::HeatReducing()
{
	if(Heat > 0)
	{
		Heat -= HeatDeduct;
		MulticastUpdateWidget(Heat);
	}
	if(Heat > 0)
	{
		GetWorldTimerManager().SetTimer(HeatReducingTimer, this,
			&AGun::HeatReducing, HeatDeductPeriod, false);
	}
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::Shoot(AController* Shooter, bool Begin)
{
	if(!bCooldown)
	{
		if(bIsHeatable && !bOverheat || !bIsHeatable)
		{
			if(ASCharacter* ShooterCharacter = Cast<ASCharacter>(Shooter->GetPawn()))
			{
				UCameraComponent* ShooterCamera = ShooterCharacter->GetCamera();
		
				FVector TraceStart = ShooterCamera->GetComponentLocation();
				FVector TraceEnd = TraceStart + ShooterCamera->GetForwardVector() * ShootDistance;
				FHitResult HitResult;
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);
				QueryParams.AddIgnoredActor(ShooterCharacter);
				GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
				FVector EndPoint;
	
				if (HitResult.GetActor())
				{
					EndPoint = HitResult.ImpactPoint;
				}
				else
				{
					EndPoint = HitResult.TraceEnd;
				}
				//DrawDebugLine(GetWorld(), HitResult.TraceStart, EndPoint, FColor::MakeRandomColor(), false, 1.f);
				FVector ShootVector = EndPoint - Arrow->GetComponentLocation();
				FRotator AimRotation = ShootVector.Rotation();
		
				FTransform SpawnTransform = GetActorTransform();
				SpawnTransform.SetLocation(Arrow->GetComponentLocation());
				SpawnTransform.SetRotation(AimRotation.Quaternion());
				SpawnTransform.SetScale3D(FVector(1,1,1));

				const FActorSpawnParameters SpawnParams;

				ABullet* NewBullet = GetWorld()->SpawnActor<ABullet>(Bullet, SpawnTransform, SpawnParams);
				GunNoise();
				if(NewBullet) NewBullet->Shooter = Shooter;

				bCooldown = true;
				GetWorldTimerManager().SetTimer(ShootCooldownTimer, this,
					&AGun::ReShootCooldown, ShootCooldown, false);

				if(bIsHeatable)
				{
					Heat += HeatPerShot;
					MulticastUpdateWidget(Heat);
					if(Heat < HeatThreshold)
					{
						GetWorldTimerManager().SetTimer(HeatReducingTimer, this,
							&AGun::HeatReducing, FirstHeatDeductPeriod, false);
					}
					else
					{
						bOverheat = true;
						MulticastOverheating();
						GetWorldTimerManager().ClearTimer(HeatReducingTimer);
						GetWorldTimerManager().SetTimer(OverheatCooldownTimer, this,
							&AGun::ReOverheadCooldown, OverheatCooldown, false);
					}
				}
			}
		}
	}
}
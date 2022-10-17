// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserGun.h"
#include "LaserBullet.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "FamageType/Laser2DT.h"
#include "FamageType/LaserDT.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALaserGun::ALaserGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ShootDistance = 500.f;
	ShootCooldown = 0.f;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ALaserGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaserGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALaserGun::Shoot(AController* Shooter, bool Begin)
{
	if(ASCharacter* ShooterCharacter = Cast<ASCharacter>(Shooter->GetPawn()))
	{
		UCameraComponent* ShooterCamera = ShooterCharacter->GetCamera();

		FVector TraceStart1 = ShooterCamera->GetComponentLocation();
		FVector TraceEnd1 = TraceStart1 + ShooterCamera->GetForwardVector() * ShootDistance;
		FHitResult HitResult1;
		FCollisionQueryParams QueryParams1;
		QueryParams1.AddIgnoredActor(this);
		QueryParams1.AddIgnoredActor(ShooterCharacter);
		GetWorld()->LineTraceSingleByChannel(HitResult1, TraceStart1, TraceEnd1, ECC_Visibility, QueryParams1);
		
		FVector EndPoint1;
		if (HitResult1.GetActor())
		{
			EndPoint1 = HitResult1.ImpactPoint;
			//UE_LOG(LogTemp, Error, TEXT("%s"), *HitResult1.GetActor()->GetName());
		}
		else
		{
			EndPoint1 = HitResult1.TraceEnd;
		}
		//DrawDebugLine(GetWorld(), HitResult1.TraceStart, EndPoint1, FColor::Green, false, 1.f);
		
		FVector TraceStart = Arrow->GetComponentLocation();
		FVector TraceEnd = EndPoint1 + (EndPoint1-TraceStart)/(EndPoint1-TraceStart).Size()*2;
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(ShooterCharacter);
		
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
		
		FVector EndPoint;
		
		if (HitResult.GetActor())
		{
			EndPoint = HitResult.ImpactPoint;
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, Shooter, this, DamageType);
		}
		else
		{
			EndPoint = HitResult.TraceEnd;
		}
		EffectLocation = EndPoint;
		GunNoise();

		FVector LengthVector = EndPoint - HitResult.TraceStart;
		UKismetMathLibrary::Vector_Normalize(LengthVector, 0.0001);
		if(Begin)
		{
			CurrentLaser = GetWorld()->SpawnActor<ALaserBullet>(LaserClass, HitResult.TraceStart, UKismetMathLibrary::MakeRotFromZ(LengthVector));
		}
		if(CurrentLaser)
		{
			CurrentLaser->SetActorLocation(HitResult.TraceStart);
			CurrentLaser->SetActorRotation(UKismetMathLibrary::MakeRotFromZ(LengthVector));
			FVector NewScale = FVector(0.05,0.05,FVector::Distance(HitResult.TraceStart, EndPoint)/100);
			CurrentLaser->SetScale(NewScale);
			CurrentLaser->RenewFading();
		}

		//DrawDebugLine(GetWorld(), HitResult.TraceStart, EndPoint, FColor::Green, false, 1.f);
	}
	
}

void ALaserGun::SpawnEffect_Implementation(bool IsLv2Required)
{
	if(!IsLv2Required)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Effect,
			EffectLocation, FRotator(0,0,0), FVector(0.25, 0.25, 0.25), true,
			true, ENCPoolMethod::None, true);
	}
	else
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Lv2RequiredEffect,
			EffectLocation, FRotator(0,0,0), FVector(0.25, 0.25, 0.25), true,
			true, ENCPoolMethod::None, true);
	}
}

void ALaserGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALaserGun, EffectLocation);
}



// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SO2.h"

#include <string>

#include "Components/SHealth.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USO2::USO2()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MaxO2Volume = 360.f;
	O2Volume = 360.f;
	ZoneInterval = 0.5f;
	DepleteRate = 1.f;
	DepleteDamage = 2.f;
	RefillRate = 10.f;
	bIsInO2FreeZone = true;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void USO2::BeginPlay()
{
	Super::BeginPlay();
	MyHealth = GetOwner()->FindComponentByClass<USHealth>();
	MyOwner = Cast<ASCharacter>(GetOwner());
}

void USO2::ZoneDepleteO2()
{
	if(O2Volume > 0)
	{
		LastCheckTime = GetWorld()->GetTimeSeconds();
		DepleteO2_Internal(DepleteRate);
	}
	else if(O2Volume <= 0)
	{
		LastCheckTime = GetWorld()->GetTimeSeconds();
		MyHealth->Health = FMath::Clamp(MyHealth->Health-DepleteDamage,0.f,MyHealth->DefaultHealth);
		if(MyHealth->Health ==  0.f)
		{
			ASCharacter* Owner = Cast<ASCharacter>(GetOwner());
			Owner->HandleDeath(Owner);
		}
	}
}

void USO2::ZoneRefillO2()
{
	if(O2Volume<MaxO2Volume)
	{
		LastCheckTime = GetWorld()->GetTimeSeconds();
		RefillO2_Internal(RefillRate);
	}
}

void USO2::SetO2Volume_Implementation(const float Volume)
{
	O2Volume = FMath::Clamp(Volume,0.f,MaxO2Volume);
}

//return actual refill volume
float USO2::RefillO2(const float RefillVolume)
{
	return RefillO2_Internal(RefillVolume);
}

//return actual refill volume
float USO2::RefillO2_Internal(const float RefillVolume)
{
	if(O2Volume >= MaxO2Volume)
	{
		return 0.f;
	}
	if(O2Volume + RefillVolume > MaxO2Volume)
	{
		const float ActualRefill = MaxO2Volume - O2Volume;
		SetO2Volume(MaxO2Volume);
		return ActualRefill;
	}
	if(O2Volume + RefillVolume <= MaxO2Volume)
	{
		SetO2Volume(O2Volume + RefillVolume);
		return RefillVolume;
	}
	return 0.f;
}

float USO2::DepleteO2_Internal(const float DepleteVolume)
{
	if(O2Volume <= 0)
	{
		return 0.f;
	}
	if(DepleteVolume > O2Volume)
	{
		const float ActualDepleteVolume = O2Volume;
		SetO2Volume(0);
		return O2Volume;
	}
	if(DepleteVolume <= O2Volume)
	{
		SetO2Volume(O2Volume - DepleteVolume);
		return DepleteVolume;
	}
	return 0.f;
}

// Called every frame
void USO2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(GetOwner()->HasAuthority())
	{
		if(GetWorld()->TimeSince(LastCheckTime)>ZoneInterval)
		{
			if(!bIsInO2FreeZone) ZoneDepleteO2();
			else ZoneRefillO2();
		}
	}
}

void USO2::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USO2, O2Volume);
}
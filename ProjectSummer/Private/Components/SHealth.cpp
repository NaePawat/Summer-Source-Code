// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealth.h"
#include "Gun.h"
#include "SCharacter.h"
#include "Components/SO2.h"
#include "FamageType/GunDamageType.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealth::USHealth()
{
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USHealth::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealth::HandleTakeAnyDamage);
	}
	
}

void USHealth::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(DamageType->IsA(UGunDamageType::StaticClass()))
	{
		if (Damage <= 0.0f) return;

		//update health clamped
		ASCharacter* Owner = Cast<ASCharacter>(GetOwner());
		USO2* OwnerO2 = Owner->FindComponentByClass<USO2>();
		if((Owner->bIsInvincible || OwnerO2->bIsInO2FreeZone) && OwnerO2) return;
		Health = FMath::Clamp(Health - Damage, 0.0f,DefaultHealth);
		if(Health == 0.0f)
		{
			Owner->HandleDeath(DamageCauser);
			Owner->HandleDeathSetup();
		}

		//UE_LOG(LogTemp, Log, TEXT("Health changed: %s"), *FString::SanitizeFloat(Health));
	}
}

void USHealth::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f) return;

	//update health clamped
	const ASCharacter* Owner = Cast<ASCharacter>(GetOwner());
	if(Owner->bIsInvincible) return;
	Health = FMath::Clamp(Health + HealAmount, 0.0f,DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health changed: %s"), *FString::SanitizeFloat(Health));
}

void USHealth::ServerHealth_Implementation(float StartHealth)
{
	Health=StartHealth;
}

void USHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealth, DefaultHealth);
	DOREPLIFETIME(USHealth, Health);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SDecoyCharacter.h"
#include "SecCam.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "SRole.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWidgetHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProjectileHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathHandle,ASCharacter*,Other);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFindEvidenceHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayersLO);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDashWarpEffectHandle,FHitResult, HitComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOctaneHandle,bool, Run);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSUMMER_API USRole : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	USRole();

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float DefaultSpeed;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float WarpDistance;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float WarpDelay;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float FastSpeed;

	float AddFastSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float SprintTime;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float InvincTime;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	FVector EndLinePos;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	FVector EndParabolicPos;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	TSubclassOf<ASDecoyCharacter> ActorToSpawn;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	TSubclassOf<ASecCam> CameraToSpawn;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float ThrowDistance;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float DetectiveCheckDis;

	UPROPERTY(EditDefaultsOnly,Category="Ability")
	float ShowHealthTime;

	UPROPERTY()
	AActor* OwnerEquipping;

	UPROPERTY()
	ASCharacter* OwnerCharacter;

	UPROPERTY(BlueprintReadOnly)
	TArray<ASCharacter*> PlayerPawns;
	
	FTimerHandle AbilityTimer;
	FTimerHandle ShowHealthTimer;

	UFUNCTION()
	void ResetWalkSpeed();

	UFUNCTION()
	void ResetShowMinnaSanHealth();

	UFUNCTION()
	void Warp();

	UFUNCTION(Server, Reliable)
	void ServerSetHitComp();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FHitResult HitComp;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FHitResult ProjectileComp;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Role")
	FString NameRole;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Role")
	int Team;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Ability")
	float AbilityCooldown;

	UPROPERTY(BlueprintReadWrite, Category="Ability")
	float ThrowPitchAngle;

	UPROPERTY(BlueprintReadWrite, Category="Ability")
	float LaunchPower;

	UPROPERTY(BlueprintReadOnly)
	int CamNum;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> ActorsToIgnore;
	
	UFUNCTION()
	void UseAbility();

	UPROPERTY(BlueprintReadWrite, Replicated)
	int Index;

	UPROPERTY(BlueprintAssignable)
	FWidgetHandle WidgetHandle;

	UPROPERTY(BlueprintAssignable)
	FDeathHandle DeathHandle;

	UPROPERTY(BlueprintAssignable)
	FPlayersLO PlayersLoHandle;

	UPROPERTY(BlueprintReadWrite, Category="Ability")
	bool ActivateLineTrace;

	UPROPERTY(BlueprintReadWrite, Category="Ability")
	bool ActivateParabolicTrace;

	UFUNCTION(BlueprintCallable)
	void ApplyMedic();

	UPROPERTY(BlueprintAssignable)
	FThrowHandle ThrowHandle;

	UPROPERTY(BlueprintAssignable)
	FFindEvidenceHandle FindEvidenceHandle;

	UPROPERTY(BlueprintAssignable)
	FProjectileHandle ProjectileHandle;

	UPROPERTY(BlueprintAssignable)
	FDashWarpEffectHandle DashWarpEffectHandle;
	
	UPROPERTY(BlueprintAssignable)
	FOctaneHandle OctaneHandle;

	UFUNCTION(BlueprintCallable)
	void Deploy();

	UFUNCTION(BlueprintCallable)
	void SetIndex(int NewIndex);

	UFUNCTION(Server, Reliable)
	void SetIndexServer(int NewIndex);

	UFUNCTION(Client, Reliable)
	void ShowMinnaSanHealth(const TArray<ASCharacter*>& TargetPlayers);
};

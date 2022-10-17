// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Components/SRole.h"
#include "SGameModeBase.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASGameModeBase();

	//call on server -> run to all client
	//UFUNCTION(NetMulticast, Reliable)
	void AssignRoleToPlayer();

	UFUNCTION(BlueprintCallable)
	void EndGame(bool Result);

	UFUNCTION()
	float CheckHealthFromRole(USRole* CharacterRole);

	UFUNCTION()
	float CheckAbiFromRole(USRole* CharacterRole);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void StartTimer();

	void CheckPlayersJoin();

	void SetPlayerCount();

	int GoodGuy;
	int BadGuy;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<AActor> playerCharacter;

	TArray<FString> Roles;
	int32 Werewolf_Num;
	bool assignRole;
};

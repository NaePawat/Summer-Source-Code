// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "Components/SRole.h"
#include "Kismet/GameplayStatics.h"
#include "SGameStateBase.h"
#include "Components/SHealth.h"

//set default value
ASGameModeBase::ASGameModeBase()
{
	Roles = { "Warper" ,"Octane","Bamboozler","Medic","SecOP","Detective"};
	
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	GameStateClass = ASGameStateBase::StaticClass();
}

void ASGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	assignRole = false;
	
}

void ASGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckPlayersJoin();
}

void ASGameModeBase::CheckPlayersJoin()
{
	if (GetNumPlayers() > 1 && !assignRole) {
		AssignRoleToPlayer();
		assignRole = true;
		StartTimer();
		SetPlayerCount();
	}
}

void ASGameModeBase::SetPlayerCount()
{
	ASGameStateBase* GS = GetGameState<ASGameStateBase>();
	if(GS)
	{
		GS->GoodGuyCount = GoodGuy;
		GS->BadGuyCount  = BadGuy;
	}
}

void ASGameModeBase::AssignRoleToPlayer()
{
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerCharacter , Players);
	if (Players.Num() == 6) Werewolf_Num = 2; else Werewolf_Num = 1;
	if (Players.Num() > 0) {
		for (int i = 0; i < Players.Num(); i++) {
			USRole* CharacterRole = Players[i]->FindComponentByClass<USRole>();
			USHealth* CharacterHealth = Players[i]->FindComponentByClass<USHealth>();
			int32 temp = FMath::RandRange(0, Roles.Num() - 1);
			int32 TeamIndex;
			if(Werewolf_Num != 0)
			{
				if((i==Players.Num()-1 && Werewolf_Num ==1) ||(i==Players.Num()-2&&Werewolf_Num==2))
				{
					TeamIndex = 1;
				}
				else TeamIndex = FMath::RandRange(0,1);
			}
			else
			{
				TeamIndex = 0;
				GoodGuy++;
			}
			if(TeamIndex == 1)
			{
				//UE_LOG(LogTemp, Log, TEXT("There's a werewolf!"));
				Werewolf_Num -=1;
				BadGuy++;
			}
			CharacterRole->NameRole = Roles[temp];
			CharacterRole->Team = TeamIndex;
			CharacterHealth->DefaultHealth = CheckHealthFromRole(CharacterRole);
			CharacterHealth->ServerHealth(CharacterHealth->DefaultHealth);
			CharacterRole->AbilityCooldown = CheckAbiFromRole(CharacterRole);
		}
	}
}

void ASGameModeBase::EndGame(bool Result)
{
	ASGameStateBase* GS = GetGameState<ASGameStateBase>();
	if(GS)
	{
		GS->EndGameResult(Result);
	}
}

float ASGameModeBase::CheckHealthFromRole(USRole* CharacterRole)
{
	if (CharacterRole->NameRole == "Warper") {
		return 100.f;
	}
	if (CharacterRole->NameRole == "Octane") {
		return 80.f;
	}
	if (CharacterRole->NameRole == "Bamboozler") {
		return 80.f;
	}
	if (CharacterRole->NameRole == "Medic") {
		return 60.f;
	}
	if(CharacterRole->NameRole=="SecOP")
	{
		return 60.f;
	}
	if(CharacterRole->NameRole=="Detective")
	{
		return 80.f;
	}
	return 0.f;
}

float ASGameModeBase::CheckAbiFromRole(USRole* CharacterRole)
{
	if (CharacterRole->NameRole == "Warper") {
		return 10.f;
	}
	if (CharacterRole->NameRole == "Octane") {
		return 8.f;
	}
	if (CharacterRole->NameRole == "Bamboozler") {
		return 6.f;
	}
	if (CharacterRole->NameRole == "Medic") {
		return 8.f;
	}
	if (CharacterRole->NameRole == "SecOP") {
		return 8.f;
	}
	if(CharacterRole->NameRole=="Detective")
	{
		return 6.f;
	}
	return 0.f;
}

void ASGameModeBase::StartTimer()
{
	ASGameStateBase* GS = GetGameState<ASGameStateBase>();
	if (GS) {
		GS->StartWorldTimer();
	}
}


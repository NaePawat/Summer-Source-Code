// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacter.h"
#include "GameFramework/Actor.h"
#include "VoteManager.generated.h"

UCLASS()
class PROJECTSUMMER_API AVoteManager : public AInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoteManager();

	//component
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bOngoingVote;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 Yes = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 No = 0;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="VoteSys")
	ASCharacter* VoteTarget;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="VoteSys")
	ASCharacter* VoteInitializer;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="VoteSys")
	int32 PlayerLength;

	//timer
	FTimerHandle VoteTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VoteSys")
	float VoteTime;

	UPROPERTY(Replicated,BlueprintReadOnly,Category="VoteSys")
	float VoteTimeRemaining;

	//UFUNCTION(NetMulticast,Reliable)
	void SetVoteTimer();

	virtual void OnInteract(class ACharacter* Character) override;

	UFUNCTION(BlueprintImplementableEvent)
	void KickBanGuy(ASCharacter* TargetCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void VoteBeginWidgetHandle();

	UFUNCTION(NetMulticast, Reliable)
	void VoteEndWidgetHandle();

	UFUNCTION(BlueprintImplementableEvent)
	void VoteBeginWidgetHandleEvent();
	
	UFUNCTION(BlueprintImplementableEvent)
    void VoteEndWidgetHandleEvent();

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<ASCharacter*> BannedCharacters;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void EndVote();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API ASGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ASGameStateBase();

	UFUNCTION(NetMulticast,BlueprintCallable, Reliable)
	void StartWorldTimer();

	void EndGame();

	UFUNCTION(NetMulticast,BlueprintCallable, Reliable)
	void EndGameResult(bool Result);

	UFUNCTION(BlueprintImplementableEvent)
	void TravelTo();

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	FTimerHandle GameTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timer")
	float GameTime;

	UPROPERTY(Replicated,BlueprintReadOnly,Category="Timer")
	float GameTimeRemaining;

	UPROPERTY(Replicated,BlueprintReadWrite,Category="PlayerCount")
	int32 BadGuyCount;

	UPROPERTY(Replicated,BlueprintReadWrite,Category="PlayerCount")
	int32 GoodGuyCount;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="DayNight")
	bool bIsNightTime;

	UPROPERTY(Replicated,BlueprintReadOnly, Category="WinState")
	bool bWinResult;
	
	UFUNCTION(BlueprintCallable, Category="DayNight")
	void SetIsNightTime(bool Value);

	UFUNCTION(BlueprintImplementableEvent, Category="DayNight")
	void OnRep_IsNightTime();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DayNight")
	float DawnPercentage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DayNight")
	float DayPeriod;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DayNight")
	float DayPercentage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DayNight")
	float DuskPercentage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DayNight")
	float NightPercentage;

protected:
	virtual void Tick(float DeltaTime) override;
	
};

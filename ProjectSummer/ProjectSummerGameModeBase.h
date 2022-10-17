// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectSummerGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSUMMER_API AProjectSummerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void AssignRole(AActor* Requester);
};

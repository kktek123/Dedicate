// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CGameMode.h"
#include "CPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATE_API ACPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACPlayerState(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(Replicated)
		float Health;

	UPROPERTY(Replicated)
		float Death;

	UPROPERTY(Replicated)
		ETeam Team;
};

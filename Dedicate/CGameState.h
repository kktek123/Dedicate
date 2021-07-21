// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CGameState.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATE_API ACGameState : public AGameState
{
	GENERATED_BODY()
public:
	ACGameState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:
	UPROPERTY(Replicated)
		bool bInMenu;
};

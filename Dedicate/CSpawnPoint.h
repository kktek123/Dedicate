// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGameMode.h"
#include "CSpawnPoint.generated.h"

UCLASS()
class DEDICATE_API ACSpawnPoint : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ETeam Team;

public:
	ACSpawnPoint();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	bool GetBlocked() { return OverlappingActors.Num() != 0; }

	ETeam GetTeam() { return Team; }

private:
	UFUNCTION()
		void ActorBeginOverlap(AActor* OverlapActor, AActor* OtherActor);

	UFUNCTION()
		void ActorEndOverlap(AActor* OverlapActor, AActor* OtherActor);

private:
	class UCapsuleComponent* SpawnCapsule;
	TArray<AActor *> OverlappingActors;
};

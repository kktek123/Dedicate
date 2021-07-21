// Fill out your copyright notice in the Description page of Project Settings.


#include "ACPlayer.h"

// Sets default values
AACPlayer::AACPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AACPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DedicateGameMode.h"
#include "DedicateHUD.h"
#include "DedicateCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADedicateGameMode::ADedicateGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADedicateHUD::StaticClass();
}

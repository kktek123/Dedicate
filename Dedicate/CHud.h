// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CHud.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATE_API ACHud : public AHUD
{
	GENERATED_BODY()

public:
	ACHud();
	virtual void DrawHUD() override;

private:
	class UTexture2D* CrosshairTex;
	
};

#pragma once

#include "CLog.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

#include "Particles/ParticleSystemComponent.h"

//#include "BehaviorTree/BehaviorTree.h"
//#include "BehaviorTree/BehaviorTreeComponent.h"
//#include "BehaviorTree/BlackboardComponent.h"
//#include "BehaviorTree/BlackboardData.h"

#include "Animation/AnimInstance.h"7/

#define RetNull(p){ if(p == NULL) return ; }
#define RetNullResult(p, result){ if(p == NULL) return result; }

#define CreateTextRender(parent, text, height)\
{\
	Text = CreateDefaultSubobject<UTextRenderComponent>("Text");\
	Text->SetupAttachment(parent);\
	\
	Text->SetRelativeLocation(FVector(0, 0, height));\
	Text->SetRelativeRotation(FRotator(0, 180, 0));\
	Text->TextRenderColor = FColor::Red;\
	Text->Text = FText::FromString(text);\
	Text->HorizontalAlignment = EHorizTextAligment::EHTA_Center;\
}
#include "CSpawnPoint.h"
#include "Global.h"

ACSpawnPoint::ACSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnCapsule = CreateDefaultSubobject<UCapsuleComponent>(L"Capsule");
	SpawnCapsule->SetCollisionProfileName("OverlapAllDynamic");
	SpawnCapsule->SetGenerateOverlapEvents(true);
	SpawnCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &ACSpawnPoint::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACSpawnPoint::ActorEndOverlap);
}

void ACSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnCapsule->UpdateOverlaps();
}

void ACSpawnPoint::OnConstruction(const FTransform & Transform)
{
	if (Team == ETeam::Red)
		SpawnCapsule->ShapeColor = FColor(255, 0, 0);
	else if (Team == ETeam::Blue)
		SpawnCapsule->ShapeColor = FColor(0, 0, 255);
}

void ACSpawnPoint::ActorBeginOverlap(AActor * OverlapActor, AActor * OtherActor)
{
	if (Role == ROLE_Authority)
	{
		if (OverlappingActors.Find(OtherActor) < 0)
			OverlappingActors.Add(OtherActor);
	}
}

void ACSpawnPoint::ActorEndOverlap(AActor * OverlapActor, AActor * OtherActor)
{
	if (Role == ROLE_Authority)
	{
		if (OverlappingActors.Find(OtherActor) >= 0)
			OverlappingActors.Remove(OtherActor);
	}
}
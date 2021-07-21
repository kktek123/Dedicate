#include "CPlayerState.h"
#include "Net/UnrealNetwork.h"


ACPlayerState::ACPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	Death = 0;

	Team = ETeam::Blue;
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayerState, Health);
	DOREPLIFETIME(ACPlayerState, Death);
	DOREPLIFETIME(ACPlayerState, Team);
}
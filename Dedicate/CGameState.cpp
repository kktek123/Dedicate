#include "CGameState.h"
#include "Net/UnrealNetwork.h"

ACGameState::ACGameState()
{
	bInMenu = false;
}

void ACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGameState, bInMenu);
}

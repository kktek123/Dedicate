#include "CGameMode.h"
#include "Global.h"
#include "CPlayer.h"
#include "CHud.h"
#include "CGameState.h"
#include "CPlayerState.h"
#include "CSpawnPoint.h"

bool ACGameMode::bInGameMenu = true;

ACGameMode::ACGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BpCPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ACHud::StaticClass();


	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = ACPlayerState::StaticClass();
}

void ACGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		for (TActorIterator<ACSpawnPoint> iter(GetWorld()); iter; ++iter)
		{
			if ((*iter)->GetTeam() == ETeam::Red)
				RedTeamSpawns.Add(*iter);
			else if ((*iter)->GetTeam() == ETeam::Blue)
				BlueTeamSpawns.Add(*iter);
		}

		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if (controller != NULL)
		{
			ACPlayer* player = Cast<ACPlayer>(controller->GetPawn());
			player->SetTeam(ETeam::Blue);
			BlueTeams.Add(player);

			Spawn(player);
		}
		Cast<ACGameState>(GameState)->bInMenu = bInGameMenu;
	}
}

void ACGameMode::Tick(float DeltaSeconds)
{

	if (Role == ROLE_Authority)
	{
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if (ToBeSpawns.Num() != 0)
		{
			for (ACPlayer* player : ToBeSpawns)
				Spawn(player);
		}
		if (controller != NULL && controller->IsInputKeyDown(EKeys::R))
		{
			bInGameMenu = false;
			GetWorld()->ServerTravel(L"/Game/Level?");

			Cast<ACGameState>(GameState)->bInMenu = bInGameMenu;
		}
	}

}

void ACGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACPlayer* player = Cast<ACPlayer>(NewPlayer->GetPawn());
	ACPlayerState* playerState = Cast<ACPlayerState>(NewPlayer->PlayerState);

	if (player != NULL && playerState != NULL)
		player->SetPlayerState(playerState);

	if (player != NULL && Role == ROLE_Authority)
	{
		if (BlueTeams.Num()>RedTeams.Num())
		{
			RedTeams.Add(player);
			playerState->Team = ETeam::Red;
		}
		else if (BlueTeams.Num() < RedTeams.Num())
		{
			BlueTeams.Add(player);
			playerState->Team = ETeam::Blue;
		}
		else
		{
			BlueTeams.Add(player);
			playerState->Team = ETeam::Blue;
		}

		player->CurrentTeam = playerState->Team;
		player->SetTeam(playerState->Team);

		Spawn(player);

	}

}

void ACGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Quit && EndPlayReason == EEndPlayReason::EndPlayInEditor)
		bInGameMenu = true;
}

void ACGameMode::Spawn(ACPlayer * Player)
{
	if (Role == ROLE_Authority)
	{
		ACSpawnPoint* point = NULL;
		TArray <ACSpawnPoint *>*  targetTeam = NULL;

		if (Player->CurrentTeam == ETeam::Blue)
		{
			targetTeam = &BlueTeamSpawns;
		}
		else
		{
			targetTeam = &RedTeamSpawns;
		}

		for (ACSpawnPoint* point : *targetTeam)
		{
			if (point->GetBlocked() == false)
			{
				if (ToBeSpawns.Find(Player) >= 0)
					ToBeSpawns.Remove(Player);

				Player->SetActorLocation(point->GetActorLocation());
				point->UpdateOverlaps();

				return;
			}
		}

		if (ToBeSpawns.Find(Player) < 0)
		{
			ToBeSpawns.Add(Player);
		}
	}
}

void ACGameMode::Respawn(ACPlayer * Player)
{
	if (Role == ROLE_Authority)
	{
		AController* controller = Player->GetController();
		Player->DetachFromControllerPendingDestroy();

		ACPlayer* player = Cast<ACPlayer>(GetWorld()->SpawnActor(DefaultPawnClass));
		if (player != NULL)
		{
			controller->Possess(player);
			ACPlayerState* playerState = Cast<ACPlayerState>(player->GetController()->PlayerState);
		
			player->CurrentTeam = playerState->Team;
			player->SetSelfPlayerState(playerState);

			Spawn(player);

			player->SetTeam(playerState->Team);
		}
	}
}



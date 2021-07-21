#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CGameMode.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Blue, Red,
};

UCLASS(minimalapi)
class ACGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Respawn(class ACPlayer* Player);
	void Spawn(class ACPlayer* Player);

private:
	TArray<class ACPlayer *> RedTeams;
	TArray<class ACPlayer *> BlueTeams;
	TArray<class ACPlayer *> ToBeSpawns;

	TArray<class ACSpawnPoint *> RedTeamSpawns;
	TArray<class ACSpawnPoint *> BlueTeamSpawns;
	//TArray<class ACSpawnPoint *> ToBeSpawns;

	bool bGameStarted;
	static bool bInGameMenu;
};
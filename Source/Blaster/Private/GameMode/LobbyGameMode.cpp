// Blaster project. All rights reserved.


#include "GameMode/LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const auto GameStatePtr = GameState.Get();
	if (!GameStatePtr) return;

	if (const int32 PlayersNum = GameStatePtr->PlayerArray.Num(); PlayersNum == 2)
	{
		UWorld* World = GetWorld();
		if (!World) return;

		bUseSeamlessTravel = true;
		World->ServerTravel(TEXT("/Game/Blaster/Maps/BlasterMap?listen"));
	}
}

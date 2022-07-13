// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABPlayerController.h"
#include "ABCharacter.h"
#include "ABPlayerState.h"
#include "ABGameState.h"


AABGameMode::AABGameMode()
{
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameState::StaticClass();
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData();
}


void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABGameState = Cast<AABGameState>(GameState);
}


void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
{
	for(FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		const auto ABPlayerController = Cast<AABPlayerController>(it->Get());
		if(ABPlayerController && ScoredPlayer == ABPlayerController)
		{
			ABPlayerController->AddGameScore();
			break;
		}
	}

	ABGameState->AddGameScore();
}

int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}

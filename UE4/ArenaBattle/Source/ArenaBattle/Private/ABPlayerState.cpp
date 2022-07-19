// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"


AABPlayerState::AABPlayerState()
	: GameScore(0)
	, CharacterLevel(1)
	, Exp(0)
	, SaveSlotName(TEXT("Player1"))
	, GameHighScore(0)
{
}


int32 AABPlayerState::GetGameScore() const
{
	return GameScore;
}


int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}


int32 AABPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}


float AABPlayerState::GetExpRatio() const
{
	if(CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		return 0.0f;

	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	return Result;
}


bool AABPlayerState::AddExp(const int32& IncomeExp)
{
	if(CurrentStatData->NextExp == -1)
	{
		return false;
	}
	else
	{
		bool DidLevelUp = false;
		Exp = Exp + IncomeExp;

		if(Exp >= CurrentStatData->NextExp)
		{
			Exp = Exp - CurrentStatData->NextExp;
			SetCharacterLevel(CharacterLevel);
			CharacterLevel++;
			DidLevelUp = true;
		}

		OnPlayerStateChanged.Broadcast();
		SavePlayerData();
		return DidLevelUp;
	}
}


void AABPlayerState::AddGameScore()
{
	GameScore++;
	if(GameScore >= GameHighScore)
	{
		GameHighScore = GameScore;
	}
	OnPlayerStateChanged.Broadcast();
	SavePlayerData();
}


void AABPlayerState::SetCharacterLevel(const int32& NewCharacterLevel)
{
	auto gameInstance = GetGameInstance();
	
	const auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	CurrentStatData = ABGameInstance->GetAbCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}


void AABPlayerState::InitPlayerData()
{
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if(ABSaveGame == nullptr)
	{
		ABSaveGame = GetMutableDefault<UABSaveGame>();
	}

	SetPlayerName(ABSaveGame->PlayerName);
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	SavePlayerData();
}


void AABPlayerState::SavePlayerData()
{
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;

	if(!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))
	{
		ABLOG(Error, TEXT("Game Save Failed."));
	}
}

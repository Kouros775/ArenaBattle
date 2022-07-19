// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"


struct FABCharacterData;


DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AABPlayerState();

	int32 GetGameScore() const;
	int32 GetGameHighScore() const;
	
	int32 GetCharacterLevel() const;


	

	float GetExpRatio() const;
	bool AddExp(const int32& IncomeExp);

	void AddGameScore();
	
	void InitPlayerData();
	void SavePlayerData();
	
	FOnPlayerStateChangedDelegate OnPlayerStateChanged;

private:
	void SetCharacterLevel(const int32& NewCharacterLevel);
	 FABCharacterData* CurrentStatData;

	
	// variables
protected:
	UPROPERTY(Transient)
	int32 GameScore;

	UPROPERTY(Transient)
	int32 CharacterLevel;

	UPROPERTY(Transient)
	int32 Exp;

	FString SaveSlotName;

	UPROPERTY(Transient)
	int32 GameHighScore;
};

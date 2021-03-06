// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"


/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AABGameState();

	int32 GetTotalGameScore() const;
	void AddGameScore();

	
	// variables
private:
	UPROPERTY(Transient)
	int32 TotalGameScore;
};

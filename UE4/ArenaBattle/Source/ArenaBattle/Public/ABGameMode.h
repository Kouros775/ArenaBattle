// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

class AABGameState;
class AABPlayerController;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	explicit AABGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostInitializeComponents() override;
	void AddScore(AABPlayerController* ScoredPlayer);


	// variables
private:
	AABGameState* ABGameState;
};

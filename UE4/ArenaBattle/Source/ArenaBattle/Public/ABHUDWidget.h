// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABHUDWidget.generated.h"


class UABCharacterStatComponent;
class AABPlayerState;
class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void BindCharacterStat(UABCharacterStatComponent* CharacterStat);
	void BindPlayerState(AABPlayerState* PlayerState);

protected:
	virtual void NativeConstruct() override;
	void UpdateCharacterStat();
	void UpdatePlayerState();

	// variables
private:
	TWeakObjectPtr<UABCharacterStatComponent> CurrentCharacterStat;
	TWeakObjectPtr<AABPlayerState> CurrentPlayerState;

	UPROPERTY()
	UProgressBar* HPBar;

	UPROPERTY()
	UProgressBar* ExpBar;

	UPROPERTY()
	UTextBlock* PlayerName;

	UPROPERTY()
	UTextBlock* PlayerLevel;

	UPROPERTY()
	UTextBlock* CurrentScore;
	
	UPROPERTY()
	UTextBlock* HighScore;
};

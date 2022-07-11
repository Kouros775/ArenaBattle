// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"


/**
 * 
 */


class UABCharacterStatComponent;
class UProgressBar;

UCLASS()
class ARENABATTLE_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacterStat(UABCharacterStatComponent* NewCharacterStat);

protected:
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

	
	// variables
private:
	TWeakObjectPtr<UABCharacterStatComponent> CurrentCharacterStat;
	UPROPERTY()
	UProgressBar* HPProgressBar;
};

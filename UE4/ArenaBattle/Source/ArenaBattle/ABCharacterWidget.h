// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"


/**
 * 
 */


class UABCharacterStartComponent;
class UProgressBar;

UCLASS()
class ARENABATTLE_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacterStat(UABCharacterStartComponent* NewCharacterStat);

protected:
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

	
	// variables
private:
	TWeakObjectPtr<UABCharacterStartComponent> CurrentCharacterStat;
	UPROPERTY()
	UProgressBar* HPProgressBar;
};

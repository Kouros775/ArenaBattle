// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"


class UABHUDWidget;
class AABCharacter;
class AABPlayerState;


/**
 * 
 */


UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AABPlayerController();

	UABHUDWidget* GetHUDWidget() const;
	void NPCKill(const AABCharacter* KilledNPC) const;
	void AddGameScore() const;
protected:
	virtual void BeginPlay() override;


	// variables;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
	TSubclassOf<UABHUDWidget> HUDWidgetClass;

	UPROPERTY()
	UABHUDWidget* HUDWidget;

	UPROPERTY()
	AABPlayerState* ABPlayerState;
};

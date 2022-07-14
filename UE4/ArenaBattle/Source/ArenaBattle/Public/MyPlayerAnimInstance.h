// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyPlayerAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class ARENABATTLE_API UMyPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UMyPlayerAnimInstance();

	virtual void NativeUpdateAnimation(const float DeltaSeconds) override;

	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// variables
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimStat)
	float _fRunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimStat)
	bool _bIsDead;
};

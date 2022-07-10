// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStartComponent.generated.h"


struct FABCharacterData;


DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStartComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStartComponent();

	void SetNewLevel(const int32& NewLevel);
	void SetDamage(const float& NewDamage);
	float GetAttack() const;
	
	FOnHPIsZeroDelegate OnHPIsZero;

	void SetHP(const float& NewHP);
	float GetHPRatio() const;

	FOnHPChangedDelegate OnHPChanged;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponent() override;

private:
	FABCharacterData* CurrentStatData;

	UPROPERTY(EditInstanceOnly, Category=Stat, meta=(AllowPrivateAccess=true))
	int32 Level;

	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess=true))
	float CurrentHP;
	
	// variables
public:
protected:
private:
};

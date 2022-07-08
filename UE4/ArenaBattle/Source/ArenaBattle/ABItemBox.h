// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
private:
	UFUNCTION()
	void OnCharacterOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor
		, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	


	// variable
public:
	UPROPERTY(VisibleAnywhere, Category=Box)
	UBoxComponent* Trigger;
	UPROPERTY(VisibleAnywhere, Category=Box)
	UStaticMeshComponent* Box;
	UPROPERTY(EditInstanceOnly, Category=Box)
	TSubclassOf<class AABWeapon> WeaponItemClass;
protected:
private:
};

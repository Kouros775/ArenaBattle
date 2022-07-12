// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABSection.generated.h"


UCLASS()
class ARENABATTLE_API AABSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABSection();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	enum class ESectionState : uint8
	{
		Ready = 0,
		Batte,
		Complete
	};

	void SetState(const ESectionState& NewState);
	void OperateGates(bool bOpen = true);

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
		, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep
		, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
	, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep
	, const FHitResult& SweepResult);

	
	// variables
public:
protected:
private:
	UPROPERTY(VisibleAnywhere, Category=Mesh, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category=Mesh, meta=(AllowPrivateAccess=true))
	TArray<UStaticMeshComponent*> GateMeshes;

	UPROPERTY(VisibleAnywhere, Category=Trigger, meta=(AllowPrivateAccess=true))
	TArray<UBoxComponent*> GateTriggers;

	UPROPERTY(VisibleAnywhere, Category=Trigger, meta=(AllowPrivateAccess=true))
	UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere, Category=State, meta=(AllowPrivateAccess=true))
	bool bNoBattle;

	ESectionState CurrentState;
};

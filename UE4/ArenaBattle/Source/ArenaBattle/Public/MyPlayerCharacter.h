// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayerCharacter.generated.h"

UCLASS()
class ARENABATTLE_API AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// >> 초기화 관련 함수
	void _initCamera();
	void _initSkeletalMesh() const;
	void _initCollision();
	// << 초기화 관련 함수
	
	// >> 키보드&마우스 이벤트를 통한 캐릭터의 움직임 함수
	void _upDown(const float paramAxis);
	void _leftRight(const float paramAxis);
	void _lookUp(const float NewAxisValue);
	void _turn(const float NewAxisValue);
	// << 키보드&마우스 이벤트를 통한 캐릭터의 움직임 함수

	// >> 캐릭터 액션관련 함수
	void _interact();
	// << 캐릭터 액션관련 함수
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// variables
protected:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
};


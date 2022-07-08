// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"


class UABAnimInstance;
class AABWeapon;


UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	enum class EControlMode
	{
		GTA,
		DIABLO
	};

	// Sets default values for this character's properties
	AABCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	bool CanSetWeapon();
	void SetWeapon(AABWeapon* NewWeapon);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetControlMode(const EControlMode& ControlMode);

private:
	void UpDown(const float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
	void ViewChange();
	void Attack();
	void AttackCheck();
	
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// variables
protected:
	EControlMode CurrentControlMode;
	FVector DirectionToMove;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AABWeapon* CurrentWeapon;


	float ArmLengthTo;
	FRotator ArmRotationTo;
	float ArmLengthSpeed;
	float ArmRotationSpeed;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta=(AllowPrivateAccess=true))
	bool IsAttacking;

	UPROPERTY()
	UABAnimInstance* ABAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	bool CanNextCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	bool IsComboInputOn;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	int32 MaxCombo;
	
	
	// >> Collsion Check Debug variable
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta=(AllowPrivateAccess=true))
	float AttackRange;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta=(AllowPrivateAccess=true))
	float AttackRadius;
	// << Collsion Check Debug variable
};

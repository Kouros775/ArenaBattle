// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"

#include "ABPlayerController.h"

#include "Components/WidgetComponent.h"
#include "Engine/StreamableManager.h"

#include "GameFramework/Character.h"
#include "ABPlayerCharacter.generated.h"




class AABWeapon;
class UABAnimInstance;
class UABCharacterStatComponent;


DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);


UCLASS()
class ARENABATTLE_API AABPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	enum class EControlMode
	{
		GTA
		,DIABLO
		,NPC
	};

	// Sets default values for this character's properties
	AABPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	bool CanSetWeapon() const;
	void SetWeapon(AABWeapon* NewWeapon);
	void Attack();

	int32 GetExp() const;
	float GetFinalAttackRange() const;
	float GetFinalAttackDamage() const;

	
	void SetCharacterState(const ECharacterState& paramState);
	ECharacterState GetCharacterState() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetControlMode(const EControlMode& ControlMode);

private:
	void _upDown(const float NewAxisValue);
	void _leftRight(float NewAxisValue);
	void _lookUp(float NewAxisValue);
	void _turn(float NewAxisValue);
	
	void _viewChange();
	void _attackCheck();
	
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();

	void OnAssetLoadCompleted();


	// >> Init
	void _initCamera();
	void _initUI();
	void _initMesh();
	void _initAnimation();
	void _initCharacterStat();
	void _initPhysics();
	// << Init

	AABPlayerController* _getPlayerController() const;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// variables
public:
	FOnAttackEndDelegate OnAttackEnd;

protected:
	EControlMode CurrentControlMode;
	FVector DirectionToMove;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AABWeapon* CurrentWeapon;
	UPROPERTY(VisibleAnywhere, Category=UI)
	UWidgetComponent* HPBarWidget;

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

	// >> Stat
	UPROPERTY(VisibleAnywhere, Category=Stat)
	UABCharacterStatComponent* CharacterStat;
	// << Stat


	FSoftObjectPath CharacterAssetToLoad;
	TSharedPtr<FStreamableHandle> AssetStreamingHandle;



	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess=true))
	ECharacterState CurrentState;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess=true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABAIController.h"
#include "Components/WidgetComponent.h"

#include "Engine/StreamableManager.h"
#include "GameFramework/Character.h"
#include "ABEnemyCharacter.generated.h"



class AABWeapon;
class UABAnimInstance;
class UABCharacterStatComponent;


DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);



UCLASS()
class ARENABATTLE_API AABEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABEnemyCharacter();

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


	void SetCharacterState(const ECharacterState& NewState);
	ECharacterState GetCharacterState() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetControlMode();

private:
	void AttackCheck();

	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();

	void OnAssetLoadCompleted();


	// >> Init
	void _initUI();
	void _initMesh();
	void _initAnimation();
	void _initCharacterStat();
	void _initPhysics();
	void _initController();
	// << Init


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// variables
public:
	FOnAttackEndDelegate OnAttackEnd;

protected:
	FVector DirectionToMove;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		AABWeapon* CurrentWeapon;
	UPROPERTY(VisibleAnywhere, Category = UI)
		UWidgetComponent* HPBarWidget;


	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = true))
		bool IsAttacking;

	UPROPERTY()
		UABAnimInstance* ABAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = true))
		bool CanNextCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = true))
		bool IsComboInputOn;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = true))
		int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = true))
		int32 MaxCombo;


	// >> Collsion Check Debug variable
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = true))
		float AttackRange;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = true))
		float AttackRadius;
	// << Collsion Check Debug variable

	// >> Stat
	UPROPERTY(VisibleAnywhere, Category = Stat)
		UABCharacterStatComponent* CharacterStat;
	// << Stat


	FSoftObjectPath CharacterAssetToLoad;
	TSharedPtr<FStreamableHandle> AssetStreamingHandle;



	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = true))
		ECharacterState CurrentState;


	UPROPERTY()
		AABAIController* ABAIController;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = true))
		float DeadTimer;

	FTimerHandle DeadTimerHandle = {};
};

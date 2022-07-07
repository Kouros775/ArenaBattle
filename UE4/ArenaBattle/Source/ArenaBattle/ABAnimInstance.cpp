// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
	: CurrentPawnSpeed(0.0f)
	, IsInAir(false)
	, IsDead(false)
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage'"));
	if(ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}


void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if(::IsValid(Pawn))
	{
		if(IsDead == false)
		{
			// About Run,Idle
			CurrentPawnSpeed = Pawn->GetVelocity().Size();

			// About Jump
			auto Character = Cast<ACharacter>(Pawn);
			if(Character)
			{
				IsInAir = Character->GetMovementComponent()->IsFalling();
			}
		}
		else
		{
			// nothing. 죽었음.
		}
	}
}


void UABAnimInstance::PlayAttackMontage()
{
	ABCHECK(this->IsDead == false);
	Montage_Play(AttackMontage, 1.0f);
}


void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(this->IsDead == false);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}


void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}


void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}


FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}



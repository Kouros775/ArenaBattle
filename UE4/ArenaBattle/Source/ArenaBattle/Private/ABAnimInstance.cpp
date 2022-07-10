// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"


/**
 * @brief 생성자
 */
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


/**
 * @brief 
 * @param DeltaSeconds 
 */
void UABAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
	if(::IsValid(Character))
	{
		if(IsDead == false)
		{
			// About Run,Idle
			CurrentPawnSpeed = Character->GetVelocity().Size();

			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
		else
		{
			// nothing. 죽었음.
		}
	}
}


/**
 * @brief 공격 Montage를 실행한다.
 */
void UABAnimInstance::PlayAttackMontage()
{
	ABCHECK(this->IsDead == false);
	Montage_Play(AttackMontage, 1.0f);
}


/**
 * @brief 해당하는 공격번호의 몽타주를 실행한다.
 * @param NewSection 공격몽타주의 번호
 */
void UABAnimInstance::JumpToAttackMontageSection(const int32& NewSection)
{
	ABCHECK(this->IsDead == false);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}


/**
 * @brief AttackHitCheck 애니메이션 노티파이가 호출되면 충돌을 확인한다.
 */
void UABAnimInstance::AnimNotify_AttackHitCheck() const
{
	OnAttackHitCheck.Broadcast();
}


/**
 * @brief NextAttackCheck 에니메이션 노티파이가 호출되면 다음공격으로의 상태를 확인한다.
 */
void UABAnimInstance::AnimNotify_NextAttackCheck() const
{
	OnNextAttackCheck.Broadcast();
}


/**
 * @brief 해당번호의 공격몽타주의 섹션이름을 반환
 * @param Section 섹션번호
 * @return 섹션이름
 */
FName UABAnimInstance::GetAttackMontageSectionName(const int32& Section) const
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}



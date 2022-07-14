// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerAnimInstance.h"

#include "ArenaBattle.h"
#include "MyPlayerCharacter.h"


UMyPlayerAnimInstance::UMyPlayerAnimInstance()
	: _fRunSpeed(0.0f)
	, _bIsDead(false)
{
}

void UMyPlayerAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABLOG_S(Warning);
	const AMyPlayerCharacter* pMyPlayerCharacter = Cast<AMyPlayerCharacter>(TryGetPawnOwner());

	if(IsValid(pMyPlayerCharacter))
	{
		if(_bIsDead == false) // 살아있음
		{
			_fRunSpeed = pMyPlayerCharacter->GetVelocity().Size();
		}
		else	// 죽음
		{
			
		}
	}
}

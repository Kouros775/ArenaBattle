// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

//#include "ABCharacter.h"
#include "ABPlayerCharacter.h"
#include "ABEnemyCharacter.h"


UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}


bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	const auto ControllingPawn = Cast<AABEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if(ControllingPawn == nullptr)
	{
		return false;
	}
	else
	{
		const auto Target = Cast<AABPlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
		if(Target == nullptr)
		{
			return false;
		}
		else
		{
			bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFinalAttackRange());
			return bResult;
		}
	}
}

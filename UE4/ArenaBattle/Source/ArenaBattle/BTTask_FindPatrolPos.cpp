// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}


EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 리턴값이 aborted, failed, succeeded, inprogress 이렇게 4개로 정해져있음.
	
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if(nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());

		if(nullptr == NavSystem)
		{
			return EBTNodeResult::Failed;
		}
		else
		{
			const FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AABAIController::HomePosKey);

			FNavLocation NextPatrol;

			if(NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f, NextPatrol))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(AABAIController::PatrolPosKey, NextPatrol.Location);
				return EBTNodeResult::Succeeded;
			}
			else
			{
				return EBTNodeResult::Failed;
			}
		}
	}
}

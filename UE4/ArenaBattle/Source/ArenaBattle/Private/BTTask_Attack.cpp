// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"


UBTTask_Attack::UBTTask_Attack()
	: IsAttacking(false)
{
	bNotifyTick = true;
}


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if(ABCharacter == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		ABCharacter->Attack();
		IsAttacking = true;
		ABCharacter->OnAttackEnd.AddLambda([this]()->void
		{
			IsAttacking = false;
		});
	}
	
	return EBTNodeResult::InProgress;
}


void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if(IsAttacking == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

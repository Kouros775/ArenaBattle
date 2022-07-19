// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

//#include "ABCharacter.h"
#include "ABPlayerCharacter.h"
#include "ABEnemyCharacter.h"


UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}


EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	auto ABCharacter = Cast<AABEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if(ABCharacter == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		auto Target = Cast<AABPlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));

		if(Target == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		else
		{
			FVector LookVector= Target->GetActorLocation() - ABCharacter->GetActorLocation();
			LookVector.Z = 0.0f;

			const FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
			ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));
			
			return EBTNodeResult::Succeeded;
		}
	}
}

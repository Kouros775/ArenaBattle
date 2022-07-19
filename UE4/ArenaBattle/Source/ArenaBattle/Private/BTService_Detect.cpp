// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

//#include "DrawDebugHelpers.h"

//#include "ABCharacter.h"
#include "ABPlayerCharacter.h"
#include "ABEnemyCharacter.h"


UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}


void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(ControllingPawn == nullptr)
	{
		return;
	}
	else
	{
		const UWorld* World = ControllingPawn->GetWorld();
		const FVector Center = ControllingPawn->GetActorLocation();

		if(nullptr == World)
		{
			return;
		}
		else
		{
			constexpr float DetectRadius = 600.0f;
			TArray<FOverlapResult> OverlapResults;
			const FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

			const bool bResult = World->OverlapMultiByChannel(
				OverlapResults
				, Center
				, FQuat::Identity
				, ECollisionChannel::ECC_GameTraceChannel2
				, FCollisionShape::MakeSphere(DetectRadius)
				, CollisionQueryParam
			);

			if (bResult)
			{
				for (auto const& OverlapResult : OverlapResults)
				{
					AABPlayerCharacter* ABCharacter = Cast<AABPlayerCharacter>(OverlapResult.GetActor());
					if (ABCharacter && ABCharacter->GetController()->IsPlayerController())
					{
						OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter);

						//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
						//DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
						//DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.27f);
						
						return;
					}
				}
			}

			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
			//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
		}
	}
}

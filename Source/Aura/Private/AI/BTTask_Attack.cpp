// Copyright Ashish


#include "AI/BTTask_Attack.h"

#include "AIController.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	DrawDebugSphere(GetWorld(),OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(),40.f,12,FColor::Red,true,-1);
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

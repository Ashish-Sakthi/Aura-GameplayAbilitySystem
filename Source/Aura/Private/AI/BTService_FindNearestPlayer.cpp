#include "AI/BTService_FindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return;
    APawn* OwningPawn = AIController->GetPawn();
    if (!OwningPawn) return;

    // Determine a target tag based on the pawn's current tag.
    const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

    // Find all actors with the target tag and Find the closest actor.
    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);
    float ClosestDistance = TNumericLimits<float>::Max();
    AActor* ClosestActor = nullptr;
    for (AActor* Actor : ActorsWithTag)
    {
        if (IsValid(Actor) && IsValid(OwningPawn))
        {
            const float Distance = OwningPawn->GetDistanceTo(Actor);
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestActor = Actor;
            }
        }
    }

    // Update blackboard values if a target was found
    if (ClosestActor)
    {
        BlackboardComp->SetValueAsObject(TargetToFollowSelector.SelectedKeyName, ClosestActor);
        BlackboardComp->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, ClosestDistance);
    }
}
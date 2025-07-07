// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindNearestPlayer.generated.h"

/**
 * UBTService_FindNearestPlayer is a behavior tree service that identifies the nearest player or enemy
 * based on the pawn's current tag and updates corresponding Blackboard keys with the target and its distance.
 *
 * This service is typically used to dynamically acquire the closest actor with a specified tag during AI behavior execution.
 */
UCLASS()
class AURA_API UBTService_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector TargetToFollowSelector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector DistanceToTargetSelector;
};

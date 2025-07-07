// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * @class UBTTask_Attack
 * @brief Custom behavior tree task for AI attack sequences in Unreal Engine.
 *
 * This class represents a behavior tree task node responsible for executing an attack sequence by activating
 * specific abilities using an associated ability system component. The task fetches a combat target
 * from the AI's blackboard and attempts to trigger the attack ability on the controlled actor.
 * If any required steps in the process fail, the task will return a failure result.
 *
 * @details
 * - The controlled AI pawn is retrieved through the behavior tree component's AI owner.
 * - The attack target is selected based on the blackboard key specified by the `CombatTargetSelector`.
 * - The task uses the ability system component of the controlled pawn to trigger abilities marked with
 *   the "Abilities_Attack" gameplay tag.
 * - If the combat target or ability system component is invalid, the task will report as failed.
 */
UCLASS()
class AURA_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector CombatTargetSelector;
};

// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack_Elementalist.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBTTask_Attack_Elementalist : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector CombatTargetSelector;

	UPROPERTY(EditAnywhere, Category = "AI")
	int32 MinionSpawnThreshold = 2;
};

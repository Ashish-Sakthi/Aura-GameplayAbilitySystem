// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AuraAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * @class AAuraAIController
 * @brief A controller class designed to handle AI logic within the Aura framework.
 *
 * This class inherits from AAIController and is responsible for managing the behavior
 * of AI characters. It contains functionality to work with Behavior Trees for decision-making.
 */
UCLASS()
class AURA_API AAuraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAuraAIController();

protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};

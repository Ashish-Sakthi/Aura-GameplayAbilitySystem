// Copyright Ashish


#include "AI/BTTask_Attack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AuraGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interaction/EnemyInterface.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	if (!BlackboardComponent) return EBTNodeResult::Failed;
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
	if (!ASC) return EBTNodeResult::Failed;

	// Gets the actor to turn to while attacking by casting the blackboard value.
	AActor* CombatTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(CombatTargetSelector.SelectedKeyName));
	if (!IsValid(CombatTarget)) return EBTNodeResult::Failed;

	IEnemyInterface::Execute_SetCombatTarget(ControlledPawn, CombatTarget);
	
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FAuraGameplayTags::Get().Abilities_Attack);

	ASC->TryActivateAbilitiesByTag(TagContainer);
	return EBTNodeResult::Succeeded;

	//will fail until ranged attack is set.
	/*const bool bSucceeded = ASC->TryActivateAbilitiesByTag(TagContainer);
	return bSucceeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;*/
}

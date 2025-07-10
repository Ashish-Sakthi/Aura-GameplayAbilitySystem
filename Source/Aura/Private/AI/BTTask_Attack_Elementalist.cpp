// Copyright Ashish


#include "AI/BTTask_Attack_Elementalist.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AuraGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"


EBTNodeResult::Type UBTTask_Attack_Elementalist::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	const int32 MinionTotal = ICombatInterface::Execute_GetMinionCount(ControlledPawn);
	FGameplayTagContainer Tags;
	if (MinionTotal < MinionSpawnThreshold)
	{
		Tags.AddTag(FAuraGameplayTags::Get().Abilities_Summon);
	}
	else
	{
		Tags.AddTag(FAuraGameplayTags::Get().Abilities_Attack);
	}

	ASC->TryActivateAbilitiesByTag(Tags);
	return EBTNodeResult::Succeeded;
}

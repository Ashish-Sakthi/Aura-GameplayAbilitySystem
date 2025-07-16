// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"	
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;
/**
 * @class AAuraEnemy
 * @brief Represents an enemy character in the game, extending base character functionality with combat and AI-specific features.
 *
 * AAuraEnemy inherits from AAuraCharacterBase and implements the IEnemyInterface to provide enemy-specific behaviors.
 * This class includes functionality for AI behavior, combat-related mechanics, and interaction with gameplay tags.
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	virtual void PossessedBy(AController* NewController) override;

	/* Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	/* End of Enemy Interface */

	/* Combat Interface */
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	/* End of Combat Interface */
	
	UPROPERTY(BlueprintReadOnly , Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	
};

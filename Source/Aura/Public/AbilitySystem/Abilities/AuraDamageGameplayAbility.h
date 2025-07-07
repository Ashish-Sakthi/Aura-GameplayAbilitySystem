// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * @class UAuraDamageGameplayAbility
 * @brief A subclass of UAuraGameplayAbility that provides functionality for applying damage to target actors.
 *
 * This class represents a gameplay ability that applies damage to one or more target actors based on defined damage types
 * and a gameplay effect. It allows specifying how different types of damage are scaled using scalable values
 * and implements functionality for applying the damage through a gameplay effect system.
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TMap<FGameplayTag , FScalableFloat> DamageTypes;
};

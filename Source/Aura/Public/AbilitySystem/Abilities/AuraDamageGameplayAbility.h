// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * UAuraDamageGameplayAbility
 *
 * A type of gameplay ability that allows for dealing damage to target actors.
 * This class manages the application of damage effects and the configuration of
 * damage types with scalable parameters.
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

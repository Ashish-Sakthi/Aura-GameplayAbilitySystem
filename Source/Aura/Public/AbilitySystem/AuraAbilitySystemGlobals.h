// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * UAuraAbilitySystemGlobals
 *
 * This class is a custom implementation of UAbilitySystemGlobals designed for the Aura game framework.
 * It overrides functionality to allocate a specialized gameplay effect context that provides extended
 * functionality specific to the Aura project.
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};

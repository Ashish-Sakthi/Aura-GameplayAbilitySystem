// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraMeleeAttack.generated.h"

/**
 * @class UAuraMeleeAttack
 *
 * @brief Represents a melee attack gameplay ability within the Aura game framework.
 *
 * UAuraMeleeAttack is a specialized class derived from UAuraDamageGameplayAbility.
 * This class is designed to handle melee attack-specific logic and integrates with
 * the overarching gameplay ability system in the Aura framework.
 *
 * Inherits functionalities such as initiating damage effects from UAuraDamageGameplayAbility.
 * The class supports integration with gameplay tags and scalable attributes, enabling
 * flexible configuration and interaction within gameplay scenarios.
 *
 * It serves as a foundational class for implementing melee attack mechanics tied
 * to abilities in the game and can be further extended for additional functionality.
 */
UCLASS()
class AURA_API UAuraMeleeAttack : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
};

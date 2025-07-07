// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * @class UExecCalc_Damage
 * @brief A custom execution calculation class for handling damage-related gameplay effects.
 *
 * This class is derived from UGameplayEffectExecutionCalculation and is responsible for executing
 * damage-related calculations in gameplay effects. It captures relevant attributes and performs
 * calculations such as applying resistances, critical hits, and block chances.
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

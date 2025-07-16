// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * UAuraGameplayAbility is a subclass of UGameplayAbility that provides extended
 * functionality for abilities within the Aura framework. It includes support
 * for an input tag that can be associated with the ability to define how it
 * should be triggered.
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;
	
};

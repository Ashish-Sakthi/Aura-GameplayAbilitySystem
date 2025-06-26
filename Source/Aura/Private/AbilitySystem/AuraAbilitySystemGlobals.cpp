// Copyright Ashish


#include "AbilitySystem/AuraAbilitySystemGlobals.h"
#include "AuraAbilityTypes.h"

//Used to tell the engine to use Custom effect context instead of regular.
//Need to configure DefaultGame.ini
FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraGameplayEffectContext();
}

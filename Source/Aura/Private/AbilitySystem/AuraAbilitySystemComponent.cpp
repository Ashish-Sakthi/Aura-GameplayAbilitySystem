// Copyright Ashish


#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraAbilitySystemComponent::AbilityActorInfo_OnSet()
{
	// Delegate Called whenever GE applies an effect to the ASC	
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

//Passes the tags of the GE to the widget via the delegate using the tag container.
void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);//Asset tag does not assign, it passes the tags.
	EffectAssetTags.Broadcast(TagContainer);	
}

// Copyright Ashish


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfo_OnSet()
{
	// Delegate Called whenever GE applies an effect to the ASC	
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

//Adds the ability to the character only on the server.
/**
 * Adds gameplay abilities to the character at startup
 * Only executes on the server
 * @param StartupAbilities Array of abilities to add
 */
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	//Checks the list of abilities in Character BP.
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//Adds the input tag to the ability spec to check if it is equal to the tag in any
			//DynamicAbilityTags is just a container of tags in the ability spec.
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

/**
 * Handles when an input mapped to an ability is held down
 * Activates the ability if it's not already active
 * @param InputTag The gameplay tag associated with the input
 */
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//Check if any ability spec has the input tag of InParam so that we can activate it.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
	
}

/**
 * Handles when an input mapped to an ability is released
 * @param InputTag The gameplay tag associated with the input
 */
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//Tells the ability that the input has been released.
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

//Passes the tags of the GE to the widget via the delegate using the tag container.Used
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);//Asset tag does not assign, it passes the tags.
	EffectAssetTags.Broadcast(TagContainer);	
}

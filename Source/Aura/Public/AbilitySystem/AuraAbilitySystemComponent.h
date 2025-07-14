// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& /*AssetTags*/)
/* A Multicast delegate that broadcasts when abilities are given to a character, with the AuraAbilitySystemComponent as a parameter */
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven, UAuraAbilitySystemComponent*)
/* Delegate used to iterate through gameplay abilities, taking a single FGameplayAbilitySpec parameter */
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&)

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	//Used to bind with delegates when the ability actor info is set.
	void AbilityActorInfo_OnSet();

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
protected:

	/*
	 *Telling it is a client and need to receive conformation even if packet loss or network error.
	 *Implementing it so that the message is shown in the client view.As OnGameplayEffectAppliedDelegateToSelf is a server side delegate.
	 * Need client prefix and _Implementation postfix in function name to work.
	 */
	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};

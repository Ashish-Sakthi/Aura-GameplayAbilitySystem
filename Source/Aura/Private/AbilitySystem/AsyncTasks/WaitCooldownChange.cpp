// Copyright Ashish


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

/**
 * Factory method to create and initialize a cooldown change monitor
 * @param AbilitySystemComponent The GAS component to monitor
 * @param InCooldownTag The tag that represents the cooldown to track
 * @return New WaitCooldownChange object or nullptr if invalid parameters
 */
UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
    const FGameplayTag& InCooldownTag)
{
    // Create a new instance and initialize basic properties
    UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
    WaitCooldownChange->ASC = AbilitySystemComponent;
    WaitCooldownChange->CooldownTag = InCooldownTag;
    
    // Validate input parameters
    if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
    {
        WaitCooldownChange->EndTask();
        return nullptr;
    }

    // Register for tag removal events to detect when the cooldown ends . Delegate also use for hit react in enemy.
    AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag,EGameplayTagEventType::NewOrRemoved)
                            .AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChanged);

    // Register for new effect events to detect when cooldown starts
    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange,
                             &UWaitCooldownChange::OnActiveEffectAdded);

    return WaitCooldownChange;
}

/**
 * Cleanup method to properly dispose of the monitor
 * Unregisters events and marks object for garbage collection
 */
void UWaitCooldownChange::EndTask()
{
    if (!IsValid(ASC)) return;
    // Unregister from tag events to prevent memory leaks
    ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

    SetReadyToDestroy();
    MarkAsGarbage();
}

/**
 * Callback for when the cooldown tag state changes
 * @param InCooldownTag The tag that changed
 * @param NewCount New count of the tag (0 means cooldown ended)
 */
void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
    // When the tag count reaches 0, the cooldown has ended
    if (NewCount == 0)
    {
        CooldownEnd.Broadcast(0.f);
    }
}

/**
 * Callback for when a new gameplay effect is added
 * Checks if the effect is related to our cooldown and calculates remaining time
 */
void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    // Get all tags associated with the new effect
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags);

    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags);

    // Check if the effect is related to our cooldown tag
    if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
    {
        // Query for all active effects with our cooldown tag
        FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
        TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
        
        if (TimesRemaining.Num() > 0)
        {
            // Find the longest remaining cooldown time
            float TimeRemaining = TimesRemaining[0];
            for (int32 i = 0; i < TimesRemaining.Num(); i++)
            {
                if (TimesRemaining[i] > TimeRemaining)
                {
                    TimeRemaining = TimesRemaining[i];
                }
            }
            
            // Broadcast cooldown start with the remaining time
            CooldownStart.Broadcast(TimeRemaining);
        }
    }
}
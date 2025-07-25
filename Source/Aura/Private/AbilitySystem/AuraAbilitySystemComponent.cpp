// Copyright Ashish

#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

// Called when the Ability Actor Info is set or updated on this ASC
void UAuraAbilitySystemComponent::AbilityActorInfo_OnSet()
{
    // Bind delegate to receive notifications when gameplay effects are applied to self
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

// Adds gameplay abilities to the character on the server during initialization
// Only runs on the server
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    // For each ability class defined in startup list
    for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
       FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

       // Cast to AuraGameplayAbility to access custom properties like StartupInputTag
       if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
       {
          // Assign startup input tag to DynamicAbilityTags for input matching
          AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);

          // Mark ability as equipped by default using status tag
          AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);

          // Grant ability to this Ability System Component
          GiveAbility(AbilitySpec);
       }
    }

    bStartupAbilitiesGiven = true;

    // Notify that startup abilities have been granted
    AbilitiesGivenDelegate.Broadcast();
}

// Adds passive abilities to the character and activates them once immediately
void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
    {
       FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

       // Grant and activate passive abilities once at startup
       GiveAbilityAndActivateOnce(AbilitySpec);
    }
}

// Called when an input mapped to a gameplay tag is held down
// Attempts to activate matching ability if not already active
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    // Iterate all activatable abilities
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
       // Check if the ability's dynamic tags include the input tag exactly
       if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
       {
          // Register input pressed event on the ability spec
          AbilitySpecInputPressed(AbilitySpec);

          // Attempt to activate the ability if it is not already active
          if (!AbilitySpec.IsActive())
          {
             TryActivateAbility(AbilitySpec.Handle);
          }
       }
    }
}

// Called when an input mapped to a gameplay tag is released
// Notifies the ability spec of input release
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
       if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
       {
          // Notify ability spec that input has been released
          AbilitySpecInputReleased(AbilitySpec);
       }
    }
}

// Executes a provided delegate on each ability spec in the Ability System Component
// Used for operations like mapping ability spells or gathering information
void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
    // Lock the abilities list during iteration to maintain thread safety
    FScopedAbilityListLock ActiveScopeLock(*this);

    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
       // Execute the delegate for each ability spec; log error if it fails
       if (!Delegate.ExecuteIfBound(AbilitySpec))
       {
          UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
       }
    }
}

// Retrieves the primary gameplay tag associated with an ability spec
// Returns an empty tag if none found
FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    if (AbilitySpec.Ability)
    {
       // Iterate over ability tags to find one that belongs to "Abilities" category
       for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
       {
          if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
          {
             return Tag;
          }
       }
    }

    return FGameplayTag();
}

// Retrieves the input gameplay tag from dynamic ability tags of the spec
// Returns empty tag if none found
FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
    {
       if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
       {
          return Tag;
       }
    }

    return FGameplayTag();
}

// Retrieves current ability status gameplay tag from dynamic tags of the spec
FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
    {
       if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
       {
          return Tag;
       }
    }

    return FGameplayTag();
}

// Finds the ability spec that contains the given ability tag
// Returns null pointer if not found
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
    // Lock ability list for safe iteration
    FScopedAbilityListLock ActiveScopeLoc(*this);

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
       for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
       {
          if (Tag.MatchesTag(AbilityTag))
          {
             return &AbilitySpec;
          }
       }
    }

    return nullptr;
}

// Server-side function to upgrade an attribute if there are points available
void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
    // Check if avatar implements player interface to get attribute points
    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
       if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
       {
          // Invoke server RPC to upgrade attribute
          ServerUpgradeAttribute(AttributeTag);
       }
    }
}

// Updates the status of abilities based on the current character level
// Grants new abilities if level requirements are met and marks them as eligible
void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
    UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());

    // Iterate through all abilities info to check level requirement
    for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
    {
       if (!Info.AbilityTag.IsValid()) continue;     // Skip invalid tags
       if (Level < Info.LevelRequirement) continue;  // Skip if below required level

       // If ability not already granted, give ability and mark as eligible
       if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
       {
          FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);

          // Add eligible status tag to mark ability as unlocked but not equipped
          AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);

          GiveAbility(AbilitySpec);

          // Mark spec dirty for network replication
          MarkAbilitySpecDirty(AbilitySpec);

          // Notify the client about new ability status update
          ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
       }
    }
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
   FString& OutNextLevelDescription)
{
   if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
   {
      if(UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
      {
         OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
         OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
         return true;
      }
   }

   // If no ability spec found (Ability Locked), return default description for the ability tag if locked selected else return nothing.
   const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
   if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
   {
      OutDescription = FString();
   }
   else
   {
      OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
   }
   OutNextLevelDescription = FString();
   return false;
}

// Server RPC implementation to spend a spell point on a specific ability identified by its tag
void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
    if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
    {
       // Deduct spell point from player (if avatar implements player interface)
       if (GetAvatarActor()->Implements<UPlayerInterface>())
       {
          IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
       }

       const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

       // Get current status tag of the ability
       FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);

       // Change ability status or level based on current status:
       if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
       {
          // Transition from Eligible to Unlocked status
          AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
          AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
          Status = GameplayTags.Abilities_Status_Unlocked;
       }
       else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
       {
          // Increase ability level (upgrade) if already unlocked or equipped
          AbilitySpec->Level += 1;
       }

       // Propagate updated ability status and level to client
       ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);

       // Mark ability spec dirty for replication of changes
       MarkAbilitySpecDirty(*AbilitySpec);
    }
}

// Server RPC to upgrade an attribute: sends gameplay event and deducts attribute point
void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
    // Prepare gameplay event data with magnitude 1 for upgrade
    FGameplayEventData Payload;
    Payload.EventTag = AttributeTag;
    Payload.EventMagnitude = 1.f;

    // Send a gameplay event to the avatar to trigger any ability or effect related to this attribute upgrade
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

    // Deduct attribute point from player state if player interface implemented
    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
       IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
    }
}

// Called when ability activation states replicate to client
void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
    Super::OnRep_ActivateAbilities();

    // Ensure startup abilities are only initialized once on client
    if (!bStartupAbilitiesGiven)
    {
       bStartupAbilitiesGiven = true;
       AbilitiesGivenDelegate.Broadcast();
    }
}

// Client receives updated ability status and broadcasts changes to its listeners (likely UI)
void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
    const FGameplayTag& StatusTag, int32 AbilityLevel)
{
    AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

// Client method called when gameplay effect is applied to this ASC
// Extracts all asset tags and broadcasts them for UI or other systems to respond
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                      const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer TagContainer;

    // Retrieve all gameplay tags from the applied effect spec
    EffectSpec.GetAllAssetTags(TagContainer);

    // Broadcast these tags so UI or other systems can react
    EffectAssetTags.Broadcast(TagContainer);
}

#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/PlayerInterface.h"

// Called after actor info is set: binds gameplay effect application events to custom client handler
void UAuraAbilitySystemComponent::AbilityActorInfo_OnSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

// Adds a set of "base" (startup) active abilities to a character (server-only)
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
       FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);

       // If it's an Aura ability, initialize tags
       if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
       {
          // Map to hotbar slot using startup input tag, mark as equipped
          AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
          AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
          GiveAbility(AbilitySpec);
       }
    }

    bStartupAbilitiesGiven = true;
    AbilitiesGivenDelegate.Broadcast();
}

// Adds passive abilities and activate them immediately
void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
    {
       FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
       AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
       GiveAbilityAndActivateOnce(AbilitySpec);
    }
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData)
{
   // Iterate through each saved ability from the save data
   for (const FSavedAbility& Data : SaveData->SavedAbilities)
   {
      // Create a loaded ability class from saved data
      const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;

      // Create ability spec with saved level
      FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, Data.AbilityLevel);

      // Add saved ability slot and status tags
      LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilitySlot);
      LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityStatus);

      // Handle offensive abilities
      if (Data.AbilityType == FAuraGameplayTags::Get().Abilities_Type_Offensive)
      {
         GiveAbility(LoadedAbilitySpec);
      }
      // Handle passive abilities
      else if (Data.AbilityType == FAuraGameplayTags::Get().Abilities_Type_Passive)
      {
         if (Data.AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
         {
            // Activate equipped passive abilities
            GiveAbilityAndActivateOnce(LoadedAbilitySpec);
            MulticastActivatePassiveEffect(Data.AbilityTag, true);
         }
         else
         {
            // Just give unequipped passive abilities without activating
            GiveAbility(LoadedAbilitySpec);
         }
      }
   }
   // Mark startup abilities as given and broadcast delegate
   bStartupAbilitiesGiven = true;
   AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
   if (!InputTag.IsValid()) return;

   FScopedAbilityListLock ActiveScopeLoc(*this);
   for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
   {
      if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
      {
         AbilitySpecInputPressed(AbilitySpec);
         if (AbilitySpec.IsActive())
         {
            InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed,AbilitySpec.Handle,AbilitySpec.ActivationInfo.GetActivationPredictionKey());
         }
      }
   }
}

// When an input action (mapped to a tag) is pressed, try to activate corresponding abilities
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

   FScopedAbilityListLock ActiveScopeLoc(*this);
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
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

// When an input tag is released, forward release event to ability specs
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

   FScopedAbilityListLock ActiveScopeLoc(*this);
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
       if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
       {
          AbilitySpecInputReleased(AbilitySpec);
          InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased,AbilitySpec.Handle,AbilitySpec.ActivationInfo.GetActivationPredictionKey());
       }
    }
}

// Call the provided delegate with every activatable ability spec (utility function for queries/mapping)
void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
    FScopedAbilityListLock ActiveScopeLock(*this);
    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
       if (!Delegate.ExecuteIfBound(AbilitySpec))
       {
          UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
       }
    }
}

// Find and return the main "Abilities.*" tag for a spec (used for unique identification)
FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    if (AbilitySpec.Ability)
    {
       // Only return tags in the "Abilities" hierarchy
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

// Return the UI or hotbar slot tag for the ability
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

// Find current status tag (Equipped/Unlocked/Eligible/Locked) for ability
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

// Find status for a given tag by querying spec
FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
   if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
   {
      return GetStatusFromSpec(*Spec);
   }
   return FGameplayTag();
}

// Find input slot for a given tag by querying spec
FGameplayTag UAuraAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
   if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
   {
      return GetInputTagFromSpec(*Spec);
   }
   return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
   FScopedAbilityListLock ActiveScopeLoc(*this);
   for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
   {
      if (AbilityHasSlot(AbilitySpec, Slot))
      {
         return false;
      }
   }
   return true;
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
   return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
   return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
   FScopedAbilityListLock ActiveScopeLock(*this);
   for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
   {
      if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
      {
         return &AbilitySpec;
      }
   }
   return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
   const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
   const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
   const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
   const FGameplayTag AbilityType = Info.AbilityType;
   return AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
   ClearSlot(&Spec);
   Spec.DynamicAbilityTags.AddTag(Slot);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
   bool bActivate)
{
   ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

// Find the ability spec for a certain "Abilities.*" tag; returns nullptr if not found
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
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

// Tries to upgrade an attribute by spending upgrade points, if possible (server call)
void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
       if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
       {
          ServerUpgradeAttribute(AttributeTag);
       }
    }
}

// For all abilities in AbilityInfo, grant as eligible if level is high enough (called on level up)
void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
    UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());

    for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
    {
       if (!Info.AbilityTag.IsValid()) continue;
       if (Level < Info.LevelRequirement) continue;

       // Not yet learned: grant it and set status to eligible
       if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
       {
          FGameplayAbilitySpec AbilitySpec(Info.Ability, 1);
          AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
          GiveAbility(AbilitySpec);
          MarkAbilitySpecDirty(AbilitySpec);
          ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
       }
    }
}

// Requests the server to (re)assign an ability to a hotbar slot/tag
void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
   if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
   {
      const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
      const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
      const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

      // Only allow equip if ability is unlocked or already equipped
      const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;
      if (bStatusValid)
      {
         if (!SlotIsEmpty(Slot)) // There is an ability in this slot already. Deactivate and clear its slot.
         {
            FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
            if (SpecWithSlot) 
            {
               // is that ability the same as this ability? If so, we can return early.
               if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
               {
                  ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
                  return;
               }

               if (IsPassiveAbility(*SpecWithSlot))
               {
                  MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
                  DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
               }

               ClearSlot(SpecWithSlot);
            }
         }

         if (!AbilityHasAnySlot(*AbilitySpec)) // Ability doesn't yet have a slot (it's not active)
         {
            if (IsPassiveAbility(*AbilitySpec))
            {
               TryActivateAbility(AbilitySpec->Handle);
               MulticastActivatePassiveEffect(AbilityTag, true);
            }
            AbilitySpec->DynamicAbilityTags.RemoveTag(GetStatusFromSpec(*AbilitySpec));
            AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
         }
         AssignSlotToAbility(*AbilitySpec, Slot);
         MarkAbilitySpecDirty(*AbilitySpec);
      }
      ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
   }
}

// Client-side: notifies listeners that an equip operation has occurred (typically the UI)
void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
   AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

// Get the description and next level description for a given ability tag
bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                              FString& OutNextLevelDescription)
{
   if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
   {
      if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
      {
         OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
         OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
         return true;
      }
   }

   // Provide fallback locked/greyed out description (useful when ability is not yet unlocked)
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

// Removes slot tag from a single ability spec
void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
   const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
   Spec->DynamicAbilityTags.RemoveTag(Slot);
}

// Removes the slot tag from ALL abilities (ensuring a slot is never assigned to multiple abilities)
void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
   FScopedAbilityListLock ActiveScopeLock(*this);
   for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
   {
      if (AbilityHasSlot(&Spec, Slot))
      {
         ClearSlot(&Spec);
      }
   }
}

// Is the given ability assigned to a specific slot?
bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
   for (FGameplayTag Tag : Spec->DynamicAbilityTags)
   {
      if (Tag.MatchesTagExact(Slot))
      {
         return true;
      }
   }
   return false;
}

// Server method: spends a spell point to unlock or upgrade the given ability
void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
    if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
    {
       // Remove a spell point from player via attribute interface (character or player state)
       if (GetAvatarActor()->Implements<UPlayerInterface>())
       {
          IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
       }

       const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
       FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);

       // If eligible, transition to unlocked; if equipped/unlocked, increment level
       if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
       {
          AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
          AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
          Status = GameplayTags.Abilities_Status_Unlocked;
       }
       else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
       {
          AbilitySpec->Level += 1;
       }

       ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
       MarkAbilitySpecDirty(*AbilitySpec);
    }
}

// Upgrades an attribute and removes the appropriate point from player via server RPC
void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
    FGameplayEventData Payload;
    Payload.EventTag = AttributeTag;
    Payload.EventMagnitude = 1.f;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

    // Remove an attribute point from the player (attribute points)
    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
       IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
    }
}

// Called when the list of abilities is replicated to the client; used to bind UI/other listeners
void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
    Super::OnRep_ActivateAbilities();

    if (!bStartupAbilitiesGiven)
    {
       bStartupAbilitiesGiven = true;
       AbilitiesGivenDelegate.Broadcast();
    }
}

// Sends new ability status and/or level to the client so it can update UI accordingly
void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
    const FGameplayTag& StatusTag, int32 AbilityLevel)
{
    AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

// Handles notification when any gameplay effect is applied to this component (client-side only)
// Used for UI to show status effects, etc.
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                      const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);

    // Sends all tags to UI or other interested listeners
    EffectAssetTags.Broadcast(TagContainer);
}

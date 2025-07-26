#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

// Broadcasts ability information and available spell points to the UI at initialization
void USpellMenuWidgetController::BroadcastInitialValues()
{
    // Updates UI with current ability information
    BroadcastAbilityInfo();

    // Informs UI about current spell point count
    SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

// Sets up listeners to respond to game state changes (e.g., ability status or spell points)
void USpellMenuWidgetController::BindCallbacksToDependencies()
{
    // Listen for any ability status changes, update selected ability, and broadcast UI updates as needed
    GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
    {
       // If the changed ability matches the selected one, update button states and re-broadcast relevant info
       if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
       {
          SelectedAbility.Status = StatusTag;

          bool bEnableSpendPoints = false;
          bool bEnableEquip = false;
          ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

          // Fetch new ability descriptions based on the status, and update the UI
          FString Description, NextLevelDescription;
          GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
          SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
       }
       
       // If ability info is available, update it and notify listeners
       if (AbilityInfo)
       {
          FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
          Info.StatusTag = StatusTag;
          Info.InputTag = GetAuraASC()->GetInputTagFromAbilityTag(AbilityTag);
          AbilityInfoDelegate.Broadcast(Info);
       }
    });

   GetAuraASC()->AbilityEquipped.AddUObject(this,&USpellMenuWidgetController::OnAbilityEquipped);

    // Listen for changes in spell points (e.g., from leveling up or spending)
    GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 Points)
       {
          SpellPointsChangedDelegate.Broadcast(Points);
          CurrentSpellPoints = Points;
          bool bEnableSpendPoints = false;
          bool bEnableEquip = false;

          // Re-evaluate button enabled state since spell point count changed
          ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

          // Update descriptions for the current selection and inform UI
          FString Description, NextLevelDescription;
          GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
          SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
       });
}

// Triggered when user selects a spell "globe" in the UI (i.e. clicks an ability)
void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
    // If we're currently waiting to equip, stop that action before changing selection
    if (bIsWaitingForEquip)
    {
       const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
       StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
       bIsWaitingForEquip = false;
    }

    const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
    const int32 SpellPoints = GetAuraPS()->GetSpellPoints();

    FGameplayTag AbilityStatus;

    // Validate the ability selection: locked, invalid, or no ability means it can't be manipulated
    const bool bTagValid = AbilityTag.IsValid();
    const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
    const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
    const bool bSpecValid = AbilitySpec != nullptr;

    if (!bTagValid || bTagNone || !bSpecValid)
    {
       AbilityStatus = GameplayTags.Abilities_Status_Locked;
    }
    else
    {
       // Otherwise get the true status from the spec
       AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
    }

    // Store current selection in local state
    SelectedAbility.Ability = AbilityTag;
    SelectedAbility.Status = AbilityStatus;

    // Calculate updated UI button state
    bool bEnableSpendPoints = false;
    bool bEnableEquip = false;
    ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);

    // Fetch and send descriptions relevant for this selection
    FString Description, NextLevelDescription;
    GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
    SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}

// Sends request to the server to upgrade the currently selected ability
void USpellMenuWidgetController::SpendPointButtonPressed()
{
    if (GetAuraASC())
    {
       GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
    }
}

// Called when the ability globe is deselected in the UI
void USpellMenuWidgetController::GlobeDeselect()
{
   // If we were in the equip state, exit out of it on deselect
   if (bIsWaitingForEquip)
   {
      const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
      StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
      bIsWaitingForEquip = false;
   }
   SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
   SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;

   // Inform UI that nothing is selected
   SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

// User requests to equip (assign to hotbar) the selected ability
void USpellMenuWidgetController::EquipButtonPressed()
{
   // Hand off handling to UI and set waiting state
   const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
   WaitForEquipDelegate.Broadcast(AbilityType);
   bIsWaitingForEquip = true;

   // If currently equipped, remember the slot for visual feedback
   const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
   if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
   {
      SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
   }
}

// Called when the actual spell slot is picked for equip; checks if selection is valid and triggers server equip
void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
   if (!bIsWaitingForEquip) return;

   // Ensure ability type is compatible with slot (e.g., can't put offensive in passive)
   const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
   if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

   // Request server to assign ability to slot
   GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

// Handles post-server equip result: update UI and internal states for equip actions
void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
   bIsWaitingForEquip = false;

   const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

   // If replacing an existing slot, broadcast empty (NONE) info for its previous slot
   FAuraAbilityInfo LastSlotInfo;
   LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
   LastSlotInfo.InputTag = PreviousSlot;
   LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
   AbilityInfoDelegate.Broadcast(LastSlotInfo);

   // Broadcast the new ability info for the new slot
   FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
   Info.StatusTag = Status;
   Info.InputTag = Slot;
   AbilityInfoDelegate.Broadcast(Info);

   // Instruct UI to stop showing "waiting for equip"
   StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
   SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
   GlobeDeselect();
}

// Decides if the Spend Points and Equip buttons should be enabled for a given ability state and spell point count
void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
    const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

    // Equipped: can always equip, and spend points if any available
    if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
    {
       bShouldEnableEquipButton = true;
       if (SpellPoints > 0) bShouldEnableSpellPointsButton = true;
    }
    // Eligible: only spend points if available, cannot equip
    else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
    {
       if (SpellPoints > 0) bShouldEnableSpellPointsButton = true;
    }
    // Unlocked: can equip at any time, can upgrade if points available
    else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
    {
       bShouldEnableEquipButton = true;
       if (SpellPoints > 0) bShouldEnableSpellPointsButton = true;
    }
}

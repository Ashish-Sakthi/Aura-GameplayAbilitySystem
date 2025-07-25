// Copyright Ashish

#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

// Broadcasts the initial state of abilities and current spell points to UI elements
void USpellMenuWidgetController::BroadcastInitialValues()
{
    // Broadcasts ability information to UI
    BroadcastAbilityInfo();

    // Broadcast current spell points from the player state
    SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

// Binds delegates and lambda callbacks to handle changes in abilities and spell points
void USpellMenuWidgetController::BindCallbacksToDependencies()
{
    // Subscribe to ability status changes from the Ability System Component
    GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
    {
       // Check if the changed ability matches the currently selected one
       if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
       {
          SelectedAbility.Status = StatusTag;

          bool bEnableSpendPoints = false;
          bool bEnableEquip = false;

          // Determine if buttons should be enabled based on new status and current spell points
          ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

          // Broadcast updated button states
          FString Description;
          FString NextLevelDescription;
          GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
          SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
       }
       
       // If ability info is available, update and broadcast new info to UI
       if (AbilityInfo)
       {
          FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
          Info.StatusTag = StatusTag;
          AbilityInfoDelegate.Broadcast(Info);
       }
    });

    // Subscribe to spell points changes from the Player State
    GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 Points)
       {
          // Update and broadcast new spell points
          SpellPointsChangedDelegate.Broadcast(Points);
          CurrentSpellPoints = Points;

          bool bEnableSpendPoints = false;
          bool bEnableEquip = false;

          // Update button enable states based on current selected ability status and new spell points
          ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
          FString Description;
          FString NextLevelDescription;
          GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
          SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
       });
}

// Called when a spell globe is selected in the UI
void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
    const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
    const int32 SpellPoints = GetAuraPS()->GetSpellPoints();

    FGameplayTag AbilityStatus;
    
    // Validate the ability tag; set status to Locked if invalid or none or no ability spec exists
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
       // Get current status tag from ability spec
       AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
    }

    // Update selected ability and its status
    SelectedAbility.Ability = AbilityTag;
    SelectedAbility.Status = AbilityStatus;

    bool bEnableSpendPoints = false;
    bool bEnableEquip = false;

    // Determine button enable state based on status and spell points
    ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);

    // Notify UI about enabled buttons for the selected ability
    FString Description;
    FString NextLevelDescription;
    GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
    SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}

// Called when the spend point button is pressed; sends server command to spend spell point on selected ability
void USpellMenuWidgetController::SpendPointButtonPressed()
{
    if (GetAuraASC())
    {
       // Requests server to spend a spell point for the currently selected ability
       GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
    }
}

void USpellMenuWidgetController::GlobeDeselect()
{
   SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
   SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;
   SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

// Determines whether the Spend Points and Equip buttons should be enabled based on ability status and spell points
void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
    const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

    // For Equipped abilities: enable equip always; enable spend points if any points available
    if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
    {
       bShouldEnableEquipButton = true;

       if (SpellPoints > 0)
       {
          bShouldEnableSpellPointsButton = true;
       }
    }
    // For Eligible abilities: only enable spend points if available
    else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
    {
       if (SpellPoints > 0)
       {
          bShouldEnableSpellPointsButton = true;
       }
    }
    // For Unlocked abilities: enable equip always; also enable spend points if available
    else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
    {
       bShouldEnableEquipButton = true;

       if (SpellPoints > 0)
       {
          bShouldEnableSpellPointsButton = true;
       }
    }
}

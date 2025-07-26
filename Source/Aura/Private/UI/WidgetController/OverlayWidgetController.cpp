// Copyright Ashish

//Controller for Overlay Widgets.

#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

//Constructed in AuraHUD.cpp

void UOverlayWidgetController::BroadcastInitialValues()
{
	//Broadcast Health Values and called in the blueprint of HealthGlobe.
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}

		);	
	

	//Delegate to update the values only when AttributeSet value changed to the widget.
	//AddLambda create an anonymous function and call it instead of creating a new function.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);

	if(GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		//If abilities given properly dont need to wait for the broadcast
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		//If abilities not given properly wait for the before broadcast wait until given.
		else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}
		
		//EffectAssetTags delegate gets called when the GE is applied to ASC in AuraASC.cpp.
		GetAuraASC()->EffectAssetTags.AddLambda( 
			[this](const FGameplayTagContainer& TagContainer)
			{
				for (FGameplayTag Tag : TagContainer)
				{
					// "Message.1".MatchesTag("Message") will return True, "Message".MatchesTag("Message.1") will return False
					const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(TEXT("Message"));
					if(Tag.MatchesTag(MessageTag))
					{
						//This is used to broadcast the data row that matches the tag to the Widgets.
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
						OnMessageWidgetDataChanged.Broadcast(*Row);
					}
				}
			}
		);
	}

	
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	// Cast the PlayerState to AuraPlayerState and get the LevelUpInfo data table
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	// Safety check to ensure LevelUpInfo is properly set in Blueprint
	checkf(LevelUpInfo, TEXT("Unabled to find LevelUpInfo. Please fill out AuraPlayerState Blueprint"));

	// Find the current level based on total XP and get the maximum possible level
	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInfos.Num();

	// Make sure we're within the valid level range (not 0 or exceeding max level)
	if (Level <= MaxLevel && Level > 0)
	{
	    // Get the total XP required to reach the next level
	    const int32 LevelUpRequirement = LevelUpInfo->LevelUpInfos[Level].LevelUpRequirement;
	    // Get the total XP that was required to reach the current level
	    const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInfos[Level - 1].LevelUpRequirement;

	    // Calculate how much XP is needed to progress through the current level.
	    // Example: If Level 2 needs 1000 XP and Level 3 needs 2500 XP.
	    // DeltaLevelRequirement would be 1500 XP (2500-1000)
	    const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;

	    // Calculate how much XP player has earned in the current level
	    // Example: If player has 1800 total XP and Level 2 started at 1000 XP
	    // XPForThisLevel would be 800 XP (1800-1000).
	    const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

	    // Convert the progress to a percentage (0.0 to 1.0)
	    // Example: 800 XP earned in a level that needs 1500 XP
	    // Would result in 0.533... (about 53.3% through the level)
	    const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

	    // Broadcast the percentage to update UI elements (like XP bar)
	    OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}

// Copyright Ashish

//Controller for Overlay Widgets.

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

//Constructed in AuraHUD.cpp

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	//Broadcast Health Values and called in the blueprint of HealthGlobe.
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}

		);	
	
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	//Delegate to update the values only when AttributeSet value changed to the widget.
	//AddLambda create an anonymous function and call it instead of creating a new function.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);

	if(UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		//If abilities given properly dont need to wait for the broadcast
		if (AuraASC->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		//If abilities not given properly wait for the before broadcast wait until given.
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}
		
		//EffectAssetTags delegate gets called when the GE is applied to ASC in AuraASC.cpp.
		AuraASC->EffectAssetTags.AddLambda( 
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

//Called when the startup abilities are given.
void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	// Early return if startup abilities haven't been given yet
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;

	// Create delegate to process each ability
	FForEachAbility BroadcastDelegate;
	// Bind lambda to process each ability spec. Execute lamda.
	BroadcastDelegate.BindLambda([this, AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		// Get the ability tag from the ability spec
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		// Set the input tag associated with this ability
		Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		// Broadcast the ability info to update UI
		AbilityInfoDelegate.Broadcast(Info);
	});

	// Iterate through all abilities and execute the delegate for each one
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	// Cast the PlayerState to AuraPlayerState and get the LevelUpInfo data table
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
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
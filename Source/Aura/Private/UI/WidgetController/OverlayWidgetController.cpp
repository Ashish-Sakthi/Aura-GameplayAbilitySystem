// Copyright Ashish

//Controller for Overlay Widgets.

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

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
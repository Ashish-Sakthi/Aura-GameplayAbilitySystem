// Copyright Ashish


#include "UI/WidgetController/AttributeInfoWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeInfoWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	//The map contains tags and GetAttribute Function Pointers.
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key,Pair.Value());
	}
}

//Live Updates the attribute info widgets.
void UAttributeInfoWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Pair : AS->TagsToAttributes)
	{
		//Bind the every attribute to the attribute change delegate.
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
		[this, Pair](const FOnAttributeChangeData& Data)
				{
					BroadcastAttributeInfo(Pair.Key,Pair.Value());
				}
		);
	}
}

//Gets the key and value of the map and broadcasts the attribute info to the widgets.
void UAttributeInfoWidgetController::BroadcastAttributeInfo(FGameplayTag AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag,false);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}


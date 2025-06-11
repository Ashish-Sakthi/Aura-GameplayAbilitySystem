// Copyright Ashish


#include "UI/WidgetController/AttributeInfoWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeInfoWidgetController::BroadcastInitialValues()
{
	//Need AS to Set the value in data asset struct
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AS);

	/*
	 * Get the struct from DA and Set the value then broadcast it.
	 * FAuraGameplayTags::Get() returns struct of gameplay tags and can access tags as variables from AGTags.h.
	 */
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength,false);
	Info.AttributeValue = AS->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeInfoWidgetController::BindCallbacksToDependencies()
{
	
}

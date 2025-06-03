// Copyright Ashish

/*
 * Base Controller.
 * Holds common UI control logic
 * Stores essential references to:
	- Player Controller
	- Player State
	- Ability System Component
	- Attribute Set
*/

#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

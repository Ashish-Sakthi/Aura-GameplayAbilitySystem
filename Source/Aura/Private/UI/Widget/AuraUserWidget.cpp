// Copyright Ashish

/*
 * Base class for all custom UI widgets
 * Contains:
	- A reference to its controller
	- Functions to set up the controller
*/

#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetController_OnSet();
}

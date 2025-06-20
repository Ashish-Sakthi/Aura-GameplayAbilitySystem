// Copyright Ashish

/*
 *Acts as the main coordinator for UI elements
 *Has two main responsibilities:
	- Creating and managing the overlay widget
	- Managing the widget controller
*/

#include "UI/Hud/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeInfoWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

//Creates the controller and returns it.
UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

//Creates the controller and returns it.Used in BP from Blueprint library function.
UAttributeInfoWidgetController* AAuraHUD::GetAttributeInfoWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeInfoWidgetController == nullptr)
	{
		AttributeInfoWidgetController = NewObject<UAttributeInfoWidgetController>(this,AttributeInfoWidgetControllerClass);
		AttributeInfoWidgetController->SetWidgetControllerParams(WCParams);
		AttributeInfoWidgetController->BindCallbacksToDependencies();
	}
	return AttributeInfoWidgetController;
}

//Called after initializing valid values for PC, PS, ASC, AS in AuraCharacter.cpp
void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass,TEXT("OverlayWidgetClass is not set, Fill in BP_AuraHUD.h"));
	checkf(OverlayWidgetControllerClass,TEXT("OverlayWidgetControllerClass is not set, Fill in BP_AuraHUD.h"));

	//Create the overlay widget of type UUserWidget.
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);

	//Struct to set values for the controller created in AuraWidgetController.h
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS); 
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	//Set the controller to the overlay widget.
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	OverlayWidget->SetWidgetController(WidgetController);

	//Broadcast initial values to the widgets from the controller via dynamic delegates after setting the controller.
	WidgetController->BroadcastInitialValues();

	Widget->AddToViewport();
}
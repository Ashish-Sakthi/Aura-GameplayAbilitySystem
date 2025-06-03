// Copyright Ashish

/*
 *Acts as the main coordinator for UI elements
 *Has two main responsibilities:
	- Creating and managing the overlay widget
	- Managing the widget controller
*/

#include "UI/Hud/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

//Called after initializing valid values for PC, PS, ASC, AS in AuraCharacter.
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

	//Broadcast initial values to the widgets from the controller via dynamic delegates.
	WidgetController->BroadcastInitialValues();

	Widget->AddToViewport();
}



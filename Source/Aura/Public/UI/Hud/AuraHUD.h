// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeInfoWidgetController;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class UAttributeSet;
class UAuraUserWidget;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:

	//Creates/returns the widget controller
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeInfoWidgetController* GetAttributeInfoWidgetController(const FWidgetControllerParams& WCParams);

	//Sets up the main overlay UI
	void InitOverlay(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS);

protected:


private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeInfoWidgetController> AttributeInfoWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeInfoWidgetController> AttributeInfoWidgetControllerClass;
	
};

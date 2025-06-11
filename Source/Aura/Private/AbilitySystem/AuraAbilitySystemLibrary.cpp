// Copyright Ashish


#include "AbilitySystem/AuraAbilitySystemLibrary.h"	
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/Hud/AuraHUD.h"

//Creates a Function that is blueprint pure and static.
UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* GetWorldContextObject)
{
	//Getting the Local Player Controller
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorldContextObject,0))
	{
		if (AAuraHUD* AuraHud = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = Cast<AAuraPlayerState>(PC->PlayerState);
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
			return AuraHud->GetOverlayWidgetController(WidgetControllerParams);//Creates the widget controller if not created.
		}
	}
	return nullptr;
}

UAttributeInfoWidgetController* UAuraAbilitySystemLibrary::GetAttributeInfoWidgetController(
	const UObject* GetWorldContextObject)
{
	//Getting the Local Player Controller
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorldContextObject,0))
	{
		if (AAuraHUD* AuraHud = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = Cast<AAuraPlayerState>(PC->PlayerState);
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
			return AuraHud->GetAttributeInfoWidgetController(WidgetControllerParams);//Creates the widget controller if not created.
		}
	}
	return nullptr;
}
